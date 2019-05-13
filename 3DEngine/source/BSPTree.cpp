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
#include <iostream>
#include "BSPTree.h"
#include "Random.h"
#include "Object.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/normalize_dot.hpp>
#include <chrono>


Plane createPlane(glm::vec3 normal, glm::vec3 p1)
{
  Plane plane;
  plane.n = normal;
  //plane defined by Ax + By + Cz + D = 0;
  //rewriting as D = -(Ax + By + Cz) where the point x,y,z is p1

  plane.d = -(plane.n.x * p1.x + plane.n.y * p1.y + plane.n.z * p1.z);
  return plane;
}

void BSPNode::BindVertsAndFaces()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, VBO);
  glBindVertexArray(VAO);

  //2
  glEnableVertexAttribArray(0);

  //verts
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof glm::vec3, verts.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindVertexArray(0);
}


void BSPNode::CalcMinMaxCenter()
{
  //base case
  if (verts.size() > 0)
  {
    for (auto& vert : verts)
    {
      if (vert.x > maxSize.x)
        maxSize.x = vert.x;
      if (vert.y > maxSize.y)
        maxSize.y = vert.y;
      if (vert.z > maxSize.z)
        maxSize.z = vert.z;

      if (vert.x < minSize.x)
        minSize.x = vert.x;
      if (vert.y < minSize.y)
        minSize.y = vert.y;
      if (vert.z < minSize.z)
        minSize.z = vert.z;
    }
  }
  //get children stuff first
  //get their min/max size
  for(auto& child: children)
  {
    child->CalcMinMaxCenter();
  }

  //set this nodes min/max size
  for (auto& child : children)
  {

    if (child->maxSize.x > maxSize.x)
      maxSize.x = child->maxSize.x;
    if (child->maxSize.y > maxSize.y)
      maxSize.y = child->maxSize.y;
    if (child->maxSize.z > maxSize.z)
      maxSize.z = child->maxSize.z;

    if (child->minSize.x < minSize.x)
      minSize.x = child->minSize.x;
    if (child->minSize.y < minSize.y)
      minSize.y = child->minSize.y;
    if (child->minSize.z < minSize.z)
      minSize.z = child->minSize.z;
  }
}


void clearBSPTreeNode(BSPNode* node)
{
  //clear children
  for (auto*subNode : node->children)
  {
    clearBSPTreeNode(subNode);
  }

  //clear opengl stuff
  glDeleteBuffers(1, node->VBO);
  glDeleteVertexArrays(1, &node->VAO);


  //clear self
  delete(node);
}

void SplitPolygon(std::vector<glm::vec3>& points, Plane plane, std::vector<glm::vec3>& frontPoints,
  std::vector<glm::vec3>& backPoints)
{
  int numInFront = 0, numInBack = 0;

  glm::vec3 a = points.back();
  BSPFLAG aSide = ClassifyPointToPlane(plane, a);
  std::vector<glm::vec3> frontVerts;
  std::vector<glm::vec3> backVerts;

  frontVerts.reserve(3);
  backVerts.reserve(3);
  for(int n = 0; n < points.size(); ++n)
  {
    glm::vec3 b = points[n];
    BSPFLAG bSide = ClassifyPointToPlane(plane, b);
    if(bSide == BSPFLAG::Front)
    {
      if(aSide == BSPFLAG::Back)
      {
        //suggested (b,a,plane)
        glm::vec3 i{0,0,0};
        IntersectEdgeAgainstPlane(b, a, plane, i);

        frontVerts.push_back(i);
        backVerts.push_back(i);

      }
      frontVerts.push_back(b);

    }
    else if (bSide == BSPFLAG::Back)
    {
      if (aSide == BSPFLAG::Front)
      {
        glm::vec3 i{ 0,0,0 };
        IntersectEdgeAgainstPlane(b, a, plane, i);

        frontVerts.push_back(i);
        backVerts.push_back(i);
      }
      if (aSide == BSPFLAG::Straddling)
      {
        backVerts.push_back(a);
      }
      backVerts.push_back(b);
    }
    else
    {
      frontVerts.push_back(b);
      if(aSide == BSPFLAG::Back)
      {
        backVerts.push_back(b);
      }
    }
    a = b;
    aSide = bSide;
  }
  //if already 3 then we are good
  //std::vector<glm::vec3> frontPts;
  //std::vector<glm::vec3> backPts;

  
  //assuming convex polygon pushing back to make polygon
  for(unsigned i = 0 ; i + 2 < frontVerts.size(); ++i)
  {
    frontPoints.push_back(frontVerts[i]);
    frontPoints.push_back(frontVerts[i + 1]);
    frontPoints.push_back(frontVerts.back());
  }

  for (unsigned i = 0; i + 2 < backVerts.size(); ++i)
  {
    backPoints.push_back(backVerts[i]);
    backPoints.push_back(backVerts[i + 1]);
    backPoints.push_back(backVerts.back());
  }

}

 bool IntersectEdgeAgainstPlane(glm::vec3 a, glm::vec3 b, Plane plane, glm::vec3& midPoint)
{
  glm::vec3 ab = b - a;
  float t = (plane.d - glm::dot(plane.n, a)) / glm::dot(plane.n, ab);
  //if t within points
  if(t >= 0.0f && t <= 1.0f)
  {
    midPoint = a + t * ab;
    return true;
  }
  return false;
}

