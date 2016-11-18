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

#ifndef BLONSTECH_TEMPHELPERS_H_
#define BLONSTECH_TEMPHELPERS_H_

// Includes
#include <memory>
#include <vector>
// Public Includes
#include <blons/graphics/graphics.h>
#include <blons/graphics/render/renderer.h>

namespace blons
{
class Camera;
class Input;
class Model;

////////////////////////////////////////////////////////////////////////////////
/// \brief Cruft that'll be removed sooner or later
////////////////////////////////////////////////////////////////////////////////
namespace temp
{
void noclip(Input*, Camera*); ///< Performs noclip camera movement
void move_camera_around_origin(float, Camera*); ///< Spins camera around the origin point
void FPS(); ///< Logs FPS
std::vector<std::unique_ptr<Model>> load_codmap(std::string, std::vector<std::unique_ptr<Model>>, Graphics* graphics); ///< Loads a bunch of meshes
} // namespace temp
} // namespace blons

#endif // BLONSTECH_TEMPHELPERS_H_