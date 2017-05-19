////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/graphics/graphics.h>

// Public Includes
#include <blons/graphics/gui/gui.h>
#include <blons/graphics/light.h>
#include <blons/graphics/pipeline/pipeline.h>
#include <blons/graphics/render/shader.h>
#include <blons/math/math.h>
#include <blons/system/job.h>
#include <blons/temphelpers.h>
// Local Includes
#include "render/rendererd3d11.h"
#include "render/renderergl43.h"
#include "resource.h"

namespace blons
{
namespace
{
auto const cvar_exposure = console::RegisterVariable("light:exposure", -15.0f);
auto const cvar_sun_luminance = console::RegisterVariable("light:sun-luminance", 1e5f);
auto const cvar_sky_luminance = console::RegisterVariable("light:sky-luminance", 2e4f);
} // namespace

// Managed assets that allows the blons::Graphics class to track and render anything it creates
class ManagedModel : public Model
{
public:
    ManagedModel(std::string filename) : Model(filename) {}
    ~ManagedModel() override;

    void Finish();
    std::function<void(ManagedModel*)> deleter_;
};

class ManagedSprite : public Sprite
{
public:
    ManagedSprite(std::string filename, TextureType::Options options) : Sprite(filename, options) {}
    ManagedSprite(std::string filename) : Sprite(filename) {}
    ManagedSprite(const PixelData& pixel_data) : Sprite(pixel_data) {}
    ~ManagedSprite() override;

