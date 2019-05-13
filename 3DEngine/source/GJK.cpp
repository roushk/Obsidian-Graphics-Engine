/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: GJK.cpp
Purpose: gjk implementation
Project: coleman.jonas_CS350_4
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 4/19/19
End Header --------------------------------------------------------*/

#include "GJK.h"
#include "Octree.h"
#include "AxisAlignedBoundingBox.h";
#include "BoundingVolume.h"
#include "BVHAABB.h"
#include "BSPTree.h"
#include <iostream>

glm::vec3 SphereSupport(const Sphere& sphere, const glm::vec3& worldDirection)
{
  return sphere.center + sphere.radius * normalize(worldDirection);
}

glm::vec3 SupportObject(const glm::vec3& worldDirection, const std::vector<glm::vec3>& localPoints)
{
  auto projMax = std::numeric_limits<float>::lowest();
  auto result = glm::vec3(0);
  for (const auto& point : localPoints)
  {
    const auto projection = dot(point, worldDirection);

    if (projection > projMax)
    {
      projMax = projection;
      result = point;
    }
  }

  return result;
}



//aabb between two aabb objects
bool AABB_to_SphereAABB(const AxisAlignedBoundingBox& obj1, const Sphere& obj2)
{

  return 
    (obj1.min.x <= obj2.center.x + obj2.radius && obj1.max.x >= obj2.center.x - obj2.radius) &&
    (obj1.min.y <= obj2.center.y + obj2.radius && obj1.max.y >= obj2.center.y - obj2.radius) &&
    (obj1.min.z <= obj2.center.z + obj2.radius && obj1.max.z >= obj2.center.z - obj2.radius);
}

//aabb between two octree nodes
bool Octree_to_SphereAABB(const OctreeNode* obj1, const Sphere& obj2)
{
  return  
    (obj1->center.x - obj1->halfWidth <= obj2.center.x + obj2.radius && obj1->center.x + obj1->halfWidth >= obj2.center.x - obj2.radius) &&
    (obj1->center.y - obj1->halfWidth <= obj2.center.y + obj2.radius && obj1->center.y + obj1->halfWidth >= obj2.center.y - obj2.radius) &&
    (obj1->center.z - obj1->halfWidth <= obj2.center.z + obj2.radius && obj1->center.z + obj1->halfWidth >= obj2.center.z - obj2.radius);
}

glm::vec2 LineBary(glm::vec3& a, glm::vec3& b)
{
  return glm::vec2(dot(b.p, b.p - a.p), dot(a.p, a.p - b.p));
}

glm::vec3 TriBary(glm::vec3& a, glm::vec3& b, glm::vec3& c)
{
  glm::vec3 ABC;
  glm::vec3 normal = cross(b - a, c - a);
  ABC.x = dot(cross(b, c), normal);
  ABC.y = dot(cross(c, a), normal);
  ABC.z = dot(cross(a, b), normal);
  return ABC;
}

glm::vec4 TetBary(glm::vec3& a, glm::vec3& b, glm::vec3& c, glm::vec3& d)
{
  glm::vec4 ABCD;

  float volume = dot(c - b, cross(a - b, d - b));

  //if plane
  if (volume == 0)
    volume = 1;
  volume = 1.0f / volume;
  //a * ( b x c)
  ABCD.x = dot(c, cross(d, b)) * volume;
  ABCD.y = dot(c, cross(a, d)) * volume;
  ABCD.z = dot(d, cross(a, b)) * volume;
  ABCD.w = dot(d, cross(a, c)) * volume;
  return ABCD;
}

bool BSP_to_SphereAABB(const BSPNode* obj1, const Sphere& obj2)
{

  return
    (obj1->minSize.x <= obj2.center.x + obj2.radius && obj1->maxSize.x >= obj2.center.x - obj2.radius) &&
    (obj1->minSize.y <= obj2.center.y + obj2.radius && obj1->maxSize.y >= obj2.center.y - obj2.radius) &&
    (obj1->minSize.z <= obj2.center.z + obj2.radius && obj1->maxSize.z >= obj2.center.z - obj2.radius);
}

void Simplex::AddPoint(glm::vec3& point)
{

  if(count == 0)
  {
    a.p = point;
  }
  if (count == 1)
  {
    b.p = point;
  }
  if (count == 2)
  {
    c.p = point;
  }
  if (count == 3)
  {
    d.p = point;
  }
  //no count 4

  ++count;
}

