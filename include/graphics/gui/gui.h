#ifndef BLONSTECH_GRAPHICS_GUI_GUI_H_
#define BLONSTECH_GRAPHICS_GUI_GUI_H_

// Includes
#include <map>
// Local Includes
#include "graphics/gui/font.h"
#include "graphics/gui/label.h"
#include "graphics/gui/button.h"
#include "graphics/gui/window.h"

namespace blons
{
// Forward declarations
class Shader;

namespace GUI
{
class Manager
{
public:
    enum FontType
    {
        DEFAULT,
        HEADING,
        LABEL,
        CONSOLE
    };
public:
    Manager(int screen_width, int screen_height, std::unique_ptr<Shader>, RenderContext& context);
    ~Manager();

    bool LoadFont(const char* filename, int pixel_size, RenderContext& context);
    bool LoadFont(const char* filename, FontType usage, int pixel_size, RenderContext& context);

    void Render(RenderContext&);

private:
    struct
    {
        std::unique_ptr<Font> fallback, heading, label, console;
    } font_list_;

    int width_, height_;
    Matrix ortho_matrix_;

    std::unique_ptr<Shader> ui_shader_;
};
} // namespace GUI
} // namespace blons
#endif