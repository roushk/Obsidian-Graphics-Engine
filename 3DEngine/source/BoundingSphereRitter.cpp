#include "BoundingSphereRitter.h"
#include "Object.h"

BoundingSphereRitter::BoundingSphereRitter(Object& _obj) : obj(_obj)
{

  center = obj.modelMatrix * glm::vec4(obj.center, 1);  //want BV in world space
  min = (obj.modelMatrix * glm::vec4(obj.min, 1));
  max = (obj.modelMatrix * glm::vec4(obj.max, 1));

  radius = distance(center, max);

  for (auto& face : obj.faces)
  {
    //from tri a,b,c to line segments a,b a,c b,c
    faces.push_back({ face.x, face.y });
    faces.push_back({ face.x, face.z });
    faces.push_back({ face.y, face.z });
  }

  for (auto& vert : obj.verts)
  {
    glm::vec3 postVert = obj.modelMatrix * glm::vec4(vert, 1);
    verts.push_back(postVert);
  }
  
  SphereFromDistPoints();
  for(auto& vert: verts)
  {
    SphereOfSphereAndPt(vert);
  }
  
}

BoundingSphereRitter::BoundingSphereRitter(const BoundingSphereRitter& rhs) : obj(rhs.obj)
{
  *this = rhs;
}

BoundingSphereRitter& BoundingSphereRitter::operator=(const BoundingSphereRitter& rhs)
{
  center = rhs.center;
  min = rhs.min;
  max = rhs.max;
  radius = rhs.radius;
  verts = rhs.verts;
  faces = rhs.faces;
  oldOffset = rhs.oldOffset;
  obj = rhs.obj;
  return *this;
}

void BoundingSphereRitter::Update(float dt, glm::mat4 transform)
{
}

void BoundingSphereRitter::Update(float dt, glm::vec3 offset)
{
  oldOffset = offset;
}

void BoundingSphereRitter::MostSeparatedPoints(int& min, int& max)
{
  int minx = 0;
  int maxx = 0;

  int miny = 0;
  int maxy = 0;

  int minz = 0;
  int maxz = 0;

  for(unsigned i = 0; i < verts.size(); ++i)
  {
    if (verts[i].x < verts[minx].x)
      minx = i;
    if (verts[i].x > verts[maxx].x)
      maxx = i;

    if (verts[i].y < verts[miny].y)
      miny = i;
    if (verts[i].y > verts[maxy].y)
      maxy = i;

    if (verts[i].z < verts[minz].x)
      minz = i;
    if (verts[i].z > verts[maxz].z)
      maxz = i;
  }

  float dist2x = dot(verts[maxx] - verts[minx], verts[maxx] - verts[minx]);
  float dist2y = dot(verts[maxy] - verts[miny], verts[maxy] - verts[miny]);
  float dist2z = dot(verts[maxz] - verts[minz], verts[maxz] - verts[minz]);

  min = minx;
  max = maxx;
  if(dist2y > dist2x && dist2y > dist2z)
  {
    max = maxy;
    min = miny;
  }
  if(dist2z > dist2x && dist2z > dist2y)
  {
    max = maxz;
    min = minz;
  }
}

void BoundingSphereRitter::SphereFromDistPoints()
{
  int min = 0;
  int max = 0;

  MostSeparatedPoints(min, max);

  center = (verts[min] + verts[max]) * 0.5f;
  radius = sqrt(dot(verts[max] - center, verts[max] - center));
}

void BoundingSphereRitter::SphereOfSphereAndPt(glm::vec3& point)
{
  glm::vec3 d = point - center;
  float dist2 = dot(d, d);
  if(dist2 > radius * radius)
  {
    float dist = sqrt(dist2);
    float newRadius = (radius + dist) * 0.5f;
    float k = (newRadius - radius) / dist;

    radius = newRadius;
    center += d * k;
  }
}
