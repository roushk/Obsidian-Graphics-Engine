/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: Camera.cpp
Purpose: Camera class
Language: C++ MSVC
Platform: VS 141, OpenGL 4.3 compatabile device driver, Win10
Project: coleman.jonas_CS350_1
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 7/15/18
End Header --------------------------------------------------------*/

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/transform.hpp>
#include "Camera.h"
#include <cmath>



const float PI = 4.0f*atanf(1.0f);


/*    
glm::vec4 eye_point;
glm::vec4 right_vector, up_vector, back_vector;
float width, height, distance, near, far;
*/
/*(default constructor) creates a camera object with center of projection at the
origin, looking in the direction of the negative z{axis, and having up glm::vec4 in the
direction of the y{axis. The viewport should have a eld of view of 90 (PI
2 radians),
aspect ratio of 1, near clipping plane distance of 0.1, and a far clipping plane distance
of 10 in world coordinates.*/
Camera::Camera(void)
{
  eye_point = glm::vec4(0,0,0,0);

  up_vector = glm::vec4(0,1,0,1);
  right_vector = glm::vec4(1,0,0,1);
  back_vector = glm::vec4(0,0,-1,1);
  //90* fov
  //Pi/2 radians
  //aspect ratio = 1
  near = 0.1f;
  far = 10;
  distance = near * 2;
  width = (tanf(PI/4.0f) / distance) * (distance * 2);
  height = width;

  //W of N = (W/D) N
  //same with height
}

/*(non{default constructor) creates a camera with
center of projection E, looking in the direction specied by the glm::vec4 look, and
oriented by the relative up glm::vec4 rel. Note that rel is not necessarily parallel to the
actual up glm::vec4 ~v of the camera that is created. Rather, the plane spanned by rel and
look is orthogonal the the right glm::vec4 ~u of the created camera. The vec3s look and
rel are not necessarily unit vec3s; however, it is assumed that they are not parallel.
The view frustum of the created camera has a horizontal eld of view angle fov given
in radians, the specied aspect ratio (ratio of the width of the viewport to its height),
near clipping plane distance, and far clipping plane distance. All values are given in
world coordinates.*/
Camera::Camera(const glm::vec4& E, const glm::vec4& look, const glm::vec4& rel,
       float fov, float aspect, float N, float F)
{
  /*    
  glm::vec4 eye_point;
  glm::vec4 right_vector, up_vector, back_vector;
  float width, height, distance, near, far;

  N(back glm::vec4) = -I/mag(I)
  U(right glm::vec4) = IxR / mag 
  V(up glm::vec4) = NxU
  */
  eye_point = E;
  
  glm::vec4 newLook = look;

  back_vector = (-1.0f / glm::length(newLook)) * newLook;

  right_vector = ((1.0f / glm::length(cross(look, rel))) * cross(look, rel) );
  back_vector.w = 1.0f;
  right_vector.w = 1.0f;

  up_vector = cross(back_vector, right_vector);

  up_vector.w = 1.0f;


  near = N;
  far = F;
  //decided to place the viewport location as near place * 2 distance 
  distance = abs(N * 2.0f);

  //float nearWidth = /(2 * N);
  //Wn = (W/D)N
  //Wn/n * D = W
  
  //tan(1/x theta) = W/2D
  //aspect = W/H
  
  width = (distance * 2.0f) * tanf(fov/2.0f);
  height = width/aspect;

}

/*moves the camera dist units (in world coordinates) in the direction
􀀀~n (where ~n is the back glm::vec4 of the camera). The instance of the camera class is
returned.*/
Camera& Camera::forward(float distance)
{
  //C <- C - dn
  eye_point = eye_point - (distance * back_vector);
  return *this;
}


/*moves the camera dist units (in world coordinates) in the direction
􀀀~n (where ~n is the back glm::vec4 of the camera). The instance of the camera class is
returned.*/
Camera& Camera::leftRight(float distance)
{
  //C <- C - dn
  eye_point = eye_point - (distance * right_vector);
  return *this;
}

Camera& Camera::upDown(float distance)
{
  //C <- C - dn
  eye_point = eye_point - (distance * up_vector);
  return *this;
}

/*changes the dimensions of the viewport by specied amount. That is,
the width and height of the viewport are multiplied by factor. The instance of the
camera class is returned.*/
Camera& Camera::zoom(float factor)
{
  //zoom W <- BW H <-BH
  width *= factor;
  height *= factor;
  return *this;
}

/*rotates the camera by the specied angle (in radians) about an axis
parallel to the camera up glm::vec4 ~v and passing through the center of projection. The
instance of the camera class is returned.*/
Camera& Camera::yaw(float angle)
{
  //U = right_vector
  //V = up_vector
  //N = back_vector
  //right_vector←R(θup_vector)right_vector 
  right_vector = rotate(angle, glm::vec3(up_vector)) * right_vector;
  //back_vector←R(θup_vector) back_vector
  back_vector = rotate(angle, glm::vec3(up_vector)) *  back_vector;
  return *this;

}

/*rotates the camera by the specied angle (in radians) about an axis parallel
to the camera right glm::vec4 ~u and passing through the center of projection. The instance
of the camera class is returned.
*/
Camera& Camera::pitch(float angle)
{
  
  //up_vector←R(θright_vector)up_vector
  up_vector = rotate(angle, glm::vec3(right_vector)) * up_vector;
  //back_vector←R(θright_vector)back_vector
  back_vector = rotate(angle, glm::vec3(right_vector)) * back_vector;
  return *this;
}

/*rotates the camera by the specied angle (in radians) about an axis parallel to
the camera back glm::vec4 ~n and passing through the center of projection. The instance
of the camera class is returned.
*/
Camera& Camera::roll(float angle)
{
  //right_vector←R(θback_vector)right_vector 
  right_vector = rotate(angle, glm::vec3(back_vector)) * right_vector;
  //up_vector(θback_vector)up_vector
  up_vector = rotate(angle, glm::vec3(back_vector)) * up_vector;
  return *this;
}

//returns the center of projection E of the camera, in world coordinates.
glm::vec4 Camera::eye(void) const
{
  return eye_point;
}

//returns the right glm::vec4 ~u of the camera, in world coordinates. This should be a unit glm::vec4.
glm::vec4 Camera::right(void) const
{
  return right_vector;
}

//returns the up glm::vec4 ~v of the camera, in world coordinates. This should be a unit glm::vec4.
glm::vec4 Camera::up(void) const
{
  return up_vector;
}

/*returns the back glm::vec4 ~n of the camera, in world coordinates. This should be
a unit glm::vec4. The vec3s ~u, ~v, ~n are mutually orthonormal, and in this order, form a
right{handed coordinate system.
*/
glm::vec4 Camera::back(void) const
{
  return back_vector;
}

/*returns the glm::vec4 hW;H;Di, where W is the width of the view-
port, H is the height of the viewport, and D is the distance from the center of projection
to the plane of projection.
*/
glm::vec4 Camera::viewportGeometry(void) const
{
  return glm::vec4(width, height, distance, 1);
}

//not inverted camera matrix
/*returns the distance (in world coordinates) from the center of projection
to the near clipping plane of the viewing frustum.
*/
float Camera::nearDistance(void) const
{
  return near;
}
/*returns the distance (in world coordinates) from the center of projection
to the far clipping plane of the viewing frustum.*/
float Camera::farDistance(void) const
{
  return far;
}