#ifndef BLONSTECH_TEMPHELPERS_H_
#define BLONSTECH_TEMPHELPERS_H_

// Includes
#include <memory>
#include <vector>
// Local Includes
#include "graphics/graphics.h"
#include "graphics/render/render.h"

namespace blons
{
class Camera;
class Input;
class Model;
namespace temp
{
using namespace DirectX;

void noclip(Input*, Camera*);
void move_camera_around_origin(float, Camera*);
void FPS();
std::vector<std::unique_ptr<Model>> load_codmap(std::string, std::vector<std::unique_ptr<Model>>, Graphics* graphics);
} // namespace temp
} // namespace blons

#endif