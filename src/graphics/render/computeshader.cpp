////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
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

// Includes
#include <algorithm>

namespace blons
{
ComputeShader::ComputeShader(ShaderSourceList source_files) :
    CommonShader::CommonShader()
{
    source_files_ = source_files;
    Reload();
}

ComputeShader::~ComputeShader()
{
}

void ComputeShader::Reload()
{
    ShaderSourceList parsed_source_list;
    std::transform(source_files_.begin(), source_files_.end(), std::back_inserter(parsed_source_list),
    [&](const auto& source_file)
    {
        std::string source = ParseFile(source_file.second);
        ShaderPipelineStage type = source_file.first;
        return ShaderSource{ type, source };
    });

    program_.reset(render::context()->RegisterComputeShader(parsed_source_list));
    if (program_ == nullptr)
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

bool ComputeShader::SetOutput(const char* field, TextureResource* value)
{
    return render::context()->SetShaderOutput(program_.get(), field, value, 0, 0);
}

bool ComputeShader::SetOutput(const char* field, TextureResource* value, unsigned int texture_index, unsigned int mip_level)
{
    return render::context()->SetShaderOutput(program_.get(), field, value, texture_index, mip_level);
}
} // namespace blons
