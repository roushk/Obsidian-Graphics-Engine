#include <pch.h>
#include "Camera.h"

const float PI = 4.0f*atanf(1.0f);

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

Camera& Camera::forward(float distance)
{
  //C <- C - dn
  eye_point = eye_point - (distance * back_vector);
  return *this;
}

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

Camera& Camera::zoom(float factor)
{
  //zoom W <- BW H <-BH
  width *= factor;
  height *= factor;
  return *this;
}

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

Camera& Camera::pitch(float angle)
{
  
  //up_vector←R(θright_vector)up_vector
  up_vector = rotate(angle, glm::vec3(right_vector)) * up_vector;
  //back_vector←R(θright_vector)back_vector
  back_vector = rotate(angle, glm::vec3(right_vector)) * back_vector;
  return *this;
}

Camera& Camera::roll(float angle)
{
  //right_vector←R(θback_vector)right_vector 
  right_vector = rotate(angle, glm::vec3(back_vector)) * right_vector;
  //up_vector(θback_vector)up_vector
  up_vector = rotate(angle, glm::vec3(back_vector)) * up_vector;
  return *this;
}

glm::vec4 Camera::eye(void) const
{
  return eye_point;
}

glm::vec4 Camera::right(void) const
{
  return right_vector;
}

glm::vec4 Camera::up(void) const
{
  return up_vector;
}


glm::vec4 Camera::back(void) const
{
  return back_vector;
}

glm::vec4 Camera::viewportGeometry(void) const
{
  return glm::vec4(width, height, distance, 1);
}


float Camera::nearDistance(void) const
{
  return near;
}


float Camera::farDistance(void) const
{
  return far;
}