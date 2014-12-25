#ifndef BLONSTECH_TEMPHELPERS_H_
#define BLONSTECH_TEMPHELPERS_H_

// Includes
#include <memory>
#include <vector>
// Public Includes
#include <blons/graphics/graphics.h>
#include <blons/graphics/render/render.h>

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