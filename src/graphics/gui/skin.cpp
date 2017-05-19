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

#include <blons/graphics/gui/skin.h>

// Public Includes
#include <blons/graphics/sprite.h>

namespace blons
{
namespace gui
{
Skin::Skin()
{
    // TODO: Load from user supplied image
    skin_.reset(new Texture("skin.png", { TextureType::RAW, TextureType::LINEAR, TextureType::CLAMP }));

    font_list_.resize(kMaxFonts);
    // TODO: Ensure DEFAULT font is somehow not nullptr before being handed over to user
    for (auto& f : font_list_)
    {
        f = nullptr;
    }
}

bool Skin::LoadFont(std::string filename, units::pixel pixel_size, FontStyle style)
{
    font_list_[style].reset(new Font(filename, pixel_size));
    return true;
}

Font* Skin::font(FontStyle style)
{
    auto& font = font_list_[style];
    if (font != nullptr)
    {
        return font.get();
    }
    return font_list_[DEFAULT].get();
}

const Texture* Skin::texture() const
{
    return skin_.get();
}

const Skin::Layout* Skin::layout() const
{
    return &layout_;
}
} // namespace gui
} // namespace blons