#ifndef BLONSTECH_TEMPHELPERS_H_
#define BLONSTECH_TEMPHELPERS_H_

// Includes
#include <memory>
#include <vector>
// Local Includes
#include "graphics/render/render.h"

namespace blons
{
using namespace DirectX;
class Camera;
class Input;
class Model;

void noclip(Input*, Camera*);
void move_camera_around_origin(float, Camera*);
void FPS();
std::vector<std::unique_ptr<Model>> load_codmap(const char*, std::vector<std::unique_ptr<Model>>, RenderContext& context);
} // namespace blons

#endif