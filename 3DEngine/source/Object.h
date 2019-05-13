/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: Object.h
Purpose: header for custom object class to hold the model info
Language: C++ MSVC
Platform: VS 141, OpenGL 4.3 compatabile device driver, Win10
Project: coleman.jonas_CS350_1
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 5/18/18
End Header --------------------------------------------------------*/

#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>
#include <set>
#include <fstream>

using namespace glm;

struct Object
{
  std::vector<vec3> verts;
  std::vector<vec2> uvCylindrical;
  std::vector<vec2> uvSpherical;
  std::vector<vec2> uvPlanar;
  glm::mat4 modelMatrix;

  
  //0 = up, 1 = down, 2 = left, 3 = right, 4 = forward, 5 = backward
  std::vector<uvec3> faces;
  
  //vert normals using verts pos in verts vec as positon
  std::vector<vec3> vertexNormals;

  //face normals using facep Pos in faces vec as positon 
  std::vector<vec3> faceNormals;
  
  glm::vec3 min, max;

  float timeToRead;
  float scale;
  vec3 center;
  std::string name;

  vec3 emissiveColor = vec3(0,0,0);
  float Kambient, Kdiffuse, Kspecular, ns;

  GLuint VAO; //entire thing
  GLuint IBO; //faces
  GLuint VBO[4];
  //Positions
  //VertNormals, 
  //UVcoords;
  //FaceNormals, 

  void setup_mesh();
};

class ObjectReader
{
public:
  bool ReadObject(std::string filename);
  Object& GetObject(int location);
  unsigned MaxObjects() { return objects.size(); };
  void ReadFile(std::string filename);
private:

  std::vector<Object> objects;
};

struct vec3Operator
{
  //float x, y, z;
  //vec3Operator(const vec3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {};

  bool operator()(const vec3& lhs, const vec3& rhs) const
  {
    if ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.x < rhs.x))
      return true;
    if ((lhs.x == rhs.x) && (lhs.y < rhs.y))
      return true;
    if ((lhs.x < rhs.x))
      return true;
    return false;
  }
};

inline vec2 UVCalc(vec3 point)
{
  float absX = abs(point.x);
  float absY = abs(point.y);
  float absZ = abs(point.z);

  bool XPositive = point.x > 0 ? true : false;
  bool YPositive = point.y > 0 ? true : false;
  bool ZPositive = point.z > 0 ? true : false;

  float largestLine = 0;
  vec2 uv;

  //posx, negx, posy, negy, posz, negz
  if (XPositive && absX >= absY && absX >= absZ) 
  {

    largestLine = absX;
    uv.x = -point.z;
    uv.y = point.y;
  }
  if (XPositive == false && absX >= absY && absX >= absZ) 
  {

    largestLine = absX;
    uv.x = point.z;
    uv.y = point.y;
  }
  if (YPositive && absY >= absX && absY >= absZ) 
  {

    largestLine = absY;
    uv.x = point.x;
    uv.y = -point.z;
  }
  if (YPositive == false && absY >= absX && absY >= absZ) 
  {

    largestLine = absY;
    uv.x = point.x;
    uv.y = point.z;
  }
  if (ZPositive && absZ >= absX && absZ >= absY) 
  {

    largestLine = absZ;
    uv.x = point.x;
    uv.y = point.y;
  }
  if (ZPositive == false && absZ >= absX && absZ >= absY) 
  {

    largestLine = absZ;
    uv.x = -point.x;
    uv.y = point.y;
  }

  uv.x = 0.5f * (uv.x / largestLine + 1.0f);
  uv.y = 0.5f * (uv.y / largestLine + 1.0f);
  return uv;
}

inline std::string load_file(const std::string& filename)
{
  std::ifstream file(filename);


  std::string data;

  file.seekg(0, std::ios::end);
  data.reserve(static_cast<const unsigned>(file.tellg()));  //allocated all memory for it
  file.seekg(0, std::ios::beg);

  data.assign(std::istreambuf_iterator<char>(file),
    std::istreambuf_iterator<char>());

  return data;
}

#endif
