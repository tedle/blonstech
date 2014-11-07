#include "graphics.h"

// Local Includes
#include "renderd3d11.h"
#include "rendergl40.h"
#include "camera.h"
#include "math.h"
#include "model.h"
#include "sprite.h"
#include "shader.h"
#include "temphelpers.h"

namespace blons
{
Graphics::Graphics(int screen_width, int screen_height, HWND hwnd)
{
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

    // Shaders
    ShaderAttributeList inputs3d;
    inputs3d.push_back(ShaderAttribute(0, "input_pos"));
    inputs3d.push_back(ShaderAttribute(1, "input_uv"));
    inputs3d.push_back(ShaderAttribute(2, "input_norm"));
    shader3d_ = std::unique_ptr<Shader>(new Shader("3d_test.vert.glsl", "3d_test.frag.glsl", inputs3d, context_));
    if (shader3d_ == nullptr)
    {
        g_log->Fatal("Shaders failed to initialize\n");
        throw "Failed to initialize shader";
    }

    ShaderAttributeList inputs2d;
    inputs2d.push_back(ShaderAttribute(0, "input_pos"));
    inputs2d.push_back(ShaderAttribute(1, "input_uv"));
    shader2d_ = std::unique_ptr<Shader>(new Shader("2d_test.vert.glsl", "2d_test.frag.glsl", inputs2d, context_));
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

    // USE STREAM TO UPDATE POS
    /*std::unique_ptr<Texture> sprite(new Texture("../../notes/me.dds", Texture::Type::DIFFUSE, context_));
    std::unique_ptr<BufferResource> vert_buffer(context_->CreateBufferResource());
    std::unique_ptr<BufferResource> index_buffer(context_->CreateBufferResource());
    context_->RegisterQuad(vert_buffer.get(), index_buffer.get());
    context_->SetModelBuffer(vert_buffer.get(), index_buffer.get());*/
    std::unique_ptr<Sprite> quad(new Sprite("../../notes/me.dds", context_));

    shader2d_->SetInput("world_matrix", MatrixIdentity(), context_);
    shader2d_->SetInput("proj_matrix", context_->ortho_matrix(), context_);
    shader2d_->Render(6, context_);

    // Swap buffers
    context_->EndScene();

    return true;
}
} // namespace blons
