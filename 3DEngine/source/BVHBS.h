#pragma once
#include "BoundingSphereCentroid.h"
#include "BVH.h"
#include <vector>
#include <functional>
#include "Wireframe.h"

class BoundingVolumeHeiarchy_BS
{
public:
  BoundingVolumeHeiarchy_BS(std::vector<std::pair<Object*, glm::vec3>> offsets, bool topDown = true);
  ~BoundingVolumeHeiarchy_BS();
  void TopDown(BoundingVolumeHeiarchyNode* node, std::vector<std::pair<Object*, glm::vec3>>& obj);
  void BottomUp(BoundingVolumeHeiarchyNode* node,
    std::vector<std::pair<Object*, glm::vec3>>& obj);
  BoundingVolumeHeiarchyNode* BuildBottomUpNodeTree(std::vector < BoundingVolumeHeiarchyNode*> parents);

  BoundingVolumeHeiarchyNode root;

  std::vector<Wireframe> make_wireframes(Object& sphere);
  void makeWireframes(std::vector<Wireframe>& wireframes, BoundingVolumeHeiarchyNode* node, Object& sphere);

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



