#include "BVHBS.h"
#include "BVHAABB.h"
#include "Object.h"
#include <algorithm>
#include "Wireframe.h"

#define MIN_PRIMATIVES 500

BoundingVolumeHeiarchyNode* BoundingVolumeHeiarchy_BS::BuildBottomUpNodeTree(std::vector < BoundingVolumeHeiarchyNode*> parents)
{
  //find closest bounding obj
  if (parents.size() == 1)
  {
    return parents.front();
  }
  std::vector < BoundingVolumeHeiarchyNode*> newParents;
  while (parents.size() >= 2)
  {
    float dist = std::numeric_limits<float>::max();
    unsigned obj1 = 0;
    unsigned obj2 = 1;
    for (unsigned i = 0; i < parents.size(); ++i)
    {
      for (unsigned j = 0; j < parents.size(); ++j)
      {
        if (i == j)
          continue;
        if (dist > glm::distance(parents[i]->center, parents[j]->center))
        {
          dist = glm::distance(parents[i]->center, parents[j]->center);
          obj1 = i;
          obj2 = j;
        }
      }
    }
    //2+ obj
    //obj1 and obj2 are the closest 2 objects

    std::vector < std::pair<Object*, glm::vec3>> objects;
    for (auto& obj : parents[obj1]->objects)
    {
      objects.push_back(obj);
    }
    for (auto& obj : parents[obj2]->objects)
    {
      objects.push_back(obj);
    }

    BoundingVolumeHeiarchyNode* parent = new BoundingVolumeHeiarchyNode;
    newParents.push_back(parent);

    //set center and objects
    parent->center = (parents[obj1]->center + parents[obj2]->center) / 2.0f;
    parent->objects = objects;
    parent->bv = reinterpret_cast<BoundingVolume*>(new BoundingSphereCentroid(objects));

    parent->type = NodeType::Non_Terminal;
    parent->bvType = BVHNodeBVType::AABB;
    //link nodes
    parent->left = parents[obj1];
    parent->right = parents[obj2];

    parents.erase(parents.begin() + obj1);
    //just removed an object
    parents.erase(parents.begin() + obj2 - 1);
  }
  if (parents.size() == 1)
  {
    std::vector < std::pair<Object*, glm::vec3>> objects;
    for (auto& obj : parents[0]->objects)
    {
      objects.push_back(obj);
    }

    BoundingVolumeHeiarchyNode* parent = new BoundingVolumeHeiarchyNode;
    newParents.push_back(parent);

    //set center and objects
    parent->center = parents[0]->center;
    parent->objects = objects;
    parent->bv = reinterpret_cast<BoundingVolume*>(new BoundingSphereCentroid(objects));

    parent->type = NodeType::Non_Terminal;
    parent->bvType = BVHNodeBVType::AABB;
    //link nodes
    parent->left = parents[0];

    parents.erase(parents.begin() + 0);
  }
  return BuildBottomUpNodeTree(newParents);
}

void BoundingVolumeHeiarchy_BS::BottomUp(BoundingVolumeHeiarchyNode* node,
  std::vector<std::pair<Object*, glm::vec3>>& obj)
{
  std::vector< BoundingVolumeHeiarchyNode*> parents;

  //turn all obj into nodes
  for (auto& object : obj)
  {
    std::vector<std::pair<Object*, glm::vec3>> objVec{ object };
    BoundingVolumeHeiarchyNode* parent = new BoundingVolumeHeiarchyNode;
    parents.push_back(parent);

    parent->bv = reinterpret_cast<BoundingVolume*>(new BoundingSphereCentroid(objVec));
    parent->objects = objVec;
    parent->type = NodeType::Leaf;
    parent->bvType = BVHNodeBVType::AABB;
    parent->center = objVec.front().first->center + objVec.front().second;
  }
  BoundingVolumeHeiarchyNode* rootPtr = BuildBottomUpNodeTree(parents);
  root = *rootPtr;
  //free that one node
  delete rootPtr;
  root.level = 0;
  setNodeLevels(&root);
}

