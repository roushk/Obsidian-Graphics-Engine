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

#include "Octree.h"
#include "Object.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include "Random.h"
#include <chrono>
#include <iostream>

//checks if P is within cube created by center +- halfWidth
bool pointWithin(Point p, Point center, float halfWidth)
{
  //p is within cube defined by center +- halfwidth 
  //using not because want to quickly exit instead of checking all of them
  //this method is faster to find if a vert is outside than if a vert is inside
  //and because we are going from a size to 1/8 the size this should be faster
 /*
  return(p.x >= center.x - halfWidth
      && p.y >= center.y - halfWidth
      && p.z >= center.z - halfWidth
      && p.x <= center.x + halfWidth
      && p.y <= center.y + halfWidth
      && p.z <= center.z + halfWidth);
      */
  return !(p.x <= center.x - halfWidth
        || p.y < center.y - halfWidth
        || p.z < center.z - halfWidth
        || p.x > center.x + halfWidth
        || p.y > center.y + halfWidth
        || p.z > center.z + halfWidth);
}

//pushes back object's verts onto pointCloud
void createPointCloudFromObj(const Object & obj, std::vector<glm::vec3>& pointCloud, const glm::vec3& transform)
{
  //for each face containing 3 verts
  //push back all verts so you can use glDrawArrays

  pointCloud.reserve(obj.faces.size() * 3);
  for(auto& face: obj.faces)
  {
    pointCloud.push_back(glm::translate(transform) * obj.modelMatrix * glm::vec4(obj.verts[face.x], 1));
    pointCloud.push_back(glm::translate(transform) * obj.modelMatrix * glm::vec4(obj.verts[face.y], 1));
    pointCloud.push_back(glm::translate(transform) * obj.modelMatrix * glm::vec4(obj.verts[face.z], 1));
  }
}

void OctreeNode::createPointsWireframe()
{
  wireframe_verts.reserve(12 * 2);
  std::vector<glm::vec3> subverts;
  std::vector<glm::uvec2> faces;
  glm::vec3 min = center + halfWidth;
  glm::vec3 max = center - halfWidth;


  //assuming AABB is in world space
  glm::vec3 a{ min.x, max.y, min.z };
  glm::vec3 b = min;
  glm::vec3 c{ min.x, min.y, max.z };
  glm::vec3 d{ min.x, max.y, max.z };

  glm::vec3 e = max;
  glm::vec3 f{ max.x, min.y, max.z };
  glm::vec3 g{ max.x, min.y, min.z };
  glm::vec3 h{ max.x, max.y, min.z };

  /*
   * based on cube with lower left corner b and upper right corner e
   */

  subverts.reserve(8);
  faces.reserve(12);

  subverts.push_back(a); //vert 0
  subverts.push_back(b); //vert 1
  subverts.push_back(c); //vert 2
  subverts.push_back(d); //vert 3
  subverts.push_back(e); //vert 4
  subverts.push_back(f); //vert 5
  subverts.push_back(g); //vert 6
  subverts.push_back(h); //vert 7

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

  for(auto& face: faces)
  {
    wireframe_verts.push_back(subverts[face.x]);
    wireframe_verts.push_back(subverts[face.y]);

  }
}

void OctreeNode::BindVertsAndFaces()
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

