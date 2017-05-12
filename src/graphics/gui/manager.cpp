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

#include <blons/graphics/gui/manager.h>

// Includes
#include <numeric>
// Public Includes
#include <blons/graphics/graphics.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/render/shader.h>
#include <blons/math/math.h>

namespace blons
{
namespace gui
{
namespace
{
    // Blur is rendered at screen_res / kBlurFactor
    // Saves cycles & strengthens blur
    static const float kBlurFactor = 3.0f;
    static const int kBlurFactori = static_cast<int>(kBlurFactor);
    static const int kBlurIterations = 2;
}

Manager::Manager(units::pixel width, units::pixel height)
{
    Init(width, height);
    // TODO: Make the windows resize on reload
    main_window_.reset(new Window("main", Box(0.0f, 0.0f, screen_dimensions_.w, screen_dimensions_.h), Window::INVISIBLE, this));
    console_window_.reset(new ConsoleWindow("main", Box(0.0f, 0.0f, screen_dimensions_.w, screen_dimensions_.h / 3), Window::INVISIBLE, this));
}

void Manager::Init(units::pixel width, units::pixel height)
{
    screen_dimensions_ = Box(0, 0, width, height);
    ortho_matrix_ = MatrixOrthographic(0, screen_dimensions_.w, screen_dimensions_.h, 0,
                                       pipeline::kScreenNear, pipeline::kScreenFar);
    blur_ortho_matrix_ = MatrixOrthographic(0, screen_dimensions_.w / kBlurFactori, screen_dimensions_.h / kBlurFactori, 0,
                                            pipeline::kScreenNear, pipeline::kScreenFar);

    ShaderAttributeList ui_inputs = { { POS, "input_pos" },
                                      { TEX, "input_uv" } };
    ui_shader_.reset(new Shader({ { VERTEX, "shaders/ui.vert.glsl" }, { PIXEL, "shaders/ui.frag.glsl" } }, ui_inputs));
    blur_shader_.reset(new Shader({ { VERTEX, "shaders/sprite.vert.glsl" }, { PIXEL, "shaders/ui-blur.frag.glsl" } }, ui_inputs));
    composite_shader_.reset(new Shader({ { VERTEX, "shaders/sprite.vert.glsl" }, { PIXEL, "shaders/ui-composite.frag.glsl" } }, ui_inputs));

    ui_buffer_.reset(new Framebuffer(width, height, { { TextureType::R8G8B8A8, TextureType::LINEAR, TextureType::CLAMP } }, true));
    blur_buffer_a_.reset(new Framebuffer(width / kBlurFactori, height / kBlurFactori, { { TextureType::R8G8B8A8, TextureType::LINEAR, TextureType::CLAMP } }, false));
    blur_buffer_b_.reset(new Framebuffer(width / kBlurFactori, height / kBlurFactori, { { TextureType::R8G8B8A8, TextureType::LINEAR, TextureType::CLAMP } }, false));

    batch_shader_data_ = nullptr;

    skin_.reset(new Skin());

    // TODO: move this out of constructor! font load included!
    LoadFont("font stuff/test-console.ttf", 28, Skin::FontStyle::DEFAULT);
    LoadFont("font stuff/test-heading.ttf", 20, Skin::FontStyle::HEADING);
    LoadFont("font stuff/test-label.ttf", 20, Skin::FontStyle::LABEL);
    LoadFont("font stuff/test-console.ttf", 28, Skin::FontStyle::CONSOLE);

    // For batch rendering
    quad_mesh_.reset(new Mesh("blons:quad"));

    // Reset batches
    draw_batches_.clear();
    batch_index_ = 0;
}

Manager::~Manager()
{
}

bool Manager::LoadFont(std::string filename, units::pixel pixel_size, Skin::FontStyle style)
{
    return skin_->LoadFont(filename, pixel_size, style);
}

bool Manager::LoadFont(std::string filename, units::pixel pixel_size)
{
    return LoadFont(filename, pixel_size, Skin::FontStyle::DEFAULT);
}

Window* Manager::MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption, Window::Type type)
{
    Box win_pos(x, y, width, height);
    Window* temp_win;
    if (type == Window::DRAGGABLE)
    {
        temp_win = new Window(id, win_pos, caption, this);
    }
    else
    {
        temp_win = new Window(id, win_pos, type, this);
    }
    auto new_window = std::unique_ptr<Window>(temp_win);

    for (auto& w : windows_)
    {
        if (w->id() == id)
        {
            w = std::move(new_window);
            return w.get();
        }
    }
    windows_.push_back(std::move(new_window));
    return windows_.back().get();
}

Window* Manager::MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption)
{
    return MakeWindow(id, x, y, width, height, caption, Window::DRAGGABLE);
}

Window* Manager::MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, Window::Type type)
{
    return MakeWindow(id, x, y, width, height, "", type);
}

