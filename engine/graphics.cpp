#include "graphics.h"

// Local Includes
#include "renderd3d11.h"
#include "rendergl40.h"
#include "camera.h"
#include "font.h"
#include "math.h"
#include "model.h"
#include "sprite.h"
#include "shader.h"
#include "temphelpers.h"

namespace blons
{
Graphics::Graphics(int screen_width, int screen_height, HWND hwnd)
{
    font_ = nullptr;
    context_ = nullptr;
    camera_ = nullptr;
    shader3d_ = nullptr;
    shader2d_ = nullptr;

    // DirectX
    //context_ = RenderContext(new RenderD3D11);

    // OpenGL
    context_ = RenderContext(new RenderGL40(screen_width, screen_height, kEnableVsync, hwnd,
                                            (kRenderMode==kRenderModeFullscreen),
                                            kScreenDepth, kScreenNear));
    if (!context_)
    {
        g_log->Fatal("Renderer failed to initailize\n");
        throw "Failed to initialize rendering context";
    }

    // Camera
    camera_ = std::unique_ptr<Camera>(new Camera);
    if (camera_ == nullptr)
    {
        throw "Failed to initialize camera";
    }

    camera_->set_pos(0.0f, 0.0f, -10.0f);

    // Model 1
    models_.push_back(std::unique_ptr<Model>(new Model("../../notes/teapot_highpoly.bms", context_)));
    if (models_[0] == nullptr)
    {
        g_log->Fatal("FATAL: Teapot initialization procedures were unsuccessful\n");
        throw "Failed to initialize model";
    }
    models_[0]->set_pos(0.0, 0.0, 20.0);

    // Model 2
    models_.push_back(std::unique_ptr<Model>(new Model("../../notes/cube.bms", context_)));
    if (models_[1] == nullptr)
    {
        g_log->Fatal("no cube :(\n");
        throw "Failed to initialize model";
    }
    models_[1]->set_pos(10.0, 0.0, 20.0);
    //models_ = load_codmap("../../notes/bms_test", std::move(models_), context_);

    // Fonts
    font_ = std::unique_ptr<Font>(new Font("../../notes/font stuff/test.otf", 32, context_));
    //font_ = std::unique_ptr<Font>(new Font("C:/Windows/Fonts/arial.ttf", 32, context_));

    // Shaders
    ShaderAttributeList inputs3d;
    inputs3d.push_back(ShaderAttribute(0, "input_pos"));
    inputs3d.push_back(ShaderAttribute(1, "input_uv"));
    inputs3d.push_back(ShaderAttribute(2, "input_norm"));
    shader3d_ = std::unique_ptr<Shader>(new Shader("mesh.vert.glsl", "mesh.frag.glsl", inputs3d, context_));

    ShaderAttributeList inputs2d;
    inputs2d.push_back(ShaderAttribute(0, "input_pos"));
    inputs2d.push_back(ShaderAttribute(1, "input_uv"));
    shader2d_ = std::unique_ptr<Shader>(new Shader("sprite.vert.glsl", "sprite.frag.glsl", inputs2d, context_));

    ShaderAttributeList inputs_font;
    inputs_font.push_back(ShaderAttribute(0, "input_pos"));
    inputs_font.push_back(ShaderAttribute(1, "input_uv"));
    shader_font_ = std::unique_ptr<Shader>(new Shader("sprite.vert.glsl", "font.frag.glsl", inputs_font, context_));

    if (shader3d_ == nullptr ||
        shader2d_ == nullptr ||
        shader_font_ == nullptr)
    {
        g_log->Fatal("Shaders failed to initialize\n");
        throw "Failed to initialize shader";
    }
}

Graphics::~Graphics()
{
}

bool Graphics::Frame()
{
    if (!Render())
    {
        return false;
    }

    return true;
}

Camera* Graphics::camera()
{
    return camera_.get();
}

bool Graphics::Render()
{
    Matrix view_matrix, projection_matrix, world_matrix;

    // Clear buffers
    context_->BeginScene();

    // Update camera matrix
    camera_->Render();

    // Get matrices
    view_matrix       = camera_->view_matrix();
    projection_matrix = context_->projection_matrix();

    // Needed so models dont render over themselves
    context_->SetDepthTesting(true);
    for (auto const& model : models_)
    {
        // Prep the pipeline 4 drawering
        model->Render(context_);
        world_matrix = model->world_matrix();

        // Set the inputs
        if (!shader3d_->SetInput("world_matrix", world_matrix, context_) ||
            !shader3d_->SetInput("view_matrix", view_matrix, context_) ||
            !shader3d_->SetInput("proj_matrix", projection_matrix, context_) ||
            !shader3d_->SetInput("diffuse", model->texture(), context_))
        {
            return false;
        }

        // Finally do the render
        if (!shader3d_->Render(model->index_count(), context_))
        {
            return false;
        }
    }

    // Needed so sprites can render over themselves
    context_->SetDepthTesting(false);

    //(sin(GetTickCount64()/500.0f) + 1) * 100

    std::string words = "std::move('run config') // testing setup";
    int x = 100;
    int y = 100;
    shader_font_->SetInput("world_matrix", MatrixIdentity(), context_);
    shader_font_->SetInput("proj_matrix", context_->ortho_matrix(), context_);
    shader_font_->SetInput("diffuse", font_->texture(), context_);
    shader_font_->SetInput("text_colour", Vector3(0.0, 0.0, 0.0), context_);
    for (auto& c : words)
    {
        font_->Render(c, x, y, context_);
        x += font_->advance();

        shader_font_->Render(font_->index_count(), context_);
    }

    // Swap buffers
    context_->EndScene();

    return true;
}
} // namespace blons
