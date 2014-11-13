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
    font_list_[DEFAULT] = nullptr;
    font_list_[HEADING] = nullptr;
    font_list_[LABEL] = nullptr;
    font_list_[CONSOLE] = nullptr;

    width_ = width;
    height_ = height;
    ortho_matrix_ = MatrixOrthographic(static_cast<float>(width_), static_cast<float>(height_),
                                       kScreenNear, kScreenDepth);

    ui_shader_ = std::move(ui_shader);

    temp_labels_.push_back(Label(20, 527, "EVERY MORNING I WAKE UP AND OPEN PALM SLAM A VHS INTO ", this));
    //for (int i = 0; i < 30; i++)
    temp_labels_.push_back(Label(20, 492, "THE SLOT. ITS CHRONICLES OF RIDDICK AND RIGHT THEN ", this));
    temp_labels_.push_back(Label(20, 457, "AND THERE I START DOING THE MOVES ALONGSIDE THE MAIN CHARACTER, RIDDICK. I D", this));
    temp_labels_.push_back(Label(20, 422, "> _", this));
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
    font_list_[usage] = std::unique_ptr<Font>(new Font(filename, pixel_size, context));

    return true;
}

Font* Manager::GetFont(FontType usage)
{
    auto& font = font_list_[usage];
    if (font != nullptr)
    {
        return font.get();
    }
    return font_list_[DEFAULT].get();
}

DrawBatcher* Manager::GetFontBatch(FontType usage, Vector4 colour, RenderContext& context)
{
    FontCall call = { usage, colour };
    auto index_match = font_batches_.find(call);
    if (index_match != font_batches_.end())
    {
        return index_match->second.get();
    }
    else
    {
        auto batcher = std::unique_ptr<DrawBatcher>(new DrawBatcher(context));
        font_batches_.insert(font_batches_.begin(),
                             std::make_pair(call, std::move(batcher)));
        return font_batches_.begin()->second.get();
    }

    return nullptr;
}

void Manager::Render(RenderContext& context)
{
    for (auto& label : temp_labels_)
    {
        label.Render(context);
    }

    // Font pass
    ui_shader_->SetInput("world_matrix", MatrixIdentity(), context);
    ui_shader_->SetInput("proj_matrix", ortho_matrix_, context);
    ui_shader_->SetInput("is_text", true, context);
    for (auto& batch : font_batches_)
    {
        auto font = GetFont(batch.first.usage);
        ui_shader_->SetInput("diffuse", font->texture(), context);
        ui_shader_->SetInput("text_colour", batch.first.colour, context);

        batch.second->Render(context);
        ui_shader_->Render(batch.second->index_count(), context);
    }
}
} // namespace GUI
} // namespace blons