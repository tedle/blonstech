#include "graphics/gui/gui.h"

// Local Includes
#include "graphics/graphics.h"
#include "graphics/render/drawbatcher.h"
#include "graphics/render/shader.h"
#include "math/math.h"

namespace blons
{
namespace GUI
{
Manager::Manager(int width, int height, std::unique_ptr<Shader> ui_shader, RenderContext& context)
{
    screen_dimensions_ = Vector2(static_cast<float>(width), static_cast<float>(height));
    ortho_matrix_ = MatrixOrthographic(screen_dimensions_.x, screen_dimensions_.y,
                                       kScreenNear, kScreenDepth);

    ui_shader_ = std::move(ui_shader);

    skin_ = std::unique_ptr<Skin>(new Skin(context));

    // TODO: move this out of constructor! font load included!
    LoadFont("../../notes/font stuff/test.ttf", 28, context);
    main_window_ = std::unique_ptr<Window>(new Window(0, 0, width, height, WindowType::INVISIBLE, this));
    windows_.push_back(std::unique_ptr<Window>(new Window(20, 80, 400, 200, WindowType::DRAGGABLE, this)));
    windows_.push_back(std::unique_ptr<Window>(new Window(450, 250, 300, 300, WindowType::DRAGGABLE, this)));
    windows_[0].get()->CreateLabel(10, 70, "HAello! std::move(test);");
    windows_[0].get()->CreateButton(10, 100, 120, 60, "Button!");
}

Manager::~Manager()
{
}

bool Manager::LoadFont(const char* filename, int pixel_size, RenderContext& context)
{
    return LoadFont(filename, FontType::DEFAULT, pixel_size, context);
}

bool Manager::LoadFont(const char* filename, FontType usage, int pixel_size, RenderContext& context)
{
    return skin_->LoadFont(filename, usage, pixel_size, context);
}

void Manager::Render(RenderContext& context)
{
    main_window_->Render(context);
    for (const auto& w : windows_)
    {
        w->Render(context);
    }

    // Draw pass
    ui_shader_->SetInput("world_matrix", MatrixIdentity(), context);
    ui_shader_->SetInput("proj_matrix", ortho_matrix_, context);
    for (auto& batch : draw_batches_)
    {
        if (batch.first.is_text)
        {
            auto font = skin_->font(batch.first.usage);
            ui_shader_->SetInput("diffuse", font->texture(), context);
            ui_shader_->SetInput("is_text", true, context);
            ui_shader_->SetInput("text_colour", batch.first.colour, context);
        }
        else
        {
            ui_shader_->SetInput("is_text", false, context);
            ui_shader_->SetInput("diffuse", skin_->sprite()->texture(), context);
        }

        batch.second->Render(context);
        ui_shader_->Render(batch.second->index_count(), context);
    }

    draw_batches_.clear();
}

bool Manager::Update(const Input& input)
{
    // Update backwards, since last element is top window
    // We don't want input to be sent to windows underneath the one yr clickin on
    for (auto w = windows_.rbegin(); w != windows_.rend(); w++)
    {
        if (w->get()->Update(input))
        {
            return true;
        }
    }
    return false;
}

void Manager::RegisterDrawCall(DrawCallInfo info, DrawBatcher* batch)
{
    draw_batches_.push_back(std::make_pair(info, batch));
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

Vector2 Manager::screen_dimensions()
{
    return screen_dimensions_;
}
} // namespace GUI
} // namespace blons