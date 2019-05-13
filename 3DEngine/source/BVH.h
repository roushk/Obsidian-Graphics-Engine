#pragma once
#include "BoundingVolume.h"
#include "Object.h"
#include "AxisAlignedBoundingBox.h"
#include <vector>
#include "NodeTypes.h"


enum class BVHNodeBVType
{
  AABB,
  BS,
};


struct BoundingVolumeHeiarchyNode
{
  BoundingVolume* bv = nullptr;

  NodeType type;
  BVHNodeBVType bvType;

  //converts to actual BV type
  AxisAlignedBoundingBox* GetBV() const
  {
    return reinterpret_cast<AxisAlignedBoundingBox*>(bv);
  };

  glm::vec3 center; //used for bottom up

  int level = -1;
  glm::vec4 color{ 0,0,0,1 };

  BoundingVolumeHeiarchyNode* left = nullptr;
  BoundingVolumeHeiarchyNode* right = nullptr;
  std::vector<std::pair<Object*, glm::vec3>> objects;
};


inline bool splitX(const std::pair<Object*, glm::vec3>& lhs, const std::pair<Object*, glm::vec3>& rhs)
{
  return (lhs.first->center.x < rhs.first->center.x);
}


inline bool splitY(const std::pair<Object*, glm::vec3>& lhs, const std::pair<Object*, glm::vec3>& rhs)
{
  return (lhs.first->center.y < rhs.first->center.y);
}


inline bool splitZ(const std::pair<Object*, glm::vec3>& lhs, const std::pair<Object*, glm::vec3>& rhs)
{
  return (lhs.first->center.z < rhs.first->center.z);
}


inline unsigned getSize(std::vector<std::pair<Object*, glm::vec3>>& obj)
{
  unsigned size = 0;
  for (auto& object : obj)
  {
    size += object.first->verts.size();
  }
  return size;
}
inline void setNodeLevels(BoundingVolumeHeiarchyNode* root)
{
  if(root->left != nullptr)
  {
    root->left->level = root->level + 1;
    setNodeLevels(root->left);
  }
  if (root->right != nullptr)
  {
    root->right->level = root->level + 1;
    setNodeLevels(root->right);
  }
}

inline void deleteNodeBVHAABB(BoundingVolumeHeiarchyNode*node)
{
  if (node != nullptr)
  {

    deleteNodeBVHAABB(node->left);
    deleteNodeBVHAABB(node->right);
    if (node->bv != nullptr)
      delete reinterpret_cast<AxisAlignedBoundingBox*>(node->bv);
    if (node != nullptr)
      delete node;
  }
}


inline void setColor(BoundingVolumeHeiarchyNode* node)
{
  if (node->level == 0)
  {
    //red
    node->color = { 1,0,0,1 };
  }
  else if (node->level == 1)
  {
    //light orange
    node->color = { 1,0.3f,0,1 };
  }
  else if (node->level == 2)
  {
    //dark orange
    node->color = { 1,0.7f,0,1 };
  }
  else if (node->level == 3)
  {
    //yellow
    node->color = { 1,1,0,1 };
  }
  else if (node->level == 4)
  {
    //green
    node->color = { 0,1,0,1 };
  }
  else if (node->level == 5)
  {
    // blue
    node->color = { 0,0,1,1 };
  }
  else if (node->level == 6)
  {
    //purple
    node->color = { 1,0,1,1 };
  }
}