void Simplex::Line()
{
  
  glm::vec2 AB = LineBary(a.p, b.p);

  //inside a
  if(AB.y <= 0)
  {
    //fully in a
    a.bary = 1;
    count = 1;
    
  }
  //inside b
  else if(AB.x <= 0)
  {
    b.bary = 1;
    count = 1;
  }

  a.bary = AB.x;
  b.bary = AB.y;
  count = 2;

}

void Simplex::Tri()
{

  //all 3 line segments
  //u = x
  //v = y
  //z = w
  glm::vec2 AB, BC, CA;

  //inside tri 
  glm::vec3 ABC;

  AB = LineBary(a.p, b.p);
  BC = LineBary(b.p, c.p);
  CA = LineBary(c.p, a.p);
  
  //inside a 
  if (AB.y <= 0 && CA.x <= 0)
  {
    //fully in a
    a.bary = 1;
    count = 1;
    return;
  }
  
  //inside b
  if (AB.x <= 0 && BC.y <= 0)
  {
    //switch points
    a = b;
    a.bary = 1;
    count = 1;
    return;
  }

  //inside c
  if(BC.x <= 0 && CA.y <= 0)
  {
    //switch points
    a = c;
    a.bary = 1;
    count = 1;
    return;
  }

  //must be inside closer areas
  ABC = TriBary(a.p, b.p, c.p);


  //inside ab
  if (AB.x > 0 && AB.y > 0 && ABC.z <= 0)
  {
    a.bary = AB.x;
    b.bary = AB.y;
    count = 2;
    return;
  }
  //inside bc
  if (BC.x > 0 && BC.y > 0 && ABC.x <= 0)
  {
    //Swap
    a = b;
    b = c;
    a.bary = BC.x;
    b.bary = BC.y;
    count = 2;
    return;
  }
  //inside ca
  if (CA.x > 0 && CA.y > 0 && ABC.y <= 0)
  {
    //Swap
    b = a;
    a = c;
    a.bary = CA.x;
    b.bary = CA.y;
    count = 2;
    return;
  }
  //inside abc triangle

  a.bary = ABC.x;
  b.bary = ABC.y;
  c.bary = ABC.z;
  count = 3;

}

