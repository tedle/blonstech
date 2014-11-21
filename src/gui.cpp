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
struct FontCall
{
    FontType usage;
    Vector4 colour;
    // needed for efficient std::map lookups
    bool operator< (const FontCall call) const { return memcmp(this, &call, sizeof(FontCall))>0; }
};

Manager::Manager(int width, int height, std::unique_ptr<Shader> ui_shader, RenderContext& context)
{
    width_ = width;
    height_ = height;
    ortho_matrix_ = MatrixOrthographic(static_cast<float>(width_), static_cast<float>(height_),
                                       kScreenNear, kScreenDepth);

    ui_shader_ = std::move(ui_shader);

    skin_ = std::unique_ptr<Skin>(new Skin(context));

    control_batch_ = std::unique_ptr<DrawBatcher>(new DrawBatcher(context));

    temp_window_ = std::unique_ptr<Window>(new Window(100, 100, 200, 200, WindowType::DRAGGABLE, this));
    //temp_labels_.push_back(Label(20, 100, "$47A!$27C!$65A!$967!$AEFn$7D4i$D3Dc$EE8e$FB52$8BE01$7FA3", this));
    temp_window_->CreateLabel(20, 527, "EVERY MORNING $0f0I WAKE UP AND $000PALM SLAM A VHS INTO ");
    //for (int i = 0; i < 30; i++)
    temp_window_->CreateLabel(20, 492, "$700THE SLOT. ITS OF RIDDICK AND RIGHT THEN $aa");
    temp_window_->CreateLabel(20, 457, "$00fAND THERE I START MAIN $fffCHARACTER, RIDDICK. I D$aaa");
    temp_window_->CreateLabel(20, 422, "> _");
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

DrawBatcher* Manager::font_batch(FontType usage, Vector4 colour, RenderContext& context)
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

DrawBatcher* Manager::control_batch() const
{
    return control_batch_.get();
}

Skin* Manager::skin() const
{
    return skin_.get();
}

void Manager::Render(RenderContext& context)
{
    temp_window_->Render(context);

    // Control pass
    ui_shader_->SetInput("world_matrix", MatrixIdentity(), context);
    ui_shader_->SetInput("proj_matrix", ortho_matrix_, context);
    ui_shader_->SetInput("is_text", false, context);
    ui_shader_->SetInput("diffuse", skin_->sprite()->texture(), context);

    control_batch_->Render(context);
    ui_shader_->Render(control_batch_->index_count(), context);

    // Font pass
    ui_shader_->SetInput("world_matrix", MatrixIdentity(), context);
    ui_shader_->SetInput("proj_matrix", ortho_matrix_, context);
    ui_shader_->SetInput("is_text", true, context);
    for (auto& batch : font_batches_)
    {
        auto font = skin_->font(batch.first.usage);
        ui_shader_->SetInput("diffuse", font->texture(), context);
        ui_shader_->SetInput("text_colour", batch.first.colour, context);

        batch.second->Render(context);
        ui_shader_->Render(batch.second->index_count(), context);
    }
}

void Manager::Update(const Input& input)
{
    temp_window_->Update(input);
}
} // namespace GUI
} // namespace blons