void Manager::Render(Framebuffer* output_buffer)
{
    auto context = render::context();
    context->SetDepthTesting(true);
    context->SetBlendMode(BlendMode::ALPHA);
    // Rendering UI to a separate buffer is needed for styles
    // Styles are only possible when output_buffer is supplied
    if (output_buffer != nullptr)
    {
        ui_buffer_->Bind(Vector4(0.0));
    }
    else
    {
        context->BindFramebuffer(nullptr);
    }

    // Generate draw calls if not already done so
    BuildDrawCalls();

    // Only bother drawing if there's things to draw
    if (batch_index_ > 0)
    {
        context->SetDepthTesting(false);
        // Resize shader data if necessary
        if (batch_shader_data_ == nullptr || batch_shader_data_->length() < batch_index_)
        {
            batch_shader_data_.reset(new ShaderData<InternalDrawCallInputs>(nullptr, batch_index_));
        }
        // Upload to GPU
        batch_shader_data_->set_value(draw_batches_.data(), 0, batch_index_);

        // Bind the quad mesh for instanced rendering
        context->BindMeshBuffer(quad_mesh_->buffer());

        // Draw pass
        if (!ui_shader_->SetInput("proj_matrix", ortho_matrix_) ||
            !ui_shader_->SetInput("drawcall_buffer", batch_shader_data_->data()) ||
            !ui_shader_->SetInput("skin[0]", TextureFromID(0), 0) ||
            !ui_shader_->SetInput("skin[1]", TextureFromID(1), 1) ||
            !ui_shader_->SetInput("skin[2]", TextureFromID(2), 2) ||
            !ui_shader_->SetInput("skin[3]", TextureFromID(3), 3) ||
            !ui_shader_->SetInput("skin[4]", TextureFromID(4), 4))
        {
            throw "Failed to set UI shader inputs";
        }
        // Run the shader
        if (!ui_shader_->RenderInstanced(quad_mesh_->index_count(), static_cast<unsigned int>(batch_index_)))
        {
            throw "UI shader failed to render";
        }
    }
    // Reset batch marker to 0
    batch_index_ = 0;

    // Cannot apply UI styles when rendering to backbuffer
    if (output_buffer == nullptr)
    {
        return;
    }
    // Achieve a fast, strong blur by rendering at a low resolution and skipping over pixels
    // Artifacting is resolved by iterating multiple times, which also further strengthens blur
    context->SetBlendMode(OVERWRITE);
    blur_shader_->SetInput("proj_matrix", blur_ortho_matrix_);
    for (int i = 0; i < kBlurIterations; i++)
    {
        // It is safe to never clear buffers since it's based off a clean input texture
        blur_buffer_a_->Bind(false);
        blur_buffer_a_->Render();
        if (i == 0)
        {
            blur_shader_->SetInput("composite", output_buffer->textures()[0], 0);
            blur_shader_->SetInput("ui", ui_buffer_->textures()[0], 1);
        }
        else
        {
            blur_shader_->SetInput("composite", blur_buffer_b_->textures()[0], 0);
            blur_shader_->SetInput("ui", blur_buffer_b_->textures()[0], 1);
        }
        // Uniform inputs are faster than alternating 2 separate shaders
        blur_shader_->SetInput("horizontal", 1.0f);
        blur_shader_->SetInput("screen_length", screen_dimensions_.w / kBlurFactor);
        blur_shader_->Render(blur_buffer_a_->index_count());

        blur_buffer_b_->Bind(false);
        blur_buffer_b_->Render();
        blur_shader_->SetInput("composite", blur_buffer_a_->textures()[0], 0);
        blur_shader_->SetInput("ui", blur_buffer_a_->textures()[0], 1);
        blur_shader_->SetInput("horizontal", 0.0f);
        blur_shader_->SetInput("screen_length", screen_dimensions_.h / kBlurFactor);
        blur_shader_->Render(blur_buffer_b_->index_count());
    }
    context->SetBlendMode(ALPHA);
    output_buffer->Bind(false);
    output_buffer->Render();
    composite_shader_->SetInput("proj_matrix", ortho_matrix_);
    composite_shader_->SetInput("blurred_composite", blur_buffer_b_->textures()[0], 0);
    composite_shader_->SetInput("ui", ui_buffer_->textures()[0], 1);
    composite_shader_->Render(output_buffer->index_count());
}

void Manager::Render()
{
    Render(nullptr);
}

void Manager::BuildDrawCalls()
{
    // Draw calls have already been made
    if (batch_index_ > 0)
    {
        return;
    }

    // TODO: Remove concept of a main window. That's dumb!
    // Main window always renders at the bottom
    main_window_->Render();
    // User made windows
    for (const auto& w : windows_)
    {
        if (!w->hidden())
        {
            w->Render();
        }
    }
    // Console window always renders on top
    if (!console_window_->hidden())
    {
        console_window_->Render();
    }
}

void Manager::Reload(units::pixel screen_width, units::pixel screen_height)
{
    Init(screen_width, screen_height);
}

