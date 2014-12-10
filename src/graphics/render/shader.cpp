#include "graphics/render/shader.h"

namespace blons
{
Shader::Shader(std::string vertex_filename, std::string pixel_filename, ShaderAttributeList inputs, RenderContext& context)
{
    program_ = std::unique_ptr<ShaderResource>(context->MakeShaderResource());

    if (!context->RegisterShader(program_.get(), vertex_filename, pixel_filename, inputs))
    {
        log::Fatal("Shaders failed to compile\n");
        throw "Shaders failed to compile";
    }
}

Shader::~Shader()
{
}

bool Shader::Render(unsigned int index_count, RenderContext& context)
{
    context->RenderShader(program_.get(), index_count);

    return true;
}

bool Shader::SetInput(const char* field, int value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
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

bool Shader::SetInput(const char* field, const TextureResource* value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

ShaderResource* Shader::program() const
{
    return program_.get();
}
} // namespace blons
