#include "shader.h"

Shader::Shader()
{
    program_ = nullptr;
}

Shader::~Shader()
{
}

bool Shader::Load(HWND hwnd, RenderContext& context)
{
    program_ = std::unique_ptr<ShaderResource>(new ShaderResource);
    ShaderAttributeList inputs;
    inputs.push_back(ShaderAttribute(0, "input_pos"));
    inputs.push_back(ShaderAttribute(1, "input_uv"));
    inputs.push_back(ShaderAttribute(2, "input_norm"));
    return LoadShader(hwnd, L"test.vert.glsl", L"test.frag.glsl", inputs, context);
}

void Shader::Finish(RenderContext& context)
{
    FinishShader(context);

    return;
}

bool Shader::Render(int index_count, TextureResource* texture,
                    Matrix world_matrix, Matrix view_matrix, Matrix proj_matrix,
                    RenderContext& context)
{
    bool ok = true;
    ok &= context->SetShaderInput(program_.get(), "world_matrix", world_matrix);
    ok &= context->SetShaderInput(program_.get(), "view_matrix", view_matrix);
    ok &= context->SetShaderInput(program_.get(), "proj_matrix", proj_matrix);
    ok &= context->SetShaderInput(program_.get(), "diffuse", texture);
    if (!ok)
    {
        return false;
    }

    context->RenderShader(program_.get(), index_count);

    return true;
}

bool Shader::LoadShader(HWND hwnd, WCHAR* vertex_filename, WCHAR* pixel_filename,
                        ShaderAttributeList inputs, RenderContext& context)
{
    if (!context->RegisterShader(program_.get(), vertex_filename, pixel_filename, inputs))
    {
        MessageBox(hwnd, L"Failed to compile shaders\nSee shader.log", vertex_filename, MB_OK);
        return false;
    }

    return true;
}

void Shader::FinishShader(RenderContext& context)
{
    context->DestroyShaderResource(program_.release());
    return;
}