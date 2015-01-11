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

#include <blons/graphics/light.h>

// Includes
#include <algorithm>
// Public Includes
#include <blons/graphics/camera.h>

namespace blons
{
Light::Light(Type type, Vector3 pos, Vector3 dir, Vector3 colour)
{
    view_.reset(new Camera);
    type_ = type;
    set_colour(colour);
    set_direction(dir);
    if (type_ != DIRECTIONAL)
    {
        set_pos(pos);
    }
    else
    {
        pos_ = Vector3(0, 0, 0);
    }
}

Matrix Light::ViewFrustum(Matrix frustum, units::world depth) const
{
    Matrix light_view_matrix = view_matrix();
    Matrix inv_frustum = MatrixInverse(frustum);
    // Make a box that we will transform to be shaped like the camera's frustum
    // Z ranges from [0,1] because -1 would be behind the camera. This represents
    // the screen near/far distances and would be where to apply split points
    Vector3 ndc_box[8];
    Vector3 min, max;
    for (int x = 0; x < 2; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            for (int z = 0; z < 2; z++)
            {
                auto i = x * 4 + y * 2 + z;
                // Generate a unique vertex of the clip box
                ndc_box[i] = Vector3(x % 2 ? -1.0f : 1.0f,
                                     y % 2 ? -1.0f : 1.0f,
                                     z % 2 ?  0.0f : 1.0f);
                // Shape the box like the camera frustum
                ndc_box[i] = inv_frustum * ndc_box[i];
                // Align the box to the light's view space
                ndc_box[i] = light_view_matrix * ndc_box[i];
                if (i == 0)
                {
                    min.x = max.x = ndc_box[i].x;
                    min.y = max.y = ndc_box[i].y;
                    min.z = max.z = ndc_box[i].z;
                }
                // Form a bounding box aligned to the light around the camera's frustum
                min.x = std::min(ndc_box[i].x, min.x);
                min.y = std::min(ndc_box[i].y, min.y);
                min.z = std::min(ndc_box[i].z, min.z);
                max.x = std::max(ndc_box[i].x, max.x);
                max.y = std::max(ndc_box[i].y, max.y);
                max.z = std::max(ndc_box[i].z, max.z);
            }
        }
    }
    // Modify the clip range to max out at the camera view distance and bottom out
    // at a negative kScreenFar away from the player, allowing distant objects
    // to cast shadows from off screen
    //
    // We multiply max.z's depth by 1.3f to account for some floating point
    // imprecision behind the player when facing a light
    max.z =  depth * 1.3f - max.z;
    min.z = -depth - min.z;
    // Make a projection matrix that perfectly views the camera's frustum
    Matrix light_frustum = MatrixOrthographic(min.x, max.x, min.y, max.y, min.z, max.z);

    return light_view_matrix * light_frustum;
}

const Vector3& Light::colour() const
{
    return colour_;
}

const Vector3& Light::direction() const
{
    return direction_;
}

const Vector3& Light::pos() const
{
    return pos_;
}

Matrix Light::view_matrix() const
{
    // The camera's view matrix should work fine, but this is calculated faster
    // And has less margin for rounding errors
    if (type_ == DIRECTIONAL)
    {
        return MatrixLookAt(Vector3(0, 0, 0), direction_, Vector3(0, 1, 0));
    }
    else
    {
        return view_->view_matrix();
    }
}

void Light::set_colour(const Vector3& colour)
{
    colour_ = colour;
}

void Light::set_direction(const Vector3& dir)
{
    direction_ = VectorNormalize(dir);
    // Set the camera's direction too
    view_->set_pos(0, 0, 0);
    view_->LookAt(dir.x, dir.y, dir.z);
    view_->set_pos(pos_.x, pos_.y, pos_.z);
}

void Light::set_pos(const Vector3& pos)
{
    if (type_ != DIRECTIONAL)
    {
        pos_ = pos;
        // Set the camera's position too
        view_->set_pos(pos.x, pos.y, pos.z);
    }
}
} // namespace blons