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

#ifndef BLONSTECH_GRAPHICS_RENDER_SHADERDATA_H_
#define BLONSTECH_GRAPHICS_RENDER_SHADERDATA_H_

// Includes
#include <blons/graphics/render/renderer.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Handles communication of arbitrary objects between CPU and GPU memory
///
/// \tparam T Type of data to be stored on GPU
////////////////////////////////////////////////////////////////////////////////
template <typename T>
class ShaderData
{
    // I think standard layout is what we need for safe GPU copies? Maybe POD is required, not super sure
    static_assert(std::is_standard_layout<T>::value, "ShaderData only compatible with standard layout types");
    static_assert(!std::is_array<T>::value, "ShaderData incompatible with arrays, use pointers instead");

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes an internal ShaderDataResource
    ///
    /// \param val Value to initialize shader memory with
    /// \param length Number of elements to store
    ////////////////////////////////////////////////////////////////////////////////
    ShaderData(const T* val, std::size_t length)
    {
        auto context = render::context();
        data_.reset(context->MakeShaderDataResource());
        length_ = length;
        context->RegisterShaderData(data_.get(), val, length_ * sizeof(T));
    }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves handle to data resource used for GPU bindings
    ///
    /// \return ShaderDataResource handle
    ////////////////////////////////////////////////////////////////////////////////
    ShaderDataResource* data() { return data_.get(); }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves number of `T` elements stored
    ///
    /// \return Length of memory array
    ////////////////////////////////////////////////////////////////////////////////
    std::size_t length() { return length_; }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the value of a range of memory on GPU
    ///
    /// \param val Value to set shader memory to
    /// \param offset Element offset at which to begin data copy
    /// \param length Length of data copy to be made in elements
    ////////////////////////////////////////////////////////////////////////////////
    void set_value(const T* val, std::size_t offset, std::size_t length) { render::context()->SetShaderData(data_.get(), offset * sizeof(T), length * sizeof(T), val); }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls ShaderData::set_value(const T*,std::size_t,std::size_t) with
    /// an offset of 0 and a length of ShaderData::length()
    ////////////////////////////////////////////////////////////////////////////////
    void set_value(const T* val) { set_value(val, 0, length_); }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves an array pointer to the shader data copied from GPU memory
    ///
    /// \return Value of data copied from GPU
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<T[]> value()
    {
        auto val = std::unique_ptr<T[]>(new T[length()]);
        render::context()->GetShaderData(data(), val.get());
        return val;
    }

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Handle to ShaderDataResource used by interal rendering API
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<ShaderDataResource> data_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Number of `T` elements stored
    ////////////////////////////////////////////////////////////////////////////////
    std::size_t length_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::ShaderData
/// \ingroup graphics
///
/// The ShaderData class manages transferring arbitrary data between the CPU and
/// GPU. Stores data as an arbitrary length array provided the base type
/// conforms to standard layout.
///
/// ### Example:
/// \code
/// // Initialize single element ShaderData from a struct
/// struct CameraContainer
/// {
///     blons::Vector3 position;
///     blons::Vector3 view_direction;
/// } camera;
/// camera.position = blons::Vector3(5.0f, 5.0f, 3.0f);
/// camera.view_direction = blons::Vector3(0.0f, 0.0f, 1.0f);
/// blons::ShaderData<CameraContainer> camera_data(&camera, 1);
///
/// // Initialize multi-element ShaderData from a vector
/// std::vector<blons::Vector4> vector_array = { blons::Vector4(5.0f, 5.0f, 5.0f, 3.0f), blons::Vector4(0.0f, 0.0f, 0.0f, 4.0f) };
/// blons::ShaderData<blons::Vector4> array_data(vector_array.data(), vector_array.size());
///
/// // Use ShaderData in a ComputeShader
/// blons::ComputeShader compute("ssbo-test.comp.glsl");
/// compute.SetInput("camera_data", camera_data.data());
/// compute.SetInput("array_data", array_data.data());
/// compute.Run(1, 1, 1);
///
/// // Retrieve new values from GPU memory
/// auto transformed_array = array_data.value();
/// for (int i = 0; i < array_data.length(); i++)
/// {
///     auto v = transformed_array[i];
///     blons::log::Debug("x:%.2f y:%.2f z:%.2f w:%.2f\n", v.x, v.y, v.z, v.w);
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \defgroup gpu GPU Compatible Containers
/// \ingroup graphics
/// \brief Containers formatted for storage in GPU memory
///
/// Compatible as a ShaderData type, can be stored on GPU for easy access
/// from shaders
////////////////////////////////////////////////////////////////////////////////
#endif // BLONSTECH_GRAPHICS_RENDER_SHADERDATA_H_