bool Manager::Update(const Input& input)
{
    // TODO: Replace when we have input binding system
    for (const auto& e : input.event_queue())
    {
        if (e.type == Input::Event::KEY_DOWN &&
            e.value == Input::KeyCode::SYMBOL_GRAVE_ACCENT)
        {
            if (console_window_->hidden())
            {
                console_window_->show();
            }
            else
            {
                console_window_->hide();
            }
            return true;
        }
    }
    // Update backwards, since last element is top window
    // We don't want input to be sent to windows underneath the one yr clickin on
    if (!console_window_->hidden() && console_window_->Update(input))
    {
        return true;
    }
    for (auto w = windows_.rbegin(); w != windows_.rend(); w++)
    {
        if (!w->get()->hidden() && w->get()->Update(input))
        {
            return true;
        }
    }
    return false;
}

Window* Manager::window(std::string id)
{
    for (const auto& w : windows_)
    {
        if (w->id() == id)
        {
            return w.get();
        }
    }
    return nullptr;
}

// This is setup to recycle memory as much as we can. Used like a C array
// that is only resized when the total batches for a frame is greater
// than any previous frame. Can only be reset manually by calling ClearBatches().
void Manager::SubmitBatch(const Box& pos, const Box& uv, const DrawCallInputs& inputs)
{
    InternalDrawCallInputs gpu_inputs;
    gpu_inputs.is_text = inputs.is_text;
    gpu_inputs.crop_feather = inputs.crop_feather;
    gpu_inputs.colour = inputs.colour;
    gpu_inputs.pos = pos;
    gpu_inputs.uv = uv;
    gpu_inputs.crop = inputs.crop;
    gpu_inputs.texture_id = TranslateToTextureID(inputs.font_style, inputs.is_text);
    // Normalize uv to [0,1]
    auto font = skin_->font(inputs.font_style);
    Vector2 skin_dimensions(units::pixel_to_subpixel(skin_->texture()->info()->width), units::pixel_to_subpixel(skin_->texture()->info()->height));
    Vector2 font_dimensions(units::pixel_to_subpixel(font->texture_info()->width), units::pixel_to_subpixel(font->texture_info()->height));
    Vector2 texture_dimensions = (inputs.is_text ? font_dimensions : skin_dimensions);
    gpu_inputs.uv.x /= texture_dimensions.x;
    gpu_inputs.uv.y /= texture_dimensions.y;
    gpu_inputs.uv.w /= texture_dimensions.x;
    gpu_inputs.uv.h /= texture_dimensions.y;

    if (batch_index_ < draw_batches_.size())
    {
        draw_batches_[batch_index_] = gpu_inputs;
    }
    else
    {
        draw_batches_.push_back(gpu_inputs);
    }

    batch_index_++;
}

void Manager::SubmitControlBatch(const Box& pos, const Box& uv, const Box& crop, const units::pixel& feather)
{
    DrawCallInputs inputs = { false, Skin::FontStyle::DEFAULT, Vector4(), crop, feather };
    SubmitBatch(pos, uv, inputs);
}

void Manager::SubmitFontBatch(const Box& pos, const Box& uv, const Skin::FontStyle& style, const Vector4& colour, const Box& crop, const units::pixel& feather)
{
    DrawCallInputs inputs = { true, style, colour, crop, feather };
    SubmitBatch(pos, uv, inputs);
}

int Manager::TranslateToTextureID(const Skin::FontStyle& style, bool is_text)
{
    if (!is_text)
    {
        return 0;
    }
    switch (style)
    {
    case Skin::FontStyle::DEFAULT:
        return 1;
    case Skin::FontStyle::HEADING:
        return 2;
    case Skin::FontStyle::LABEL:
        return 3;
    case Skin::FontStyle::CONSOLE:
        return 4;
    default:
        throw "Unknown font style in UI batch id";
    }
}

const TextureResource* Manager::TextureFromID(int texture_id)
{
    switch (texture_id)
    {
    case 0:
        return skin_->texture()->texture();
    case 1:
        return skin_->font(Skin::FontStyle::DEFAULT)->texture();
    case 2:
        return skin_->font(Skin::FontStyle::HEADING)->texture();
    case 3:
        return skin_->font(Skin::FontStyle::LABEL)->texture();
    case 4:
        return skin_->font(Skin::FontStyle::CONSOLE)->texture();
    default:
        throw "Unknown batch id in UI rendering";
    }
}

Skin* Manager::skin() const
{
    return skin_.get();
}

Window* Manager::active_window() const
{
    if (windows_.size() > 0)
    {
        return windows_.back().get();
    }
    return main_window_.get();
}

void Manager::set_active_window(Window* window)
{
    // Finds function param in window vector, then swaps its place with last element
    // Uses v.end()-1 bcus no point switchin last element with itself
    for (auto w = windows_.begin(); w < windows_.end() - 1; w++)
    {
        if (w->get() == window)
        {
            std::iter_swap(w, windows_.end() - 1);
        }
    }
}

Box Manager::screen_dimensions()
{
    return screen_dimensions_;
}
} // namespace gui
} // namespace blons