    void Finish();
    std::function<void(ManagedSprite*)> deleter_;
};

Graphics::Graphics(Client::Info screen)
{
    if (!Init(screen))
    {
        log::Fatal("Failed to initialize rendering context\n");
        throw "Failed to initiralize rendering context";
    }

    pipeline_.reset(new pipeline::Deferred(screen, kPi / 4.0f, pipeline::kScreenNear, pipeline::kScreenFar));

    // Camera
    camera_.reset(new Camera);
    if (camera_ == nullptr)
    {
        throw "Failed to initialize camera";
    }

    camera_->set_pos(0.0f, 2.0f, 0.0f);

    // Sunlight
    // TODO: User should be doing this somehow
    sun_.reset(new Light(Light::DIRECTIONAL,
                         Vector3(20.0f, 20.0f, 5.0f), // position
                         // Midnight
                         //Vector3(-10.0f, 0.0f, -5.0f), // direction
                         //Vector3(0.1f, 0.05f, 0.03f))); // colour
                         // Sunset
                         //Vector3(-10.0f, -2.0f, -5.0f), // direction
                         //Vector3(1.0f, 0.75, 0.4f))); // colour
                         // Noon
                         Vector3(-2.0f, -5.0f, -0.5f),
                         Vector3(1.0f, 0.95f, 0.8f), // colour
                         cvar_sun_luminance->to<float>())); // intensity in candela/meters^2
    // Sunny day
    Vector3 sky_colour(0.3f, 0.6f, 1.0f);
    // Midnight
    //Vector3 sky_colour(0.1f, 0.1f, 0.4f);

    // Describes a function f(x) = 1.0
    SHCoeffs3 uniform_function;
    uniform_function.coeffs[0] = 2.0f * sqrt(kPi);
    // Generate skybox coefficients by weighting uniform function with desired sky colour
    for (int i = 0; i < sizeof(uniform_function.coeffs) / sizeof(decltype(uniform_function.coeffs[0])); i++)
    {
        sky_box_.r.coeffs[i] = uniform_function.coeffs[i] * sky_colour.r;
        sky_box_.g.coeffs[i] = uniform_function.coeffs[i] * sky_colour.g;
        sky_box_.b.coeffs[i] = uniform_function.coeffs[i] * sky_colour.b;
    }
    sky_luminance_ = cvar_sky_luminance->to<float>();
}

Graphics::~Graphics()
{
    // If graphics gets deleted before its models & sprites, make sure
    // they're cleaned up safely
    while (models_.size() > 0)
    {
        auto m = *models_.begin();
        m->Finish();
        // Just to be safe
        models_.erase(m);
    }
    while (sprites_.size() > 0)
    {
        auto s = *sprites_.begin();
        s->Finish();
        // Just to be safe
        sprites_.erase(s);
    }
}

void Graphics::BakeRadianceTransfer()
{
    pipeline::Scene scene;
    scene.lights = { sun_.get() };
    scene.models.assign(models_.begin(), models_.end());
    scene.sky_box = sky_box_;
    scene.sky_luminance = sky_luminance_;
    scene.view = *camera_;

    pipeline_->BakeRadianceTransfer(scene);
}

std::unique_ptr<Model> Graphics::MakeModel(std::string filename)
{
    auto model = new ManagedModel(filename);
    model->deleter_ = [&](ManagedModel* m)
    {
        models_.erase(m);
    };
    models_.insert(model);
    return std::unique_ptr<Model>(model);
}

namespace
{
// Apparently you can't use a template to define a set of overloaded methods... so we have this instead
template<typename... Args>
std::unique_ptr<Sprite> BuildSprite(std::set<ManagedSprite*>& sprites, Args... args)
{
    auto sprite = new ManagedSprite(args...);
    sprite->deleter_ = [&](ManagedSprite* s)
    {
        sprites.erase(s);
    };
    sprites.insert(sprite);
    return std::unique_ptr<Sprite>(sprite);
}
} // namespace

std::unique_ptr<Sprite> Graphics::MakeSprite(std::string filename, TextureType::Options options)
{
    return BuildSprite(sprites_, filename, options);
}

std::unique_ptr<Sprite> Graphics::MakeSprite(std::string filename)
{
    return BuildSprite(sprites_, filename);
}

std::unique_ptr<Sprite> Graphics::MakeSprite(const PixelData& pixel_data)
{
    return BuildSprite(sprites_, pixel_data);
}

bool Graphics::Render()
{
    // Start performance profiling frame
    perf_timers_[perf_timers_index_].Start();

    // Update graphics settings from cvars
    camera_->set_exposure(cvar_exposure->to<float>());
    sun_->set_luminance(cvar_sun_luminance->to<float>());
    sky_luminance_ = cvar_sky_luminance->to<float>();

    auto context = render::context();
    pipeline::Scene scene;
    scene.lights = { sun_.get() };
    scene.models.assign(models_.begin(), models_.end());
    scene.sky_box = sky_box_;
    scene.sky_luminance = sky_luminance_;
    scene.view = *camera_;

    // Update performance metrics
    auto oldest_frame_index = (perf_timers_index_ + 1) % kPerfTimerBuffer;
    const auto& root = perf_timers_[oldest_frame_index].root();
    // Buld up the UI's draw batches while we do all of the scene rendering
    Job gui_batch_job([&]()
    {
        debug_overlay_->UpdateMetrics(root);
        gui_->BuildDrawCalls();
    });
    gui_batch_job.Enqueue();

    // Clear buffers
    context->BeginScene(Vector4(0, 0, 0, 1));

    // Render the scene
    performance::PushMarker("Deferred pipeline");
    if (!pipeline_->Render(scene, output_buffer_.get()))
    {
        performance::PopMarker();
        return false;
    }
    performance::PopMarker();

    output_buffer_->Bind(false);

    // Render all 2D sprites
    if (!RenderSprites())
    {
        return false;
    }

    // Render the GUI
    performance::PushMarker("User Interface");
    gui_batch_job.Wait();
    gui_->Render(output_buffer_.get());
    performance::PopMarker();

    // Output the main FBO to the backbuffer
    context->BindFramebuffer(nullptr);
    output_buffer_->Render();

    // Set the inputs
    if (!sprite_shader_->SetInput("proj_matrix", ortho_matrix_) ||
        !sprite_shader_->SetInput("sprite", output_buffer_->textures()[0]))
    {
        return false;
    }

    if (!sprite_shader_->Render(output_buffer_->index_count()))
    {
        return false;
    }

    // End performance profiling frame before buffer
    // swap since that idles the CPU on GPU bound pipelines
    perf_timers_[perf_timers_index_].End();
    perf_timers_index_ = (perf_timers_index_ + 1) % kPerfTimerBuffer;
    // Swap buffers
    context->EndScene();

    return true;
}

bool Graphics::RenderSprites()
{
    // Needed so sprites can render over themselves
    render::context()->SetDepthTesting(false);

    for (const auto& sprite : sprites_)
    {
        // Prep the pipeline 4 drawering
        sprite->Render();

        // Set the inputs
        if (!sprite_shader_->SetInput("proj_matrix", ortho_matrix_) ||
            !sprite_shader_->SetInput("sprite", sprite->texture()))
        {
            return false;
        }

        // Make the draw call
        if (!sprite_shader_->Render(sprite->index_count()))
        {
            return false;
        }
    }
    return true;
}

void Graphics::Reload(Client::Info screen)
{
    log::Debug("Reloading ... ");
    render::MakeContext(screen);
    Timer timer;
    resource::ClearBufferCache();
    Init(screen);
    pipeline_->Reload(screen, kPi / 4.0f, pipeline::kScreenNear, pipeline::kScreenFar);
    for (auto& m : models_)
    {
        m->Reload();
    }
    for (auto& s : sprites_)
    {
        s->Reload();
    }
    log::Debug("%ims!\n", timer.ms());
}

Camera* Graphics::camera() const
{
    return camera_.get();
}

gui::Manager* Graphics::gui() const
{
    return gui_.get();
}

void Graphics::set_output(pipeline::Deferred::Output output, pipeline::Deferred::Output alt_output)
{
    pipeline_->set_output(output, alt_output);
}

bool Graphics::Init(Client::Info screen)
{
    screen_ = screen;
    auto context = render::context();

    // Ortho projection matrix (for 2d stuff, shadow maps, etc)
    ortho_matrix_ = MatrixOrthographic(0, units::pixel_to_subpixel(screen.width), units::pixel_to_subpixel(screen.height), 0,
                                       pipeline::kScreenNear, pipeline::kScreenFar);

    // Shaders
    ShaderAttributeList sprite_inputs = { { POS, "input_pos" },
                                          { TEX, "input_uv" } };
    sprite_shader_.reset(new Shader({ { VERTEX, "shaders/sprite.vert.glsl" }, { PIXEL, "shaders/sprite.frag.glsl" } }, sprite_inputs));

    if (sprite_shader_ == nullptr)
    {
        return false;
    }

    // GUI
    if (gui_ == nullptr)
    {
        gui_.reset(new gui::Manager(screen.width, screen.height));
        debug_overlay_ = new gui::DebugOverlay("", Box(0, 0, screen.width, screen.height), gui::Window::INVISIBLE, gui_.get());
        gui_->AddOverlay(std::unique_ptr<gui::DebugOverlay>(debug_overlay_));
    }
    else
    {
        gui_->Reload(screen.width, screen.height);
    }

    output_buffer_.reset(new Framebuffer(screen.width, screen.height, 1, false));

    for (auto& frame : perf_timers_)
    {
        frame.Start();
        frame.End();
    }
    perf_timers_index_ = 0;

    return true;
}

void ManagedModel::Finish()
{
    if (deleter_ != nullptr)
    {
        deleter_(this);
        deleter_ = nullptr;
    }

    mesh_.reset();
    albedo_texture_.reset();
    normal_texture_.reset();
    light_texture_.reset();
}

ManagedModel::~ManagedModel()
{
    if (deleter_ != nullptr)
    {
        deleter_(this);
    }
}

void ManagedSprite::Finish()
{
    if (deleter_ != nullptr)
    {
        deleter_(this);
        deleter_ = nullptr;
    }

    buffer_.reset();
    texture_.reset();
}

ManagedSprite::~ManagedSprite()
{
    if (deleter_ != nullptr)
    {
        deleter_(this);
    }
}
} // namespace blons
