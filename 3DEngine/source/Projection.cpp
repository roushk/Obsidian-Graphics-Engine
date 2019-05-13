/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: Projection.cpp
Purpose: Implementation and header for perspective projection
Language: C++ MSVC
Platform: VS 141, OpenGL 4.3 compatabile device driver, Win10
Project: coleman.jonas_CS350_1
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 7/15/18
End Header --------------------------------------------------------*/

#include "Projection.h"
#include <glm/gtc/matrix_transform.inl>

/*returns the affine transformation that maps camera space coor-
dinates to world space coordinates. Note that the dimensions of the viewport are the
same in both coordinate systems.*/
glm::mat4 cameraToWorld(const Camera& cam)
{
  glm::mat4 Mc(1.0f);
  glm::vec4 U = cam.right();
  glm::vec4 V = cam.up();
  glm::vec4 N = cam.back();
  glm::vec4 E = cam.eye();
  
  Mc[0] = glm::vec4(glm::vec3(U), 0);
  Mc[1] = glm::vec4(glm::vec3(V), 0);
  Mc[2] = glm::vec4(glm::vec3(N), 0);
  Mc[3] = glm::vec4(glm::vec3(E), 1);
  return Mc;
}

/*returns the affine transformation that maps world space coordi-
nates to camera space coordinates. Note that the dimensions of the viewport are the
same in both coordinate systems. This transformation moves cam (via rotation and
translation) to the canonical camera, whose center of projection is at the origin and
with right, up, and back vectors aligned along the x, y, and z axes (respectively). In
particular, the look{at glm::vec4 of the canonical camera points along the negative z{axis.*/
glm::mat4 worldToCamera(const Camera& cam)
{
  return inverse(cameraToWorld(cam));
}

/*returns the matrix that represents the transformation  from camera
coordinates to normalized device coordinates. Here the normalized device coordinate
system is such that the view frustum is mapped to the standard cube (􀀀1  x  1,
􀀀1  y  1, 􀀀1  z  1) and such that the near face maps to the standard cube face
at z = 􀀀1, and the far face maps to the standard cube face at z = 1. Recall that this
matrix can be taken to be

 (view handout for matrix)

where W;H give the width and height of the camera viewport, D is the distance from
the center of projection (the origin in camera coordinates) to the viewport, and N; F
give the distance to the near and far planes. A perspective divide is needed after
applying this matrix to a glm::vec4.*/
glm::mat4 cameraToNDC(const Camera& cam)
{
  glm::mat4 M(1.0f);
  //W,H,D
  glm::vec4 v = cam.viewportGeometry();
  float N = cam.nearDistance();
  float F = cam.farDistance();
  M[0] = glm::vec4((2.0f * v.z)/v.x, 0,0,0);
  M[1] = glm::vec4(0,(2.0f * v.z)/v.y, 0,0);
  M[2] = glm::vec4(0,0,
     (N + F)
     / (N - F) , 
     (N * 2 * F) / 
      (N - F));
  M[3] = glm::vec4(0,0,-1,0); //yes [3][3] IS supposed to be zero

  return transpose(M);  //glm is column not row 

}



glm::mat4 cameraToWorldSkyBox(const Camera& cam)
{
  glm::mat4 Mc(1.0f);
  glm::vec4 U = cam.right();
  glm::vec4 V = cam.up();
  glm::vec4 N = cam.back();
  glm::vec4 E = cam.eye();


  Mc[3] = glm::vec4(glm::vec3(E), 1);
  return Mc;
}

/*returns the affine transformation that maps world space coordi-
nates to camera space coordinates. Note that the dimensions of the viewport are the
same in both coordinate systems. This transformation moves cam (via rotation and
translation) to the canonical camera, whose center of projection is at the origin and
with right, up, and back vectors aligned along the x, y, and z axes (respectively). In
particular, the look{at glm::vec4 of the canonical camera points along the negative z{axis.*/
glm::mat4 worldToCameraSkyBox(const Camera& cam)
{
  return inverse(cameraToWorldSkyBox(cam));
}