Plane GetPlaneFromPolygon(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
  Plane plane;
  glm::vec3 p12 = p2 - p1;
  glm::vec3 p13 = p3 - p1;
  plane.n = glm::cross(p12, p13);
  //plane defined by Ax + By + Cz + D = 0;
  //rewriting as D = -(Ax + By + Cz) where the point x,y,z is p1

  plane.d = -(plane.n.x * p1.x + plane.n.y * p1.y + plane.n.z * p1.z);
  return plane;
}



BSPFLAG ClassifyPointToPlane(Plane& plane, glm::vec3& p)
{
  float dist = glm::dot(plane.n, p) - plane.d;
  if (dist > PLANE_THICKNESS_EPSILON)
  {
    return BSPFLAG::Front;
  }
  if (dist < -PLANE_THICKNESS_EPSILON)
  {
    return BSPFLAG::Back;
  }
  return BSPFLAG::Straddling;
}


BSPFLAG ClassifyPolygonToPlane(Plane& plane, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3)
{

  int numInFront = 0, numBehind = 0;
  int numVerts = 3;

  auto flag1 = ClassifyPointToPlane(plane, p1);
  auto flag2 = ClassifyPointToPlane(plane, p2);
  auto flag3 = ClassifyPointToPlane(plane, p3);

  if (flag1 == BSPFLAG::Front)
    numInFront++;
  if (flag2 == BSPFLAG::Front)
    numInFront++;
  if (flag3 == BSPFLAG::Front)
    numInFront++;

  if (flag1 == BSPFLAG::Back)
    numBehind++;
  if (flag2 == BSPFLAG::Back)
    numBehind++;
  if (flag3 == BSPFLAG::Back)
    numBehind++;
  
  if(numBehind != 0 && numInFront != 0)
    return BSPFLAG::Straddling;
  
  if (numInFront != 0)
    return BSPFLAG::Front;

  if (numBehind != 0)
    return BSPFLAG::Back;

  return BSPFLAG::Coplanar;
}

Plane ChooseSplitPlane(std::vector<glm::vec3>& pointCloud)
{
  const float K = 0.8f;
  //const float goodEnough = 0.062f;
  //float depthVal = float(pointCloud.size()) * goodEnough;

  Plane bestPlane;
  float bestScore = std::numeric_limits<float>::max();


  std::vector<Plane> planes;
  planes.reserve(SplittingPlanes.size());

  //calculate weighted centeroid of points to get approx splitting plane
  glm::vec3 centeroid{ 0,0,0 };
  for (unsigned i = 0; i < pointCloud.size(); i++)
  {
    centeroid += pointCloud[i];
  }
  centeroid /= pointCloud.size();

  //create canidate planes
  for (auto& subPlane : SplittingPlanes)
  {
    planes.push_back(createPlane(subPlane, -centeroid));
  }

  //check through canidate planes
  for (auto& plane: planes)
  {
    int numInFront = 0, numBehind = 0, numStraddling = 0;

    for (unsigned j = 0; pointCloud.size() > 2 && j + 2 < pointCloud.size() + 2; j += 3)
    {

      BSPFLAG flag = ClassifyPolygonToPlane(plane, pointCloud[j], pointCloud[j + 1], pointCloud[j + 2]);
      
      //treat coplanar as front
      if (flag == BSPFLAG::Front || flag == BSPFLAG::Coplanar)
      {
        numInFront++;

      }
      else if (flag == BSPFLAG::Back)
      {
        numBehind++;

      }
      else if (flag == BSPFLAG::Straddling)
      {
        numStraddling++;
      }
    }

    float score = K * numStraddling + (1.0f - K) * std::abs(numInFront - numBehind);
    if (score < bestScore)
    {
      bestScore = score;
      bestPlane = plane;
    }

  }
  return bestPlane;
}


