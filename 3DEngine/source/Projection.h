/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: Projection.h
Purpose: Implementation and header for perspective projection
Language: C++ MSVC
Platform: VS 141, OpenGL 4.3 compatabile device driver, Win10
Project: coleman.jonas_CS350_1
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 7/15/18
End Header --------------------------------------------------------*/

#ifndef CS250_PROJECTION_H
#define CS250_PROJECTION_H

#include "Camera.h"
#include <glm/mat4x4.hpp>


glm::mat4 cameraToWorld(const Camera& cam);
glm::mat4 worldToCamera(const Camera& cam);
glm::mat4 cameraToNDC(const Camera& cam);
glm::mat4 cameraToWorldSkyBox(const Camera& cam);
glm::mat4 worldToCameraSkyBox(const Camera& cam)
;

#endif

