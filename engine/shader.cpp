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
    return InitShader(hwnd, L"test.vert.glsl", L"test.frag.glsl");
}

void Shader::Finish()
{
    FinishShader();

    return;
}

bool Shader::Render(int index_count, TextureResource* texture,
                    Matrix world_matrix, Matrix view_matrix, Matrix proj_matrix)
{
    if (!g_render->SetShaderInputs(program_.get(), texture, world_matrix, view_matrix, proj_matrix))
    {
        return false;
    }

    g_render->RenderShader(program_.get(), index_count);

    return true;
}

bool Shader::InitShader(HWND hwnd, WCHAR* vertex_filename, WCHAR* pixel_filename)
{
    if (!g_render->RegisterShader(program_.get(), vertex_filename, pixel_filename))
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