#include "shader.h"

namespace blons
{
Shader::Shader(HWND hwnd, RenderContext& context)
{
    program_ = std::unique_ptr<ShaderResource>(context->CreateShaderResource());
    ShaderAttributeList inputs;
    inputs.push_back(ShaderAttribute(0, "input_pos"));
    inputs.push_back(ShaderAttribute(1, "input_uv"));
    inputs.push_back(ShaderAttribute(2, "input_norm"));
    WCHAR* vertex_filename = L"test.vert.glsl";
    WCHAR* pixel_filename = L"test.frag.glsl";

    if (!context->RegisterShader(program_.get(), vertex_filename, pixel_filename, inputs))
    {
        g_log->Fatal("Shaders failed to compile\n");
        throw "Shaders failed to compile";
    }
}

Shader::~Shader()
{
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
} // namespace blons