void OctreeNode::BindWireframeVertsAndFaces()
{

  glGenVertexArrays(1, &wireframe_VAO);
  glGenBuffers(1, wireframe_VBO);
  glBindVertexArray(wireframe_VAO);

  //2
  glEnableVertexAttribArray(0);

  //verts
  glBindBuffer(GL_ARRAY_BUFFER, wireframe_VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, wireframe_verts.size() * sizeof glm::vec3, wireframe_verts.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindVertexArray(0);
}

void clearOctreeNode(OctreeNode* node)
{
  glBindVertexArray(0);
  //clear children
  for(auto*subNode: node->children)
  {
    clearOctreeNode(subNode);
  }
  
  //clear opengl stuff
  //dont need delete buffers because delete vert arrays cleans it up
  //for you
  //glDeleteBuffers(1, node->VBO);
  glDeleteVertexArrays(1, &node->VAO);

  //glDeleteBuffers(1, node->wireframe_VBO);
  glDeleteVertexArrays(1, &node->wireframe_VAO);


  //clear self
  delete(node);
}

OctreeNode* BuildOctree(Point center, float halfWidth,
  std::vector<glm::vec3> pointCloud)
{

  OctreeNode* pNode = new OctreeNode;
  pNode->center = center;
  pNode->halfWidth = halfWidth;

  //if not too many verts (instead of end of depth)
  //300 verts * 3 (3 verts per polygon) = 900
  if(pointCloud.size() <= 900)
  {
    pNode->verts = pointCloud;

    pNode->BindVertsAndFaces();

    //creates wireframe from set values
    pNode->createPointsWireframe();
    pNode->BindWireframeVertsAndFaces();

    pNode->color.x = RandomValue(0, 1000) / 1000.0f;
    pNode->color.y = RandomValue(0, 1000) / 1000.0f;
    pNode->color.z = RandomValue(0, 1000) / 1000.0f;
    if(pNode->verts.size() > 0)
      pNode->type = NodeType::Leaf;
    else
      pNode->type = NodeType::Non_Terminal;
    return pNode;
  }
  pNode->type = NodeType::Non_Terminal;
  pNode->children.reserve(8);
  //recursively create children because too many verts
  Point offset;
  float step = halfWidth * 0.5f;
  for(unsigned i = 0; i < 8; ++i)
  {
    offset.x = ((i & 1) ? step : -step);
    offset.y = ((i & 2) ? step : -step);
    offset.z = ((i & 4) ? step : -step);
    std::vector<glm::vec3> subPointCloud;

    //get all verts within this subsection
    //put these verts into new leaf
    //for each 3 verts
    for(unsigned j = 0; pointCloud.size() > 2 && j + 2 < pointCloud.size() + 2; j += 3)
    {
      //vert 1 = pointCloud[j]
      //vert 2 = pointCloud[j + 1]
      //vert 3 = pointCloud[j + 2]

      bool p1 = pointWithin(pointCloud[j], center + offset, step);
      bool p2 = pointWithin(pointCloud[j + 1], center + offset, step);
      bool p3 = pointWithin(pointCloud[j + 2], center + offset, step);

      /*
      if 1 or 2 verts are in
        add all 3
      if all 3 verts are in
        add all 3
        remove all 3 from the list
      */
      if(p1 || p2)
      {
        subPointCloud.push_back(pointCloud[j]);
        subPointCloud.push_back(pointCloud[j + 1]);
        subPointCloud.push_back(pointCloud[j + 2]);
        //if entire tri is inside of point
        if(p3)
        {
          //remove points from pointCloud
          //erase() is exclusive of the end iterator
          //pointCloud.erase(pointCloud.begin() + j, pointCloud.begin() + j + 3);
          //j -= 3;
        }
      }
    }
    pNode->children.push_back(BuildOctree(center + offset, step, subPointCloud));
  }
  return pNode;

}



OctreeNode* BuildOctree_Start(std::vector<glm::vec3> pointCloud)
{
  auto start = std::chrono::system_clock::now();
  std::cout << "Creating Octree" << std::endl;
  vec3 maxSize(pointCloud[0]);
  vec3 minSize(pointCloud[0]);

  for (auto& vert : pointCloud)
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

  glm::vec3 center = (maxSize + minSize) / 2.0f;
  //halfWidth is the maximum distnace in
  float halfWidth = max(max(center.x - minSize.x, center.y - minSize.y), center.z - minSize.z);

  OctreeNode* tree = BuildOctree(center, halfWidth, pointCloud);
  //read object
  auto endTime = std::chrono::system_clock::now();
  auto chronoDt = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - start);

  std::cout << "Time to Create Octree: " << chronoDt.count() << " Milliseconds" << std::endl;
  return tree;
}


