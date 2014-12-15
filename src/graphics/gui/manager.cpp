#include <blons/graphics/gui/manager.h>

// Public Includes
#include <blons/graphics/graphics.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/render/shader.h>
#include <blons/math/math.h>

namespace blons
{
namespace gui
{
Manager::Manager(units::pixel width, units::pixel height, std::unique_ptr<Shader> ui_shader, RenderContext& context)
{
    screen_dimensions_ = Box(0, 0, width, height);
    ortho_matrix_ = MatrixOrthographic(screen_dimensions_.w, screen_dimensions_.h,
                                       kScreenNear, kScreenDepth);

    ui_shader_ = std::move(ui_shader);

    skin_ = std::unique_ptr<Skin>(new Skin(context));

    // TODO: move this out of constructor! font load included!
    LoadFont("../../notes/font stuff/test-console.ttf", 28, FontStyle::DEFAULT, context);
    LoadFont("../../notes/font stuff/test-heading.ttf", 14, FontStyle::HEADING, context);
    LoadFont("../../notes/font stuff/test-label.ttf", 20, FontStyle::LABEL, context);
    LoadFont("../../notes/font stuff/test-console.ttf", 28, FontStyle::CONSOLE, context);
    // TODO: get rid of main_window... i think
    main_window_ = std::unique_ptr<Window>(new Window("main", Box(0.0f, 0.0f, screen_dimensions_.w, screen_dimensions_.h), WindowType::INVISIBLE, this));
    console_window_ = std::unique_ptr<Window>(new ConsoleWindow("main", Box(0.0f, 0.0f, screen_dimensions_.w, screen_dimensions_.h / 3), WindowType::INVISIBLE, this));
}

Manager::~Manager()
{
}

bool Manager::LoadFont(std::string filename, units::pixel pixel_size, FontStyle style, RenderContext& context)
{
    return skin_->LoadFont(filename, pixel_size, style, context);
}

bool Manager::LoadFont(std::string filename, units::pixel pixel_size, RenderContext& context)
{
    return LoadFont(filename, pixel_size, FontStyle::DEFAULT, context);
}

Window* Manager::MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption, WindowType type)
{
    Box win_pos(x, y, width, height);
    Window* temp_win;
    if (type == WindowType::DRAGGABLE)
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
    return MakeWindow(id, x, y, width, height, caption, WindowType::DRAGGABLE);
}

Window* Manager::MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, WindowType type)
{
    return MakeWindow(id, x, y, width, height, "", type);
}

void Manager::Render(RenderContext& context)
{
    // Main window always renders at the bottom
    main_window_->Render(context);
    // User made windows
    for (const auto& w : windows_)
    {
        if (!w->hidden())
        {
            w->Render(context);
        }
    }
    // Console window always renders on top
    if (!console_window_->hidden())
    {
        console_window_->Render(context);
    }

    // Draw pass
    ui_shader_->SetInput("world_matrix", MatrixIdentity(), context);
    ui_shader_->SetInput("proj_matrix", ortho_matrix_, context);
    for (auto& batch : draw_batches_)
    {
        if (batch.first.is_text)
        {
            auto font = skin_->font(batch.first.font_style);
            ui_shader_->SetInput("diffuse", font->texture(), context);
            ui_shader_->SetInput("is_text", true, context);
            ui_shader_->SetInput("text_colour", batch.first.colour, context);
        }
        else
        {
            ui_shader_->SetInput("is_text", false, context);
            ui_shader_->SetInput("diffuse", skin_->sprite()->texture(), context);
        }
        ui_shader_->SetInput("crop", batch.first.crop, context);
        ui_shader_->SetInput("feather", batch.first.crop_feather, context);

        batch.second->Render(context);
        ui_shader_->Render(batch.second->index_count(), context);
    }

    draw_batches_.clear();
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
    for (auto& w : windows_)
    {
        if (w->id() == id)
        {
            return w.get();
        }
    }
    return nullptr;
}

void Manager::RegisterDrawCall(DrawCallInputs info, DrawBatcher* batch)
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

Box Manager::screen_dimensions()
{
    return screen_dimensions_;
}
} // namespace gui
} // namespace blons
