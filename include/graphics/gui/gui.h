#ifndef BLONSTECH_GUI_H_
#define BLONSTECH_GUI_H_

// Includes
#include <map>
// Local Includes
#include "graphics/gui/font.h"

namespace blons
{
class GUI
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
    GUI(int width, int height, std::unique_ptr<class Shader>, RenderContext& context);
    ~GUI();

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
}
#endif