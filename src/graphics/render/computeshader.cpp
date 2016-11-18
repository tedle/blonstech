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

#include <blons/graphics/render/computeshader.h>

namespace blons
{
ComputeShader::ComputeShader(std::string source_filename)
{
    source_filename_ = source_filename;
    Reload();
}

ComputeShader::~ComputeShader()
{
}

void ComputeShader::Reload()
{
    std::string source = ParseFile(source_filename_);

    if (!render::context()->RegisterComputeShader(program_.get(), source))
    {
        log::Fatal("Shaders failed to compile\n");
        throw "Shaders failed to compile";
    }
}

bool ComputeShader::Run(unsigned int groups_x, unsigned int groups_y, unsigned int groups_z)
{
    render::context()->RunComputeShader(program_.get(), groups_x, groups_y, groups_z);

    return true;
}

bool ComputeShader::SetOutput(const char* field, const TextureResource* value)
{
    return render::context()->SetShaderOutput(program_.get(), field, value, 0);
}

bool ComputeShader::SetOutput(const char* field, const TextureResource* value, unsigned int texture_index)
{
    return render::context()->SetShaderOutput(program_.get(), field, value, texture_index);
}
} // namespace blons
