#ifndef BLONSTECH_TEMPHELPERS_H_
#define BLONSTECH_TEMPHELPERS_H_

#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include "camera.h"
#include "model.h"
#include "inputtemp.h"

using namespace DirectX;

void noclip(Input*, Camera*);
void move_camera_around_origin(float, Camera*);
void FPS();
std::vector<std::unique_ptr<Model>> load_codmap(const char*, std::vector<std::unique_ptr<Model>>, RenderContext& context);

#endif