BoundingVolumeHeiarchy_BS::BoundingVolumeHeiarchy_BS(std::vector<std::pair<Object*, glm::vec3>> obj, bool topDown)
{
  if (topDown)
  {
    root.level = 0;
    std::sort(obj.begin(), obj.end(), splitX);
    TopDown(&root, obj);
  }
  else //bottom up 
  {
    root.level = 0;
    std::sort(obj.begin(), obj.end(), splitX);
    BottomUp(&root, obj);
  }
  setColor(&root);
  perNode(setColor, &root);
}


BoundingVolumeHeiarchy_BS::~BoundingVolumeHeiarchy_BS()
{
  deleteNodeBVHAABB(root.left);
  deleteNodeBVHAABB(root.right);
  if (root.bv != nullptr)
    delete reinterpret_cast<BoundingSphereCentroid*>(root.bv);
}


void BoundingVolumeHeiarchy_BS::TopDown(BoundingVolumeHeiarchyNode* node,
  std::vector<std::pair<Object*, glm::vec3>>& obj)
{
  node->level++;


  if (obj.size() == 1)
  {
    node->bv = reinterpret_cast<BoundingVolume*>(new BoundingSphereCentroid(obj));
    node->objects = obj;
    node->type = NodeType::Leaf;
    node->bvType = BVHNodeBVType::BS;
  }
  else if (obj.size() >= 2)
  {
    glm::vec3 min, max;

    min = glm::vec3{ std::numeric_limits<float>::max() };
    max = glm::vec3{ std::numeric_limits<float>::min() };

    for (unsigned i = 0; i < obj.size(); ++i)
    {

      glm::vec3 objmin = obj[i].first->modelMatrix * glm::vec4(obj[i].first->min, 1);
      glm::vec3 objmax = obj[i].first->modelMatrix * glm::vec4(obj[i].first->max, 1);

      objmin += obj[i].second;
      objmax += obj[i].second;

      if (objmin.x < min.x)
        min.x = objmin.x;
      if (objmin.y < min.y)
        min.y = objmin.y;
      if (objmin.z < min.z)
        min.z = objmin.z;

      if (objmax.x > max.x)
        max.x = objmax.x;
      if (objmax.y > max.y)
        max.y = objmax.y;
      if (objmax.z > max.z)
        max.z = objmax.z;
    }

    glm::vec3 center = ((max + min) / 2.0f);

    if ((glm::abs(center.x) > glm::abs(center.y)) && (glm::abs(center.x) > glm::abs(center.z)))
    {
      //x max
      std::sort(obj.begin(), obj.end(), splitX);
    }
    else if (glm::abs(center.y) > glm::abs(center.z))
    {
      //y max
      std::sort(obj.begin(), obj.end(), splitY);
    }
    else //z max
      std::sort(obj.begin(), obj.end(), splitZ);

    node->bv = reinterpret_cast<BoundingVolume*>(new BoundingSphereCentroid(obj));
    node->objects = obj;
    std::vector < std::pair<Object*, glm::vec3>> lhs{ obj.begin(), obj.begin() + (obj.size() / 2) };
    std::vector < std::pair<Object*, glm::vec3>> rhs{ obj.begin() + (obj.size() / 2), obj.end() };

    node->type = NodeType::Non_Terminal;
    node->bvType = BVHNodeBVType::BS;

    node->left = new BoundingVolumeHeiarchyNode;
    node->right = new BoundingVolumeHeiarchyNode;

    node->left->level = node->level;
    node->right->level = node->level;

    TopDown(node->left, lhs);
    TopDown(node->right, rhs);
  }
}

std::vector<Wireframe> BoundingVolumeHeiarchy_BS::make_wireframes(Object& sphere)
{
  std::vector<Wireframe> wireframes;
  makeWireframes(wireframes, &root, sphere);
  return wireframes;
}

void BoundingVolumeHeiarchy_BS::makeWireframes(std::vector<Wireframe>& wireframes, BoundingVolumeHeiarchyNode* node, Object& sphere)
{
  if (node->bv == nullptr)
    return;

  wireframes.push_back({ *reinterpret_cast<BoundingSphereCentroid*>(node->bv), sphere});
  wireframes.back().color = node->color;

  if (node->left != nullptr)
  {
    makeWireframes(wireframes, node->left, sphere);
  }
  if (node->right != nullptr)
  {
    makeWireframes(wireframes, node->right, sphere);
  }

}
