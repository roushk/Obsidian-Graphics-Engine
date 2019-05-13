#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

inline glm::vec4 cross(const glm::vec4& left, const glm::vec4& right)
{
  glm::vec3 leftv3(left);
  glm::vec3 rightv3(right);

  glm::vec3 ans = cross(leftv3, rightv3);

  return glm::vec4(ans,1);
}

class Camera {
  public:
    Camera(void);
    Camera(const glm::vec4& E, const glm::vec4& look, const glm::vec4& rel,
           float fov, float aspect, float N, float F);
    glm::vec4 eye(void) const;  //camera pos
    glm::vec4 right(void) const;
    glm::vec4 up(void) const;
    glm::vec4 back(void) const;
    glm::vec4 viewportGeometry(void) const;

    float nearDistance(void) const;
    float farDistance(void) const;
    Camera& zoom(float factor);
    Camera& forward(float distance);
    Camera& yaw(float angle);
    Camera& pitch(float angle);
    Camera& roll(float angle);
    Camera& leftRight(float distance);
    Camera& upDown(float distance);
  private:
    glm::vec4 eye_point;
    glm::vec4 right_vector, up_vector, back_vector;
    float width, height, distance,
          near, far;
};


#endif

