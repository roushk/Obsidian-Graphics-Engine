#define GLM_ENABLE_EXPERIMENTAL

#include "Wireframe.h"
#include "Object.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#include "AxisAlignedBoundingBox.h"
#include "BoundingSphereCentroid.h"
#include "BoundingSphereLarsson.h"
#include "BoundingSpherePCA.h"
#include "BoundingSphereRitter.h"
#include "BoundingEllipsoidPCA.h"
#include "OrientedBoundingBoxPCA.h"


Wireframe::Wireframe(const Wireframe& rhs)
{
  verts = rhs.verts;
  faces = rhs.faces;
  color = rhs.color;
  BindVertsAndFaces();
}

Wireframe::Wireframe(AxisAlignedBoundingBox& bv)
{
  //assuming AABB is in world space
  glm::vec3 a{ bv.min.x, bv.max.y, bv.min.z };
  glm::vec3 b = bv.min;
  glm::vec3 c{ bv.min.x, bv.min.y, bv.max.z };
  glm::vec3 d{ bv.min.x, bv.max.y, bv.max.z };

  glm::vec3 e = bv.max;
  glm::vec3 f{ bv.max.x, bv.min.y, bv.max.z };
  glm::vec3 g{ bv.max.x, bv.min.y, bv.min.z };
  glm::vec3 h{ bv.max.x, bv.max.y, bv.min.z };

  /*
   * based on cube with lower left corner b and upper right corner e
   */

  verts.reserve(8);
  faces.reserve(12);

  verts.push_back(a); //vert 0
  verts.push_back(b); //vert 1
  verts.push_back(c); //vert 2
  verts.push_back(d); //vert 3
  verts.push_back(e); //vert 4
  verts.push_back(f); //vert 5
  verts.push_back(g); //vert 6
  verts.push_back(h); //vert 7

  //lines and not faces sadly for GL_Lines
  faces.push_back({ 0,1 });
  faces.push_back({ 0,3 });
  faces.push_back({ 0,7 });
  faces.push_back({ 1,2 });
  faces.push_back({ 1,6 });

  faces.push_back({ 4,3 });
  faces.push_back({ 4,7 });
  faces.push_back({ 4,5 });
  faces.push_back({ 5,2 });
  faces.push_back({ 5,6 });

  faces.push_back({ 7,6 });
  faces.push_back({ 2,3 });

  BindVertsAndFaces();
}

//creates wireframe out of object
Wireframe::Wireframe(Object& obj)
{
  for(auto& face: obj.faces)
  {
    //from tri a,b,c to line segments a,b a,c b,c
    faces.push_back({face.x, face.y});
    faces.push_back({face.x, face.z});
    faces.push_back({face.y, face.z});
  }

  for (auto& vert : obj.verts)
  {
    glm::vec3 postVert = obj.modelMatrix * glm::vec4(vert, 1);
    verts.push_back(postVert);
  }


  BindVertsAndFaces();
}

//creates wireframe out of object
Wireframe::Wireframe(BoundingSphereCentroid& obj, Object& sphere)
{
  for (auto& face : sphere.faces)
  {
    //from tri a,b,c to line segments a,b a,c b,c
    faces.push_back({ face.x, face.y });
    faces.push_back({ face.x, face.z });
    faces.push_back({ face.y, face.z });
  }

  for (auto& vert : sphere.verts)
  {
    //scales size 1 to 1 of size from center to max
    float scale = 2.01f * obj.radius;
    // obj.radius * 2;//
    glm::vec3 postVert = 
      (glm::translate(obj.center) * glm::scale(glm::vec3{ scale })) * 
      sphere.modelMatrix * glm::vec4(vert, 1);

    verts.push_back(postVert);
  }


  BindVertsAndFaces();
}

Wireframe::Wireframe(BoundingSphereRitter& obj, Object& sphere)
{
  for (auto& face : sphere.faces)
  {
    //from tri a,b,c to line segments a,b a,c b,c
    faces.push_back({ face.x, face.y });
    faces.push_back({ face.x, face.z });
    faces.push_back({ face.y, face.z });
  }

  for (auto& vert : sphere.verts)
  {
    //scales size 1 to 1 of size from center to max
    float scale = 2.01f * obj.radius;
    //added the 0.05 for little margin of error seems to have fixed it

    glm::vec3 postVert =
      (glm::translate(glm::vec3{ 0,0,0.05f }) * glm::translate(obj.center) * glm::scale(glm::vec3{ scale })) *
      sphere.modelMatrix * glm::vec4(vert, 1);
    postVert += obj.oldOffset;

    verts.push_back(postVert);
  }

  BindVertsAndFaces();
}

