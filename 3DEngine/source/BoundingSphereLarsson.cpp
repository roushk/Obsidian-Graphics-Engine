#include "BoundingSphereLarsson.h"
#include "BoundingVolume.h"

#include "Object.h"

BoundingSpherelarsson::BoundingSpherelarsson(Object& _obj) : obj(_obj)
{
  center = obj.modelMatrix * glm::vec4(obj.center, 1);  //want BV in world space
  //min = (obj.modelMatrix * glm::vec4(obj.min, 1));
  //max = (obj.modelMatrix * glm::vec4(obj.max, 1));

  radius = distance(center, max);

  for (auto& face : obj.faces)
  {
    //from tri a,b,c to line segments a,b a,c b,c
    faces.push_back({ face.x, face.y });
    faces.push_back({ face.x, face.z });
    faces.push_back({ face.y, face.z });
  }
  normals = obj.vertexNormals;

  //verts in world space
  for (auto& vert : obj.verts)
  {
    glm::vec3 postVert = obj.modelMatrix * glm::vec4(vert, 1);
    verts.push_back(postVert);
  }

  //set current EPOS mode
  std::vector<glm::vec3> eposVec;
  if(currentEPOS == EPOS::EPOS6)
  {
    eposVec = EPOS6;
  }
  else if (currentEPOS == EPOS::EPOS14)
  {
    eposVec = EPOS14;
  }
  else if (currentEPOS == EPOS::EPOS26)
  {
    eposVec = EPOS26;
  }
  else if (currentEPOS == EPOS::EPOS98)
  {
    eposVec = EPOS98;
  }

  //for each value find normal
  
  std::vector<glm::vec3> extremalPoints;
  //find min and max value 
  for(auto& EPOS: eposVec)
  {
    float minVal = std::numeric_limits<float>::max();
    float maxVal = std::numeric_limits<float>::min();
    for (unsigned i = 0; i < verts.size(); ++i)
    {

      if(dot(EPOS, normals[i]) < minVal)
      {
        min = verts[i];
        minVal = dot(EPOS, normals[i]);
      }
      if (dot(EPOS, normals[i]) > maxVal)
      {
        max = verts[i];
        maxVal = dot(EPOS, normals[i]);
      }
    }
    //push back points onto vec
    extremalPoints.push_back(min);
    extremalPoints.push_back(max);
  }



}

void BoundingSpherelarsson::Update(float dt, glm::mat4 transform)
{

}

void BoundingSpherelarsson::Update(float dt, glm::vec3 offset)
{
  if (offset != oldOffset)
  {

    for (auto& vert : verts)
    {
      vert -= oldOffset;
      vert += offset;
    }
    oldOffset = offset;
  }
}

void BoundingSpherelarsson::SphereOfSphereAndPt(Sphere& sphere, glm::vec3& point)
{
  glm::vec3 d = point - sphere.center;
  float dist2 = dot(d, d);
  if (dist2 > sphere.radius * sphere.radius)
  {
    float dist = sqrt(dist2);
    float newRadius = (sphere.radius + dist) * 0.5f;
    float k = (newRadius - sphere.radius) / dist;

    sphere.radius = newRadius;
    sphere.center += d * k;
  }
}

Sphere BoundingSpherelarsson::Gartner(std::vector<glm::vec3> extremalPoints)
{
  Sphere mb(glm::vec3{0,0,0},1);
  return mb;
}

//extremal points in verts point cloud
std::vector<glm::vec3> BoundingSpherelarsson::FindExtremalPoints()
{
  //N can be 2,3,4
  return {};
}
//create minimum sphere from given points
Sphere BoundingSpherelarsson::MinimumSphere(std::vector<glm::vec3>& E)
{
  //centroid of finite set of points
  glm::vec3 maxPoint{0};
  float maxDistance = 0;

  for(auto& point: E)
  {
    center += point;

    float distance = glm::distance(center, point);
    if(distance > maxDistance)
    {
      maxDistance = distance;
      maxPoint = point;
    }
  }
  center /= E.size();
  radius = distance(center, maxPoint);

  return { center, radius };
}

/*
  First, the extremal points are just guesses as to the farthest apart points.
  He calculates them using an arbitrary set of normals for minimal calculations.
  The dot product of a point and a normal gives a relative "length" along the normal.
  For each test normal you get 2 extremal points, the min and the max. 
  Thus you should get a number of extremal points = to 2 times the number normals used, 
  barring duplicate points. You then feed those points into Gartner's algorithm.

  Gartner (paper on moodle)

  EPOS 6 is the first 3
  EPOS 14 is the first 7
  EPOS 26 : first 13
  EPOS 98 : all 49

  { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }

  { 1, 1, 1 }, { 1, 1, -1 }, { 1, -1, 1 }, { 1, -1, -1 }

  { 1, 1, 0 }, { 1, -1, 0 }, { 1, 0, 1 }, { 1, 0, -1 }, { 0, 1, 1 }, { 0, 1, -1 }

  { 1, 2, 0 }, { 1, -2, 0 }, { 1, 0, 2 }, { 1, 0, -2 }, { 0, 1, 2 }, { 0, 1, -2 }, { 2, 1, 0 }, { 2, -1, 0 }, { 2, 0, 1 }, { 2, 0, -1 }, { 0, 2, 1 }, { 0, 2, -1 }

  { 1, 1, 2 }, { 1, 2, 1 }, { 2, 1, 1 }, { 1, -1, 2 }, { 1, -2, 1 }, { 2, -1, 1 }, { 1, 1, -2 }, { 1, 2, -1 }, { 2, 1, -1 }, { 1, -1, -2 }, { 1, -2, -1 }, { 2, -1, -1 }

  { 1, 2, 2 }, { 2, 1, 2 }, { 2, 2, 1 }, { 1, -2, 2 }, { 2, -1, 2 }, { 2, -2, 1 }, { 1, 2, -2 }, { 2, 1, -2 }, { 2, 2, -1 }, { 1, -2, -2 }, { 2, -1, -2 }, { 2, -2, -1 }
 */