BSPNode* BuildBSPTree(std::vector<glm::vec3> pointCloud, int maxDepth)
{
  BSPNode* pNode = new BSPNode;

  //termination
  if ((pointCloud.size() <= 900) || (maxDepth <= 0))
  {
    //create node
    pNode->verts = pointCloud;

    pNode->BindVertsAndFaces();
    pNode->color.x = RandomValue(0, 1000) / 1000.0f;
    pNode->color.y = RandomValue(0, 1000) / 1000.0f;
    pNode->color.z = RandomValue(0, 1000) / 1000.0f;

    if (pNode->verts.size() > 0)
      pNode->type = NodeType::Leaf;
    else
      pNode->type = NodeType::Non_Terminal;
    return pNode;
  }

  pNode->type = NodeType::Non_Terminal;
  //calculate best split plane
  Plane plane = ChooseSplitPlane(pointCloud);
  std::vector<glm::vec3> FrontList;
  std::vector<glm::vec3> BackList;
  FrontList.reserve(pointCloud.size());
  BackList.reserve(pointCloud.size());


  for (unsigned j = 0; pointCloud.size() > 2 && j + 2 < pointCloud.size() + 2; j += 3)
  {
    //vert 1 = pointCloud[j]
    //vert 2 = pointCloud[j + 1]
    //vert 3 = pointCloud[j + 2]
    BSPFLAG flag = ClassifyPolygonToPlane(plane, pointCloud[j], pointCloud[j + 1], pointCloud[j + 2]);
    if(flag == BSPFLAG::Front)
    {
      FrontList.push_back(pointCloud[j]);
      FrontList.push_back(pointCloud[j + 1]);
      FrontList.push_back(pointCloud[j + 2]);
    }
    else if(flag == BSPFLAG::Back)
    {
      BackList.push_back(pointCloud[j]);
      BackList.push_back(pointCloud[j + 1]);
      BackList.push_back(pointCloud[j + 2]);
    }
    else if (flag == BSPFLAG::Straddling)
    {
      std::vector<glm::vec3>points{ pointCloud[j], pointCloud[j + 1], pointCloud[j + 2] };
      SplitPolygon(points, plane, FrontList, BackList);
      /*
      FrontList.push_back(pointCloud[j]);
      FrontList.push_back(pointCloud[j + 1]);
      FrontList.push_back(pointCloud[j + 2]);

      BackList.push_back(pointCloud[j]);
      BackList.push_back(pointCloud[j + 1]);
      BackList.push_back(pointCloud[j + 2]);
      */
    }
  }
  //create two children
  pNode->children.push_back(BuildBSPTree(FrontList, maxDepth - 1));
  pNode->children.push_back(BuildBSPTree(BackList, maxDepth - 1));
  pNode->type = NodeType::Non_Terminal;

  //return node
  return pNode;
}

BSPNode* BuildBSPTree_Start(std::vector<glm::vec3> pointCloud, int maxDepth)
{
  auto start = std::chrono::system_clock::now();
  std::cout << "Creating BSPTree" << std::endl;

  BSPNode* tree = BuildBSPTree(pointCloud, maxDepth);
  
  tree->CalcMinMaxCenter();
  //read object
  auto endTime = std::chrono::system_clock::now();
  auto chronoDt = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - start);

  std::cout << "Time to Create BSPTree: " << chronoDt.count() << " Milliseconds" << std::endl;
  return tree;
}
