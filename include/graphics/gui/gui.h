#ifndef BLONSTECH_GUI_H_
#define BLONSTECH_GUI_H_

// Includes
#include <map>
// Local Includes
#include "graphics/gui/font.h"

namespace blons
{
class Gui
{
public:
    Gui(RenderContext& context);
    ~Gui();
    bool LoadFont(const char* font_name, int pixel_size);

private:
    std::map<std::string, Font> font_list_;
    // Raw pointer because this should be owned by the parent... so no problem, i think?
    RenderAPI* context_;

};
}
#endif