void Simplex::Tet()
{

  //all 6 line segments
  //u = x
  //v = y
  //z = w
  glm::vec2 AB, BC, CA, BD, DC, AD;

  //all 4 triangles
  glm::vec3 ABC;
  glm::vec3 ADB;
  glm::vec3 ACD;
  glm::vec3 CBD;


  //tetrahedron 
  glm::vec4 ABCD;

  AB = LineBary(a.p, b.p);
  BC = LineBary(b.p, c.p);
  CA = LineBary(c.p, a.p);
  BD = LineBary(b.p, d.p);
  DC = LineBary(d.p, c.p);
  AD = LineBary(a.p, d.p);


  //inside a 
  if (AB.y <= 0 && CA.x <= 0 && AD.y <= 0)
  {
    //fully in a
    a.bary = 1;
    count = 1;
    return;
  }

  //inside b
  if (AB.x <= 0 && BC.y <= 0 && BD.y <= 0)
  {
    //switch points
    a = b;
    a.bary = 1;
    count = 1;
    return;
  }

  //inside c
  if (BC.x <= 0 && CA.y <= 0 && DC.x <= 0)
  {
    //switch points
    a = c;
    a.bary = 1;
    count = 1;
    return;
  }

  //inside d
  if (BD.x <= 0 && CA.y <= 0 && AD.x <= 0)
  {
    //switch points
    a = d;
    a.bary = 1;
    count = 1;
    return;
  }

  ABC = TriBary(a.p, b.p, c.p);
  ADB = TriBary(a.p, d.p, b.p);
  ACD = TriBary(a.p, c.p, d.p);
  CBD = TriBary(c.p, b.p, d.p);

  
  //inside ab
  if (AB.x > 0 && AB.y > 0 && ABC.z <= 0 && ADB.y <= 0)
  {
    a.bary = AB.x;
    b.bary = AB.y;
    count = 2;
    return;
  }
  //inside bc
  if (BC.x > 0 && BC.y > 0 && ABC.x <= 0 && CBD.z <= 0)
  {
    //Swap
    a = b;
    b = c;
    a.bary = BC.x;
    b.bary = BC.y;
    count = 2;
    return;
  }
  
  //inside ca
  if (CA.x > 0 && CA.y > 0 && ABC.y <= 0 && ACD.z <= 0)
  {
    //Swap
    b = a;
    a = c;
    a.bary = AB.x;
    b.bary = AB.y;
    count = 2;
    return;
  }

  //inside dc
  if (DC.x > 0 && DC.y > 0 && CBD.y <= 0 && ACD.x <= 0)
  {
    //Swap
    a = d;
    b = c;
    a.bary = DC.x;
    b.bary = DC.y;
    count = 2;
    return;
  }

  //inside ad
  if (AD.x > 0 && AD.y > 0 && ACD.y <= 0 && ADB.z <= 0)
  {
    //Swap
    b = d;
    a.bary = AD.x;
    b.bary = AD.y;
    count = 2;
    return;
  }

  //inside bd
  if (BD.x > 0 && BD.y > 0 && CBD.x <= 0 && ADB.x <= 0)
  {
    //Swap
    a = b;
    b = d;
    a.bary = BD.x;
    b.bary = BD.y;
    count = 2;
    return;
  }


  ABCD = TetBary(a.p, b.p, c.p, d.p);
  //inside abc triangle
  if (ABC.x > 0 && ABC.y > 0 && ABC.z > 0 && ABCD.w <= 0)
  {
    a.bary = ABC.x;
    b.bary = ABC.y;
    c.bary = ABC.z;
    count = 3;
    return;
  }
  
  //inside cbd triangle
  if (CBD.x > 0 && CBD.y > 0 && CBD.z > 0 && ABCD.x <= 0)
  {
    a = c;
    c = d;
    a.bary = CBD.x;
    b.bary = CBD.y;
    c.bary = CBD.z;
    count = 3;

    return;
  }

  //inside acd triangle
  if (ACD.x > 0 && ACD.y > 0 && ACD.z > 0 && ABCD.y <= 0)
  {
    b = c;
    c = d;
    a.bary = ACD.x;
    b.bary = ACD.y;
    c.bary = ACD.z;
    count = 3;

    return;
  }

  //inside adb triangle
  if (ADB.x > 0 && ADB.y > 0 && ADB.z > 0 && ABCD.z <= 0)
  {
    c = b;
    b = d;
    a.bary = ADB.x;
    b.bary = ADB.y;
    c.bary = ADB.z;

    count = 3;
    return;
  }

  
  //inside abcd tetrahedron
 
  a.bary = ABCD.x;
  b.bary = ABCD.y;
  c.bary = ABCD.z;
  d.bary = ABCD.w;

  count = 4;


}

glm::vec3 Simplex::getDirection()
{

  if(count == 1)
  {
    //facing other direction vector
    return glm::vec3(0) - a.p; 
  }
  if (count == 2)
  {
    //cross cross for line and get normal of line
    return cross(cross(a.p - b.p, -a.p), a.p - b.p);
  }
  if (count == 3)
  {
    //cross for plane then dot for correct sided normal
    glm::vec3 n = cross(b.p - a.p, c.p - a.p);
    if (dot(n, a.p) <= 0)
      return n;
    return -n;
  }

  return glm::vec3(0, 0, 0);
}

//finds barycentric point of simplex
glm::vec3 Simplex::BaryPoint()
{
  if (count == 1)
  {
    return a.p;
  }
  if(count == 2)
  {
    return (a.p * a.bary + b.p * b.bary);
  }
  if (count == 3)
  {
    return (a.p * a.bary + b.p *  b.bary + c.p * c.bary);
  }
  if (count == 4)
  {
    return (a.p *  a.bary + b.p * b.bary + c.p *  c.bary + d.p * d.bary);
  }
  return glm::vec3(0, 0, 0);
}

bool GJK::checkCollision_broad(const Sphere& obj, BoundingVolumeHeiarchy_AABB& bvh)
{
  auto* bv = reinterpret_cast<AxisAlignedBoundingBox*>(bvh.root.bv);

  //wide phase
  if(!AABB_to_SphereAABB(*bv, obj))
    return false;

  if( checkCollision_mid(obj, bvh.root.left))
  {
    //bvh.root.left->color = glm::vec4(1, 1, 1, 1);
    return true;
  }
  if (checkCollision_mid(obj, bvh.root.right))
  {
    //bvh.root.right->color = glm::vec4(1, 1, 1, 1);
    return true;
  }
  return false;
}

