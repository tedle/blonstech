#ifndef BLONSTECH_GRAPHICS_GUI_GUI_H_
#define BLONSTECH_GRAPHICS_GUI_GUI_H_

// Includes
#include <map>
// Local Includes
#include "graphics/render/drawbatcher.h"
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
// Forward declarations
class Label;

enum FontType
{
    DEFAULT,
    HEADING,
    LABEL,
    CONSOLE
};

class Manager
{
public:
    Manager(int screen_width, int screen_height, std::unique_ptr<Shader> ui_shader, RenderContext& context);
    ~Manager();

    bool LoadFont(const char* filename, int pixel_size, RenderContext& context);
    bool LoadFont(const char* filename, FontType usage, int pixel_size, RenderContext& context);

    void Render(RenderContext& context);

private:
    // Since we want this class to be accessed by user, we hide these functions
    // despite widgets needing access to them. Kind of hacky to friend it up, but oh well
    friend Label;
    Font* GetFont(FontType usage);
    DrawBatcher* GetFontBatch(FontType usage, Vector4 colour, RenderContext& context);

    std::map<FontType, std::unique_ptr<Font>> font_list_;

    // One draw batch per font per colour
    std::map<struct FontCall, std::unique_ptr<DrawBatcher>> font_batches_;

    int width_, height_;
    Matrix ortho_matrix_;

    std::unique_ptr<Shader> ui_shader_;

    std::vector<Label> temp_labels_;
};
} // namespace GUI
} // namespace blons
#endif