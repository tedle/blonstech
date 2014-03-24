#include "shader.h"

Shader::Shader()
{
    program_ = nullptr;
}

Shader::~Shader()
{
}

bool Shader::Init(HWND hwnd)
{
    program_ = std::unique_ptr<ShaderResource>(new ShaderResource);
    ShaderAttributeList inputs;
    inputs.push_back(ShaderAttribute(0, "input_pos"));
    inputs.push_back(ShaderAttribute(1, "input_uv"));
    inputs.push_back(ShaderAttribute(2, "input_norm"));
    return InitShader(hwnd, L"test.vert.glsl", L"test.frag.glsl", inputs);
}

void Shader::Finish()
{
    FinishShader();

    return;
}

bool Shader::Render(int index_count, TextureResource* texture,
                    Matrix world_matrix, Matrix view_matrix, Matrix proj_matrix)
{
    bool ok = true;
    ok &= g_render->SetShaderInput(program_.get(), "world_matrix", world_matrix);
    ok &= g_render->SetShaderInput(program_.get(), "view_matrix", view_matrix);
    ok &= g_render->SetShaderInput(program_.get(), "proj_matrix", proj_matrix);
    ok &= g_render->SetShaderInput(program_.get(), "diffuse", texture);
    if (!ok)
    {
        return false;
    }

    g_render->RenderShader(program_.get(), index_count);

    return true;
}

bool Shader::InitShader(HWND hwnd, WCHAR* vertex_filename, WCHAR* pixel_filename,
                        ShaderAttributeList inputs)
{
    if (!g_render->RegisterShader(program_.get(), vertex_filename, pixel_filename, inputs))
    {
        MessageBox(hwnd, L"Failed to compile shaders\nSee shader.log", vertex_filename, MB_OK);
        return false;
    }

    return true;
}

void Shader::FinishShader()
{
    g_render->DestroyShaderResource(program_.release());
    return;
}