Wireframe::Wireframe(BoundingSpherelarsson& obj, Object& sphere)
{
  for (auto& face : sphere.faces)
  {
    //from tri a,b,c to line segments a,b a,c b,c
    faces.push_back({ face.x, face.y });
    faces.push_back({ face.x, face.z });
    faces.push_back({ face.y, face.z });
  }

  for (auto& vert : sphere.verts)
  {
    //scales size 1 to 1 of size from center to max
    float scale = 2.01f * obj.radius;
    //added the 0.05 for little margin of error seems to have fixed it

    glm::vec3 postVert =
      (glm::translate(glm::vec3{ 0,0,0.05f }) * glm::translate(obj.center) * glm::scale(glm::vec3{ scale })) *
      sphere.modelMatrix * glm::vec4(vert, 1);
    postVert += obj.oldOffset;

    verts.push_back(postVert);
  }

  BindVertsAndFaces();
}

Wireframe::Wireframe(BoundingSpherePCA& obj, Object& sphere)
{
  for (auto& face : sphere.faces)
  {
    //from tri a,b,c to line segments a,b a,c b,c
    faces.push_back({ face.x, face.y });
    faces.push_back({ face.x, face.z });
    faces.push_back({ face.y, face.z });
  }

  for (auto& vert : sphere.verts)
  {
    //scales size 1 to 1 of size from center to max
    float scale = 2.01f * obj.radius;
    //added the 0.05 for little margin of error seems to have fixed it

    glm::vec3 postVert =
      (glm::translate(glm::vec3{ 0,0,0.05f }) * glm::translate(obj.center) * glm::scale(glm::vec3{ scale })) *
      sphere.modelMatrix * glm::vec4(vert, 1);
    postVert += obj.oldOffset;

    verts.push_back(postVert);
  }

  BindVertsAndFaces();
}

Wireframe::Wireframe(BoundingEllipsoidPCA& obj, Object& sphere)
{
  for (auto& face : sphere.faces)
  {
    //from tri a,b,c to line segments a,b a,c b,c
    faces.push_back({ face.x, face.y });
    faces.push_back({ face.x, face.z });
    faces.push_back({ face.y, face.z });
  }

  for (auto& vert : sphere.verts)
  {
    //scales size 1 to 1 of size from center to max
    float scale = 2.01f * obj.radius;
    //added the 0.05 for little margin of error seems to have fixed it

    glm::vec3 postVert =
      (glm::translate(glm::vec3{ 0,0,0.05f }) * glm::translate(obj.center) * glm::scale(glm::vec3{ scale })) *
      sphere.modelMatrix * glm::vec4(vert, 1);
    postVert += obj.oldOffset;

    verts.push_back(postVert);
  }

  BindVertsAndFaces();
}

Wireframe::Wireframe(OrientedBoundingBoxPCA& bv)
{
  //assuming AABB is in world space
  glm::vec3 a{ bv.min.x, bv.max.y, bv.min.z };
  glm::vec3 b = bv.min;
  glm::vec3 c{ bv.min.x, bv.min.y, bv.max.z };
  glm::vec3 d{ bv.min.x, bv.max.y, bv.max.z };

  glm::vec3 e = bv.max;
  glm::vec3 f{ bv.max.x, bv.min.y, bv.max.z };
  glm::vec3 g{ bv.max.x, bv.min.y, bv.min.z };
  glm::vec3 h{ bv.max.x, bv.max.y, bv.min.z };

  /*
   * based on cube with lower left corner b and upper right corner e
   */

  verts.reserve(8);
  faces.reserve(12);

  verts.push_back(a); //vert 0
  verts.push_back(b); //vert 1
  verts.push_back(c); //vert 2
  verts.push_back(d); //vert 3
  verts.push_back(e); //vert 4
  verts.push_back(f); //vert 5
  verts.push_back(g); //vert 6
  verts.push_back(h); //vert 7

  //lines and not faces sadly for GL_Lines
  faces.push_back({ 0,1 });
  faces.push_back({ 0,3 });
  faces.push_back({ 0,7 });
  faces.push_back({ 1,2 });
  faces.push_back({ 1,6 });

  faces.push_back({ 4,3 });
  faces.push_back({ 4,7 });
  faces.push_back({ 4,5 });
  faces.push_back({ 5,2 });
  faces.push_back({ 5,6 });

  faces.push_back({ 7,6 });
  faces.push_back({ 2,3 });

  BindVertsAndFaces();
}

void Wireframe::BindVertsAndFaces()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, VBO);
  glGenBuffers(1, &IBO);
  glBindVertexArray(VAO);

  //faces
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof glm::uvec2, faces.data(), GL_STATIC_DRAW);
  //2
  glEnableVertexAttribArray(0);

  //verts
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof glm::vec3, verts.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindVertexArray(0);
}

Wireframe::~Wireframe()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, VBO);
  glDeleteBuffers(1, &IBO);
}
