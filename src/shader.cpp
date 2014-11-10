#include "render/shader.h"

namespace blons
{
Shader::Shader(const char* vertex_filename, const char* pixel_filename, ShaderAttributeList inputs, RenderContext& context)
{
    program_ = std::unique_ptr<ShaderResource>(context->CreateShaderResource());

    if (!context->RegisterShader(program_.get(), vertex_filename, pixel_filename, inputs))
    {
        g_log->Fatal("Shaders failed to compile\n");
        throw "Shaders failed to compile";
    }
}

Shader::~Shader()
{
}

bool Shader::Render(int index_count, RenderContext& context)
{
    context->RenderShader(program_.get(), index_count);

    return true;
}

bool Shader::SetInput(const char* field, Matrix value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

bool Shader::SetInput(const char* field, Vector3 value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

bool Shader::SetInput(const char* field, Vector4 value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

bool Shader::SetInput(const char* field, TextureResource* value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

ShaderResource* Shader::program()
{
    return program_.get();
}
} // namespace blons
