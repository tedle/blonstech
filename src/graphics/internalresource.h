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

#ifndef BLONSTECH_GRAPHICS_INTERNALRESOURCE_H_
#define BLONSTECH_GRAPHICS_INTERNALRESOURCE_H_

// Public Includes
#include <blons/graphics/render/renderer.h>
#include <blons/graphics/mesh.h>

namespace blons
{
namespace resource
{
namespace internal
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Generates an internal engine mesh of the specified name. If an
/// invalid name is supplied this function will throw.
///
/// \param name Name of the mesh to generate
/// \return MeshData of the generated mesh
////////////////////////////////////////////////////////////////////////////////
MeshData MakeEngineMesh(const std::string& name);
////////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the supplied mesh name is valid
///
/// \param name Name of the mesh to check
/// \return True if mesh can be generated
////////////////////////////////////////////////////////////////////////////////
bool ValidEngineMesh(const std::string& name);
////////////////////////////////////////////////////////////////////////////////
/// \brief Generates an internal engine texture of the specified name. If an
/// invalid name is supplied this function will throw.
///
/// \param name Name of the texture to generate
/// \return PixelData of the generated texture
////////////////////////////////////////////////////////////////////////////////
PixelData MakeEngineTexture(const std::string& name);
////////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the supplied texture name is valid
///
/// \param name Name of the texture to check
/// \return True if texture can be generated
////////////////////////////////////////////////////////////////////////////////
bool ValidEngineTexture(const std::string& name);
} // namespace internal
} // namespace resource
} // namespace blons

#endif // BLONSTECH_GRAPHICS_INTERNALRESOURCE_H_