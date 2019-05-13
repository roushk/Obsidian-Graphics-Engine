#include "BoundingSpherePCA.h"
#include "Object.h"

BoundingSpherePCA::BoundingSpherePCA(Object& obj): obj(obj)
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
  
  EigenSphere();
}

void BoundingSpherePCA::Update(float dt, glm::vec3 offset)
{
  oldOffset = offset;
}

void BoundingSpherePCA::Update(float dt, glm::mat4 transform)
{
}

glm::mat3 BoundingSpherePCA::CovarianceMatrix()
{
  float oon = 1.0f / (float)verts.size();
  glm::vec3 c{ 0.0f };
  float e00, e11, e22, e01, e02, e12;
  for(auto& point: verts)
  {
    c += point;
  }
  c *= oon;
  e00 = e11 = e22 = e01 = e02 = e12 = 0.0f;
  for (auto& point : verts)
  {
    glm::vec3 p = point - c;
    e00 += p.x * p.x;
    e11 += p.x * p.x;
    e22 += p.x * p.x;
    e01 += p.x * p.x;
    e02 += p.x * p.x;
    e12 += p.x * p.x;

  }
  glm::mat3 cov;
  cov[0][0] = e00 * oon;
  cov[1][1] = e11 * oon;
  cov[2][2] = e22 * oon;
  cov[0][1] = cov[1][0] = e01 * oon;
  cov[0][2] = cov[2][0] = e02 * oon;
  cov[1][2] = cov[2][1] = e12 * oon;

  return cov;
}

void BoundingSpherePCA::EigenSphere()
{
  glm::mat3 m, v;
  m = CovarianceMatrix();
  Jacobi(m, v);
  glm::vec3 e;
  int maxc = 0;
  float maxf = glm::abs(m[1][1]);
  float maxe = glm::abs(m[0][0]);
  
  if(maxf > maxe)
  {
    maxc = 1;
    maxe = maxf;
  }

  maxf = glm::abs(m[2][2]);
  if (maxf > maxe)
  {
    maxc = 2;
    maxe = maxf;
  }
  e[0] = v[0][maxc];
  e[1] = v[1][maxc];
  e[2] = v[2][maxc];

  int imin, imax;
  ExtremePointsAlongDirection(e, &imin, &imax);
  glm::vec3 minpt = verts[imin];
  glm::vec3 maxpt = verts[imax];

  float dist = sqrt(dot(maxpt - minpt, maxpt - minpt));

  center = (minpt + maxpt) * 0.5f;
  radius = dist * 0.5f;
}

void BoundingSpherePCA::Jacobi(glm::mat3& a, glm::mat3& v)
{
  int p, q;
  float prevoff = 0;
  float c, s;
  glm::mat3 J(1.0f);
  glm::mat3 b;
  glm::mat3 t;
  //init v to identity matrix
  v = glm::mat3(1.0f);

  const int MAX_ITERATIONS = 50;
  for (unsigned n = 0; n < MAX_ITERATIONS; ++n)
  {
    p = 0; q = 1;
    for(unsigned i = 0; i < 3; ++i)
    {
      for (unsigned j = 0; j < 3; ++j)
      {
        if (i == j)
          continue;
        if(glm::abs(a[i][j]) > glm::abs(a[p][q]))
        {
          p = i;
          q = j;
        }
      }
    }
    SymSchur(a, p, q, c, s);
    J[p][p] = c;
    J[p][q] = s;
    J[q][p] = -s;
    J[q][q] = c;
    v = v * J;

    a = (transpose(J) * a) * J;

    float off = 0.0f;
    for(unsigned i = 0; i < 3; ++i)
    {
      for (unsigned j = 0; j < 3; ++j)
      {
        if (i == j)
          continue;
        off += a[i][j] * a[i][j];
      }
    }
    if (n > 2 && off >= prevoff)
      return;
    prevoff = off;
  
  }
}

void BoundingSpherePCA::SymSchur(glm::mat3& a, int p, int q, float& c, float& s)
{
  if(abs(a[p][q]) > 0.0001f)
  {
    float r = (a[q][q] - a[p][p]) / (2.0f * a[p][q]);
    float t;
    if(r >= 0.0f)
    {
      t = 1.0f / (r + sqrt(1.0f + r * r));

    }
    else
    {
      t = -0.1 / (-r + sqrt(1.0f + r * r));
    }
    c = 1.0f / sqrt(1.0f + t * t);
    s = t * c;

  }else
  {
    c = 1.0f;
    s = 0.0f;
  }
}

void BoundingSpherePCA::ExtremePointsAlongDirection(glm::vec3 dir,  int* imin, int* imax)
{
  float minproj = FLT_MAX, maxproj = -FLT_MAX;
  for(unsigned i = 0; i < verts.size(); ++i)
  {
    float proj = dot(verts[i], dir);
    if(proj < minproj)
    {
      minproj = proj;
      *imin = i;
    }
    if(proj > maxproj)
    {
      maxproj = proj;
      *imax = i;
    }
  }
}
