#pragma once
#include <glm/vec4.hpp>
#include <vector>
#include <GL/glew.h>
#include "BoundingSphereLarsson.h"
#include "BoundingEllipsoidPCA.h"
#include "OrientedBoundingBoxPCA.h"

class BoundingSpherePCA;
struct Object;
class AxisAlignedBoundingBox;
class BoundingSphereCentroid;
class BoundingSphereRitter;



class Wireframe
{
public:
  Wireframe(const Wireframe& rhs);
  Wireframe(AxisAlignedBoundingBox& bv);
  Wireframe(Object& obj);
  Wireframe(BoundingSphereCentroid& obj, Object& sphere);
  Wireframe(BoundingSphereRitter& obj, Object& sphere);
  Wireframe(BoundingSpherelarsson& obj, Object& sphere);
  Wireframe(BoundingSpherePCA& obj, Object& sphere);
  Wireframe(BoundingEllipsoidPCA& obj, Object& sphere);
  Wireframe(OrientedBoundingBoxPCA& obj);

  ~Wireframe();

  void BindVertsAndFaces();

  //everything in world coords so just buffer to gpu
  std::vector<glm::vec3> verts;
  std::vector<glm::uvec2> faces;
  glm::vec4 color{ 1, 0, 0, 1};

  GLuint VAO; //entire thing
  GLuint IBO; //faces
  GLuint VBO[1];
};