bool GJK::checkCollision_broad(const Sphere& obj, OctreeNode* node)
{
  bool collide = false;
  if (!Octree_to_SphereAABB(node, obj))
    return false;

  //node->color = glm::vec3(1, 1, 1);
  for (auto& child : node->children)
  {
    if (checkCollision_mid(obj, child))
    {
      collide = true;
      //child->color = glm::vec3(1, 1, 1);
      // Render polygons of treeNode & sphere in RED color
    }
  }
  return collide;
}

bool GJK::checkCollision_broad(const Sphere& obj, BSPNode* node)
{
  bool collide = false;
  if (!BSP_to_SphereAABB(node, obj))
    return false;

  for (auto& child : node->children)
  {
    if (checkCollision_mid(obj, child))
    {
      collide = true;
      //child->color = glm::vec3(1, 1, 1);
      // Render polygons of treeNode & sphere in RED color
    }
  }
  return collide;
}


bool GJK::checkCollision_mid(const Sphere& obj, BoundingVolumeHeiarchyNode* bvh)
{
  if (bvh == nullptr)
    return false;

  //continue down list of objects
  auto* bv = bvh->GetBV();
  if (!AABB_to_SphereAABB(*bv, obj))
    return false;

  bool collide = false;
  if(bvh->type == NodeType::Leaf)
  {
    //most precise detection
    if(checkCollision_narrow(obj, bvh))
    {
      std::cout << "colliding BVH Narrow" << std::endl;
      //bvh->color = glm::vec4(1, 1, 1,1);
      collide = true;
      // Render polygons of treeNode & sphere in RED color*
    }
  }


  if (checkCollision_mid(obj, bvh->left))
  {
    //bvh->left->color = glm::vec4(1, 1, 1, 1);
    collide = true;
  }
  if (checkCollision_mid(obj, bvh->right))
  {
    //bvh->right->color = glm::vec4(1, 1, 1, 1);
    collide = true;
  }
  return collide;
}


bool GJK::checkCollision_mid(const Sphere& obj, OctreeNode* node)
{
  if (node == nullptr)
    return false;
  
  if (!Octree_to_SphereAABB(node, obj))
    return false;

  bool collide = false;

  //if (node->type == NodeType::Leaf)
  if (!node->verts.empty())
  {
    //most precise detection
    //node->color = glm::vec3(1, 1, 1);
    if (checkCollision_narrow(obj, node))
    {
      std::cout << "colliding Octree Narrow" << std::endl;
      //node->color = glm::vec3(1, 1, 1);
      collide = true;
      // Render polygons of treeNode & sphere in RED color
    }
  }

  //continue down list of objects

  for (auto& child : node->children)
  {
    if (checkCollision_mid(obj, child))
    {
      //child->color = glm::vec3(1, 1, 1);
      collide = true;
      // Render polygons of treeNode & sphere in RED color
    }
  }
  return collide;
}


bool GJK::checkCollision_mid(const Sphere& obj, BSPNode* node)
{
  if (node == nullptr)
    return false;
  
  if (!BSP_to_SphereAABB(node, obj))
    return false;

  bool collide = false;
  if (!node->verts.empty())
  {

    //most precise detection
    if (checkCollision_narrow(obj, node))
    {
      std::cout << "colliding BSP Narrow" << std::endl;      
      //node->color = glm::vec3(1, 1, 1);
      collide = true;
      // Render polygons of treeNode & sphere in RED color
    }
  }

  for (auto& child : node->children)
  {
    if (checkCollision_mid(obj, child))
    {
      //child->color = glm::vec3(1, 1, 1);
      collide = true;
      // Render polygons of treeNode & sphere in RED color
    }
  }
  return collide;
}


bool GJK::checkCollision_narrow(const Sphere& obj, BoundingVolumeHeiarchyNode* bvh)
{

  std::vector<glm::vec3> verts;
  int reserveSize = 0;
  for(auto& objbvh: bvh->objects)
  {
    reserveSize += objbvh.first->verts.size();
  }

  verts.reserve(reserveSize);
  for (auto& objbvh : bvh->objects)
  {
    for (auto& vert : objbvh.first->verts)
    {
      verts.push_back(glm::vec4(vert, 0) * objbvh.first->modelMatrix);
    }
  }

  bool collide = checkCollision_GJK(obj, verts);
  if(collide)
  {
    std::cout << "colliding AABB BVH Narrow" << std::endl;
  }
  return collide;
}

