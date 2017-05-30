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
    static const int kBlurIterations = 3;
}

Manager::Manager(units::pixel width, units::pixel height)
{
    Init(width, height);
    // TODO: Make the windows and overlays resize on reload
    main_window_.reset(new Window(Box(0.0f, 0.0f, screen_dimensions_.w, screen_dimensions_.h), Window::INVISIBLE, this));
    console_window_.reset(new ConsoleWindow(Box(0.0f, 0.0f, screen_dimensions_.w, screen_dimensions_.h / 3), Window::INVISIBLE, this));
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

    batches_.shader_data = nullptr;

    skin_.reset(new Skin());

    // TODO: move this out of constructor! font load included!
    LoadFont("font stuff/test-console.ttf", 28, Skin::FontStyle::DEFAULT);
    LoadFont("font stuff/test-heading.ttf", 20, Skin::FontStyle::HEADING);
    LoadFont("font stuff/test-label.ttf", 20, Skin::FontStyle::LABEL);
    LoadFont("font stuff/test-console.ttf", 28, Skin::FontStyle::CONSOLE);
    LoadFont("font stuff/test-console.ttf", 12, Skin::FontStyle::DEBUG);

    // For batch rendering
    quad_mesh_.reset(new Mesh("blons:quad"));

    // Reset batches
    batches_.inputs.clear();
    batches_.image_list.clear();
    batches_.index = 0;
    batches_.image_index = 0;
    batches_.split_markers.clear();

    // Reload all stored Images
    for (auto& tex : image_handles_)
    {
        tex->Reload();
    }
    // TODO: Add injectable shader #defines to clean up things like this
    const int kMaxShaderTextures = 32; // This is the hard-coded size of the texture array in the shaders/ui.frag.glsl
    max_texture_slots_ = std::min(render::context()->max_texture_slots(), kMaxShaderTextures);
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

Window* Manager::AddWindow(std::unique_ptr<Window> window)
{
    for (auto& w : windows_)
    {
        if (w == window)
        {
            w = std::move(window);
            return w.get();
        }
    }
    windows_.push_back(std::move(window));
    return windows_.back().get();
}

Window* Manager::MakeWindow(units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption, Window::Type type)
{
    Box win_pos(x, y, width, height);
    if (type == Window::DRAGGABLE)
    {
        return AddWindow(std::make_unique<Window>(win_pos, caption, this));
    }
    else
    {
        return AddWindow(std::make_unique<Window>(win_pos, type, this));
    }
    throw "Impossible statement reached";
}

Window* Manager::MakeWindow(units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption)
{
    return MakeWindow(x, y, width, height, caption, Window::DRAGGABLE);
}

Window* Manager::MakeWindow(units::pixel x, units::pixel y, units::pixel width, units::pixel height, Window::Type type)
{
    return MakeWindow(x, y, width, height, "", type);
}

Overlay* Manager::AddOverlay(std::unique_ptr<Overlay> overlay)
{
    overlays_.push_back(std::move(overlay));
    return overlays_.back().get();
}

Overlay* Manager::MakeOverlay()
{
    return AddOverlay(std::make_unique<Overlay>(this));
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

    // Draw pass
    if (batches_.index > 0)
    {
        // Initialize the pipeline state
        context->SetDepthTesting(false);
        // Resize shader data if necessary
        if (batches_.shader_data == nullptr || batches_.shader_data->length() < batches_.index)
        {
            batches_.shader_data.reset(new ShaderData<InternalDrawCallInputs>(nullptr, batches_.index));
        }
        // Upload to GPU
        batches_.shader_data->set_value(batches_.inputs.data(), 0, batches_.index);
        // Constant uniforms
        if (!ui_shader_->SetInput("proj_matrix", ortho_matrix_) ||
            !ui_shader_->SetInput("drawcall_buffer", batches_.shader_data->data()) ||
            !ui_shader_->SetInput("skin[0]", TextureFromID(0), 0) ||
            !ui_shader_->SetInput("skin[1]", TextureFromID(1), 1) ||
            !ui_shader_->SetInput("skin[2]", TextureFromID(2), 2) ||
            !ui_shader_->SetInput("skin[3]", TextureFromID(3), 3) ||
            !ui_shader_->SetInput("skin[4]", TextureFromID(4), 4) ||
            !ui_shader_->SetInput("skin[5]", TextureFromID(5), 5))
        {
            throw "Failed to set UI shader inputs";
        }

        // Loop thru every batch instance and render it
        int image_iterator = 0;
        int completed_instances = 0;
        for (const auto& marker : batches_.split_markers)
        {
            if (!ui_shader_->SetInput("batch_offset", completed_instances))
            {
                throw "Failed to set UI shader inputs";
            }
            // This works by building a complete list of images needed to render during the frame by
            // making an insertion to batches_.image_list on each SubmitImageBatch() call. Split
            // markers are inserted to batches_.split_markers each time the image list size hits a multiple
            // of (max_texture_slots_ - kReservedTextureSlots), marking a point where a new rendering batch is
            // needed. Now we iterate over the entire list, marking our place to continue where we left off
            // after each batch is completed. Really ugly control flow, but it's efficient. Could do with some
            // abstractions to make it safer to work with.
            int texture_slots = max_texture_slots_ - kReservedTextureSlots;
            while (image_iterator < batches_.image_index)
            {
                int tex_id = image_iterator % texture_slots;
                tex_id += kReservedTextureSlots;
                std::string uniform = "skin[" + std::to_string(tex_id) + "]";
                if (!ui_shader_->SetInput(uniform.c_str(), batches_.image_list[image_iterator]->texture(), tex_id))
                {
                    throw "Failed to set UI shader inputs";
                }
                image_iterator++;
                if (image_iterator % texture_slots == 0)
                {
                    break;
                }
            }
            // Bind the quad mesh for instanced rendering
            context->BindMeshBuffer(quad_mesh_->buffer());
            // Run the shader
            if (!ui_shader_->RenderInstanced(quad_mesh_->index_count(), marker - completed_instances))
            {
                throw "UI shader failed to render";
            }
            completed_instances = marker;
        }
    }
    // Reset batch marker to 0
    batches_.index = 0;
    batches_.image_index = 0;
    batches_.split_markers.clear();

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
    if (batches_.index > 0)
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
    // Overlays render above the rest of the UI
    for (const auto& w : overlays_)
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

    // Make a split marker to denote the end of the batch range
    batches_.split_markers.push_back(static_cast<unsigned int>(batches_.index));
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
    for (auto w = overlays_.rbegin(); w != overlays_.rend(); w++)
    {
        if (!w->get()->hidden() && w->get()->Update(input))
        {
            return true;
        }
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

    if (batches_.index < batches_.inputs.size())
    {
        batches_.inputs[batches_.index] = gpu_inputs;
    }
    else
    {
        batches_.inputs.push_back(gpu_inputs);
    }

    batches_.index++;
}

void Manager::SubmitControlBatch(const Box& pos, const Box& uv, const Box& crop, const units::pixel& feather)
{
    DrawCallInputs inputs = { false, Skin::FontStyle::DEFAULT, Vector4(), crop, feather };
    SubmitBatch(pos, uv, inputs);
}

void Manager::SubmitImageBatch(const Box& pos, const Box& uv, const Box& crop, const units::pixel& feather, const Image::InternalHandle& image)
{
    // Image specific batch bits
    int tex_id = batches_.image_index % (max_texture_slots_ - kReservedTextureSlots);
    tex_id += kReservedTextureSlots;
    if (batches_.image_index < batches_.image_list.size())
    {
        batches_.image_list[batches_.image_index] = image.tex_.get();
    }
    else
    {
        batches_.image_list.push_back(image.tex_.get());
    }
    batches_.image_index++;

    // GPU specific batch bits
    InternalDrawCallInputs gpu_inputs;
    gpu_inputs.is_text = false;
    gpu_inputs.crop_feather = feather;
    gpu_inputs.colour = Vector4();
    gpu_inputs.pos = pos;
    gpu_inputs.uv = uv;
    gpu_inputs.crop = crop;
    gpu_inputs.texture_id = tex_id;

    if (batches_.index < batches_.inputs.size())
    {
        batches_.inputs[batches_.index] = gpu_inputs;
    }
    else
    {
        batches_.inputs.push_back(gpu_inputs);
    }
    batches_.index++;

    // Insert a split marker if the next image will loop over the max texture slots
    if ((tex_id + 1) % max_texture_slots_ == 0)
    {
        batches_.split_markers.push_back(static_cast<unsigned int>(batches_.index));
    }
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
    case Skin::FontStyle::DEBUG:
        return 5;
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
    case 5:
        return skin_->font(Skin::FontStyle::DEBUG)->texture();
    default:
        throw "Unknown batch id in UI rendering";
    }
}

std::unique_ptr<Image::InternalHandle> Manager::RegisterInternalImage(const std::string& filename)
{
    TextureType::Options opts;
    opts.compression = TextureType::RAW;
    opts.filter = TextureType::LINEAR;
    opts.wrap = TextureType::CLAMP;

    auto tex = std::make_unique<Texture>(filename, opts);
    image_handles_.insert(tex.get());
    return std::make_unique<Image::InternalHandle>(std::move(tex), this);
}

std::unique_ptr<Image::InternalHandle> Manager::RegisterInternalImage(const PixelData& pixel_data)
{
    auto tex = std::make_unique<Texture>(pixel_data);
    image_handles_.insert(tex.get());
    return std::make_unique<Image::InternalHandle>(std::move(tex), this);
}

void Manager::FreeInternalImage(Image::InternalHandle* image)
{
    image_handles_.erase(image->tex_.get());
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
    // Finds function param in vector, then moves it to the end while
    // preserving the order of other elements
    // Uses v.end()-1 bcus no point switchin last element with itself
    for (auto w = windows_.begin(); w < windows_.end() - 1; w++)
    {
        if (w->get() == window)
        {
            auto window_ptr = std::unique_ptr<Window>(w->release());
            windows_.erase(w);
            windows_.push_back(std::move(window_ptr));
            return;
        }
    }
}

Box Manager::screen_dimensions()
{
    return screen_dimensions_;
}
} // namespace gui
} // namespace blons
