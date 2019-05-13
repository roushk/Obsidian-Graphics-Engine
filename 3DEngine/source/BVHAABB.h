#pragma once
#include "AxisAlignedBoundingBox.h"
#include "BVH.h"
#include <functional>
#include "Wireframe.h"



class BoundingVolumeHeiarchy_AABB
{
public:
  BoundingVolumeHeiarchy_AABB(std::vector<std::pair<Object*, glm::vec3>> offsets, bool topDown = true);
  ~BoundingVolumeHeiarchy_AABB();
  AxisAlignedBoundingBox* GetBV()
  {
    return reinterpret_cast<AxisAlignedBoundingBox*>(root.bv);
  };

  void TopDown(BoundingVolumeHeiarchyNode* node, std::vector<std::pair<Object*, glm::vec3>>& obj);
  void BottomUp(BoundingVolumeHeiarchyNode* node,
    std::vector<std::pair<Object*, glm::vec3>>& obj);
  BoundingVolumeHeiarchyNode* BuildBottomUpNodeTree(std::vector < BoundingVolumeHeiarchyNode*> parents);


  BoundingVolumeHeiarchyNode root;

  std::vector<Wireframe> make_wireframes();
  void makeWireframes(std::vector<Wireframe>& wireframes, BoundingVolumeHeiarchyNode* node);

  void perNode(std::function<void(BoundingVolumeHeiarchyNode*n)> func, BoundingVolumeHeiarchyNode* node)
  {
    if (node->left != nullptr)
    {
      func(node->left);
      perNode(func, node->left);
    }
    if (node->right != nullptr)
    {
      func(node->right);
      perNode(func, node->right);
    }    
  }

};

