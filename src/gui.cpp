#include "graphics/gui/gui.h"

// Local Includes
#include "graphics/graphics.h"
#include "graphics/render/drawbatcher.h"
#include "graphics/render/shader.h"
#include "math/math.h"

namespace blons
{
GUI::GUI(int width, int height, std::unique_ptr<Shader> ui_shader, RenderContext& context)
{
    font_list_.fallback = nullptr;
    font_list_.heading = nullptr;
    font_list_.label = nullptr;
    font_list_.console = nullptr;

    width_ = width;
    height_ = height;
    ortho_matrix_ = MatrixOrthographic(static_cast<float>(width_), static_cast<float>(height_),
                                       kScreenNear, kScreenDepth);

    ui_shader_ = std::move(ui_shader);
}

GUI::~GUI()
{
}

bool GUI::LoadFont(const char* filename, int pixel_size, RenderContext& context)
{
    return LoadFont(filename, FontType::DEFAULT, pixel_size, context);
}

bool GUI::LoadFont(const char* filename, FontType usage, int pixel_size, RenderContext& context)
{
    auto font = std::unique_ptr<Font>(new Font(filename, pixel_size, context));

    switch (usage)
    {
    case HEADING:
        font_list_.heading = std::move(font);
        break;
    case LABEL:
        font_list_.label = std::move(font);
        break;
    case CONSOLE:
        font_list_.console = std::move(font);
        break;
    case DEFAULT:
    default:
        font_list_.fallback = std::move(font);
        break;
    }

    return true;
}

void GUI::Render(RenderContext& context)
{
    static DrawBatcher batchie(context);
    auto render_text = [&](int x, int y, std::string words)
    {
        for (const auto& c : words)
        {
            batchie.Append(*font_list_.fallback->BuildSprite(c, x, y)->mesh());
            x += font_list_.fallback->advance();
        }
    };
    render_text(20, 527, "EVERY MORNING I WAKE UP AND OPEN PALM SLAM A VHS INTO ");
    //for (int i = 0; i < 30; i++)
        render_text(20, 492, "THE SLOT. ITS CHRONICLES OF RIDDICK AND RIGHT THEN ");
    render_text(20, 457, "AND THERE I START DOING THE MOVES ALONGSIDE THE MAIN CHARACTER, RIDDICK. I D");
    render_text(20, 422, "> _");

    ui_shader_->SetInput("world_matrix", MatrixIdentity(), context);
    ui_shader_->SetInput("proj_matrix", ortho_matrix_, context);
    ui_shader_->SetInput("diffuse", font_list_.fallback->texture(), context);
    ui_shader_->SetInput("text_colour", Vector3(1.0, 1.0, 1.0), context);
    ui_shader_->SetInput("is_text", true, context);

    batchie.Render(context);
    ui_shader_->Render(batchie.index_count(), context);
}
} // namespace blons