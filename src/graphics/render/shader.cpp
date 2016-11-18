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

namespace blons
{
Shader::Shader(std::string vertex_filename, std::string pixel_filename, ShaderAttributeList inputs) :
    CommonShader::CommonShader()
{
    vertex_filename_ = vertex_filename;
    pixel_filename_ = pixel_filename;
    inputs_ = inputs;
    Reload();
}

Shader::~Shader()
{
}

void Shader::Reload()
{
    std::string vertex_source = ParseFile(vertex_filename_);
    std::string pixel_source = ParseFile(pixel_filename_);

    if (!render::context()->RegisterShader(program_.get(), vertex_source, pixel_source, inputs_))
    {
        log::Fatal("Shaders failed to compile\n");
        throw "Shaders failed to compile";
    }
}

bool Shader::Render(unsigned int index_count)
{
    render::context()->RenderShader(program_.get(), index_count);

    return true;
}
} // namespace blons
