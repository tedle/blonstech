////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/graphics/render/shader.h>

// Includes
#include <fstream>
#include <sstream>
#include <regex>

namespace blons
{
Shader::Shader(std::string vertex_filename, std::string pixel_filename, ShaderAttributeList inputs, RenderContext& context)
{
    program_.reset(context->MakeShaderResource());

    std::string vertex_source = ParseFile(vertex_filename);
    std::string pixel_source = ParseFile(pixel_filename);

    if (!context->RegisterShader(program_.get(), vertex_source, pixel_source, inputs))
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

bool Shader::SetInput(const char* field, const float value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

bool Shader::SetInput(const char* field, const int value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

bool Shader::SetInput(const char* field, const Matrix value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

bool Shader::SetInput(const char* field, const Vector2 value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

bool Shader::SetInput(const char* field, const Vector3 value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

bool Shader::SetInput(const char* field, const Vector4 value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value);
}

bool Shader::SetInput(const char* field, const TextureResource* value, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value, 0);
}

bool Shader::SetInput(const char* field, const TextureResource* value, unsigned int texture_index, RenderContext& context)
{
    return context->SetShaderInput(program_.get(), field, value, texture_index);
}

std::string Shader::ParseFile(std::string filename)
{
    // Load source file into memory
    std::ifstream source_file(filename, std::ios::binary);
    if (source_file.fail())
    {
        log::Fatal("Shader file could not be accessed");
        throw "Shader file could not be accessed";
    }
    source_file.imbue(std::locale("C"));
    std::string source((std::istreambuf_iterator<char>(source_file)),
                        std::istreambuf_iterator<char>());
    source_file.close();
    if (source.size() == 0)
    {
        log::Fatal("Shader file was empty");
        throw "Shader file was empty";
    }

    // Search for and process #include directives
    std::regex re ("#include <(.*?)>");
    std::smatch match;
    while (std::regex_search(source, match, re))
    {
        std::string include_file = match[1].str();
        std::string prefix = match.prefix().str();
        std::string suffix = match.suffix().str();
        auto prefix_lines = std::count(prefix.begin(), prefix.end(), '\n');
        std::stringstream processed_source;
        processed_source << prefix << std::endl << "#line 1 \"" << include_file << "\"" << std::endl
                         << ParseFile(include_file) << std::endl << "#line " << prefix_lines + 1 << " \""
                         << filename << "\"" << std::endl << suffix;
        source = processed_source.str();
    }

    return source;
}
} // namespace blons
