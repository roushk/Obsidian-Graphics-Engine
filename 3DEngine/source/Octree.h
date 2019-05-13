/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: README.txt
Purpose: README file
Project: coleman.jonas_CS350_3
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 4/1/19
End Header --------------------------------------------------------*/

#pragma once
#include "Wireframe.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include "NodeTypes.h"


struct Object;
typedef glm::vec3 Point;

void createPointCloudFromObj(const Object & obj, std::vector<glm::vec3>& pointCloud, const glm::vec3& transform);

//TODO: create wireframe from center+=halfwidths for Octree node use that to display
//as the edges of the Octree


struct OctreeNode
{
  NodeType type;
  Point center = glm::vec3(0, 0, 0);
  float halfWidth = 0;
  
  std::vector<OctreeNode*> children;

  //only leaf nodes have verts
  std::vector<glm::vec3> verts;

  glm::vec3 color = glm::vec3(0,0,0);
  static glm::vec3 colorWireframe;

  void createPointsWireframe();
  void BindVertsAndFaces();
  void BindWireframeVertsAndFaces();

  GLuint VAO; //entire thing
  GLuint VBO[1];

  std::vector<glm::vec3> wireframe_verts;
  GLuint wireframe_VAO; //entire thing
  GLuint wireframe_VBO[1];
};

void clearOctreeNode(OctreeNode* node);
OctreeNode* BuildOctree(Point center, float halfWidth, std::vector<glm::vec3> pointCloud);
OctreeNode* BuildOctree_Start(std::vector<glm::vec3> pointCloud);

