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

const float PLANE_THICKNESS_EPSILON = 0.0000000000000000001f;

enum class BSPFLAG : unsigned
{
  Front,
  Back,
  Straddling,
  Coplanar,
  ERROR
};

struct Plane
{
  glm::vec3 n;  //plane normal
  float d;// d = dot(n,p) for p is point on plane
};
Plane createPlane(glm::vec3 normal, glm::vec3 pt);

struct BSPNode
{

  std::vector<BSPNode*> children;

  //only leaf nodes have verts
  std::vector<glm::vec3> verts;

  glm::vec3 color = glm::vec3(0, 0, 0);
  static glm::vec3 colorWireframe;

  void BindVertsAndFaces();
  void CalcMinMaxCenter();

  NodeType type;
  glm::vec3 maxSize = glm::vec3(0);
  glm::vec3 minSize = glm::vec3(0);
  GLuint VAO; //entire thing
  GLuint VBO[1];




};

//15 different splitting planes, define d at centeroid
//using Larssons normals for plane generation
static std::vector < glm::vec3> SplittingPlanes
{
  { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 },

  { 1, 1, 1 }, { 1, 1, -1 }, { 1, -1, 1 }, { 1, -1, -1 },

  { 1, 1, 0 }, { 1, -1, 0 }, { 1, 0, 1 }, { 1, 0, -1 }, { 0, 1, 1 }, { 0, 1, -1 },

  { 1, 2, 0 }, { 1, -2, 0 }, { 1, 0, 2 }, { 1, 0, -2 }, { 0, 1, 2 }, { 0, 1, -2 }, { 2, 1, 0 }, { 2, -1, 0 }, { 2, 0, 1 }, { 2, 0, -1 }, { 0, 2, 1 }, { 0, 2, -1 },

  { 1, 1, 2 }, { 1, 2, 1 }, { 2, 1, 1 }, { 1, -1, 2 }, { 1, -2, 1 }, { 2, -1, 1 }, { 1, 1, -2 }, { 1, 2, -1 }, { 2, 1, -1 }, { 1, -1, -2 }, { 1, -2, -1 }, { 2, -1, -1 },

  { 1, 2, 2 }, { 2, 1, 2 }, { 2, 2, 1 }, { 1, -2, 2 }, { 2, -1, 2 }, { 2, -2, 1 }, { 1, 2, -2 }, { 2, 1, -2 }, { 2, 2, -1 }, { 1, -2, -2 }, { 2, -1, -2 }, { 2, -2, -1 }
  /*
   *
  {1,  0,  0},
  {0,  1,  0},
  {0,  0,  1},
  {1,  1,  0},
  {1,  0,  1},
  {0,  1,  1},
  {1,  1,  1},

  {2, -3, -1},
  {2, -2, -1},
  {2, -1, -1},
  {2,  0, -1},
  {2,  1, -1},
  {2,  2, -1},
  {2,  3, -1},
  {2,  4, -1},

  {2,  1,  1},
  {3,  1,  1},
  {4,  1,  1},

  {1,  2,  1},
  {1,  3,  1},
  {1,  4,  1},

  {1,  1,  2},
  {1,  1,  3},
  {1,  1,  4},

  {-3,  1,  1},
  {-2,  1,  1},
  {-1,  1,  1},
  { 0,  1,  1},
  { 1,  1,  1},
  { 2,  1,  1},
  { 3,  1,  1},

  { 1, -3,  1},
  { 1, -2,  1},
  { 1, -1,  1},
  { 1,  0,  1},
  { 1,  1,  1},
  { 1,  2,  1},
  { 1,  3,  1},

  { 1,  1, -3},
  { 1,  1, -2},
  { 1,  1, -1},
  { 1,  1,  0},
  { 1,  1,  1},
  { 1,  1,  2},
  { 1,  1,  3},

  {-3,  -1,  -1},
  {-2,  -1,  -1},
  {-1,  -1,  -1},
  { 0,  -1,  -1},
  { 1,  -1,  -1},
  { 2,  -1,  -1},
  { 3,  -1,  -1},
            
  { -1, -3,  -1},
  { -1, -2,  -1},
  { -1, -1,  -1},
  { -1,  0,  -1},
  { -1,  1,  -1},
  { -1,  2,  -1},
  { -1,  3,  -1},
    
  { -1,  -1, -3},
  { -1,  -1, -2},
  { -1,  -1, -1},
  { -1,  -1,  0},
  { -1,  -1,  1},
  { -1,  -1,  2},
  { -1,  -1,  3},
   */
};

//intersection point between edge BA and Plane plan
bool IntersectEdgeAgainstPlane(glm::vec3 a, glm::vec3 b, Plane plane, glm::vec3& midPoint);

void SplitPolygon(std::vector<glm::vec3>& points, Plane plane, std::vector<glm::vec3>& frontVerts,
  std::vector<glm::vec3>& backVerts);

Plane GetPlaneFromPolygon(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
Plane ChooseSplitPlane(std::vector<glm::vec3>& pointCloud);
BSPFLAG ClassifyPolygonToPlane(Plane& plane, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3);
BSPFLAG ClassifyPointToPlane(Plane& plane, glm::vec3& p);

void clearBSPTreeNode(BSPNode* node);
BSPNode* BuildBSPTree(std::vector<glm::vec3> pointCloud, int maxDepth);
BSPNode* BuildBSPTree_Start(std::vector<glm::vec3> pointCloud, int maxDepth);