bool GJK::checkCollision_narrow(const Sphere& obj, OctreeNode* node)
{
  return checkCollision_GJK(obj, node->verts);
}

bool GJK::checkCollision_narrow(const Sphere& obj, BSPNode* node)
{
  return checkCollision_GJK(obj, node->verts);
}

bool GJK::checkCollision_GJK(const Sphere& obj, const std::vector<glm::vec3>& pointCloud)
{
  int iterations = 0;
  int max_iterations = 10000;
  Simplex simplex;

  //init sphere closest point to first point of cloud
  simplex.a.p = obj.center - pointCloud[0];
  
  simplex.count = 1;

  while (iterations < max_iterations)
  {
    ++iterations;
    //find closest point

    //switch on how many simplexes we have 1,2,3,4 points so far
    //attempt to simplify simplex
    switch (simplex.count)
    {
    case 1:
      break;
    case 2:
      simplex.Line();
      break;
    case 3:
      simplex.Tri();
      break;
    case 4:
      simplex.Tet();
      break;

    default:
      break;
    }

    //point within origin and simplex not simplified
    if (simplex.count == 4)
    {
      return true;
    }

    //compute new simplex vertex using support points
    //create candidate sphere support dir from sphere center to arbitrary point
    glm::vec3 direction = simplex.getDirection();

    //sphere support func to get closest vector
    glm::vec3 sphereSupp = SphereSupport(obj, direction);

    //std::cout << sphereSupp.x << "| " << sphereSupp.y << "| " << sphereSupp.z << "| " 
    //<< obj.center.x << "| " << obj.center.y << "| " << obj.center.z << std::endl;

    glm::vec3 objectSupp = SupportObject(-direction, pointCloud);

    glm::vec3 supp = sphereSupp - objectSupp;

    //if duplicate points we done
    if (supp == simplex.a.p ||
      supp == simplex.b.p ||
      supp == simplex.c.p ||
      supp == simplex.d.p)
    {
      //std::cout << "Duplicate Point" << std::endl;
      return false;
    }

    //commit new support points to simplex
    simplex.AddPoint(supp);

  }

  //if (iterations >= max_iterations)
    //std::cout << "Max iteration timeout" << std::endl;
  return false;
}
  

/*
bool DetectCollision_BroadPhase(Sphere S, Node *RootNode)
{
  // The broad phase part of collision detection
  BBox worldBox = RootNode->getBBox();
  BBox sphereBox = S->getBBox();
  // standard box-box intersection
  if (!BBOX_intersection(worldBox, sphereBox))
    return false;
  // boxes collide, move to mid-phase
  // In mid-phase, we check for intersection of S with internal
  nodes
    for each childNode of the RootNode
    {
      // For BVH and Octree: box-box collision
      if (DetectCollision_MidPhase(S, childNode))
      return true;
    }
      // no intersection with the scene
  return false;
}
*/


/*
bool DetectCollision_MidPhase(Sphere S, Node *treeNode)
{
  // if treeNode == LEAF, then perform GJK
  if (treeNode->type == LEAF_NODE)
  {
    // Potential approaches for GJK with S
    // a. (pre-)Calculate the convex hull of treeNode geometry –
    most precise calculation
      // b. use BBox of treeNode geometry – approximate solution
      // c. Level-of-detail based pseudo-geometry for dynamic
      objects
      // We will use approach b. for our assignment
      if (DetectCollision_GJK(S->getBBox(), treenode)
      {
        // Render polygons of treeNode & sphere in RED color
      }
  }
  BBox nodeBox = treeNode->getBBox();
    BBox sphereBox = S->getBBox();
    // standard box-box intersection
    if (!BBOX_intersection(nodeBox, sphereBox))
      return false;
    // Now recurse through the child nodes
      for each childNode of treeNode
      {
        // For BVH and Octree: box-box collision
        if (DetectCollision_MidPhase(S, childNode))
        return true;
      }
        // no intersection with the scene
  return false;
}
*/

