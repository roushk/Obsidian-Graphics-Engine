#pragma once
#include <vector>
#include <glm/vec4.hpp>
#include <GL/glew.h>


struct Object;

class Wireframe
{
public:
  Wireframe(const Wireframe& rhs);
  Wireframe(Object& obj);


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
