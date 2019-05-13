/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: Object.cpp
Purpose: Implementation file for the object class
Language: C++ MSVC
Platform: VS 141, OpenGL 4.3 compatabile device driver, Win10
Project: coleman.jonas_CS350_1
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 5/18/18
End Header --------------------------------------------------------*/
#include <vector>
#include "Object.h"
#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <map>
#include <algorithm>
#include <chrono>
#include <sstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#define MAXLINELENGTH 10000
#define PI 3.14159265359f

bool ObjectReader::ReadObject(std::string filename)
{
  auto start = std::chrono::system_clock::now();
  vec3 maxSize(0, 0, 0);
  vec3 minSize(0, 0, 0);

  std::vector<vec3> tempVerts;

  std::vector<uvec3> tempFaces;

  //vert tempNormals using tempVerts pos in tempVerts vec as positon
  std::vector<vec3> tempvertexNormals;

  //face tempNormals using facep Pos in tempFaces vec as positon 
  std::vector<vec3> tempFaceNormals;

  std::cout << "Reading File: " << filename << std::endl;

  Object object;
  char buffer[MAXLINELENGTH];
  
  
  std::ifstream file;
  std::ofstream fileOut;

  file.open(filename);

  if(file)  
    file >> buffer;

  while (file)
  {
    
    
    //if first character is comment then ignore
    if (buffer[0] == '#'    //for comment
      || (buffer[0] == 'v' && buffer[1] == 'n')   //for vn
      || (buffer[0] == 'v' && buffer[1] == 't')   //for vt
      || buffer[0] == 'g'   //for group
      || buffer[0] == 'm'   //for mtllib
      || buffer[0] == 'o'   //for object
      || buffer[0] == 'u')  //for usemlt
    {
      file.ignore(MAXLINELENGTH, '\n');
      file >> buffer;
    }
    else if (buffer[0] == 'v')
    {
      file >> buffer;

      vec3 newVec;
      //strip number//samenumber???
      //substring of value and rest of string till next number
      std::string parse = buffer;
      newVec.x = atof(parse.c_str());
      file >> buffer;

      parse = buffer;
      newVec.y = atof(parse.c_str());
      file >> buffer;

      parse = buffer;
      newVec.z = atof(parse.c_str());

      tempVerts.push_back(newVec);
      file >> buffer;
    }

    //f = tempFaces (can be more than 3 in a row
    else if (buffer[0] == 'f')
    {
      std::deque<int> tempFacesDeque;
      file >> buffer;

      while((buffer[0] >= '0' && buffer[0] <= '9')|| buffer[0] == '.' && file)
      {
        
        std::string parse = buffer;
        tempFacesDeque.push_back(atoi(parse.c_str()) - 1);
        file >> buffer;
      }

      vec3 newVec;
      newVec.x = tempFacesDeque[0];
      newVec.y = tempFacesDeque[1];
      newVec.z = tempFacesDeque[2];

      //for 3+ tempFaces

      tempFaces.push_back(newVec);
      for(unsigned i = 0; i + 3 < tempFacesDeque.size(); ++i)
      {
        newVec.x = tempFacesDeque[0];
        newVec.y = tempFacesDeque[i + 2];
        newVec.z = tempFacesDeque[i + 3];
        tempFaces.push_back(newVec);
      }
    }
    else
      file >> buffer;
  }
  
  
  //reserve the data so that at that position is guarenteed data
  
  //calculate face tempNormals
  //for all VERTS generate normal
  //for all tempFaces generate normal
  for (unsigned i = 0; i < tempFaces.size(); ++i)
  {
    vec3 face = tempFaces.at(i);

    vec3 U = tempVerts[face.y] - tempVerts[face.x];
    vec3 V = tempVerts[face.z] - tempVerts[face.x];
    vec3 N = cross(U, V);

    tempFaceNormals.push_back(normalize(N));
    
  }
  std::vector<vec3> temp;
  std::vector<std::vector<vec3>> tempVecVecNormals(tempVerts.size(), temp);

  for (unsigned i = 0; i < tempFaces.size(); ++i)
  {
    //add all the tempFaces to the vert list
    //inserts vec3 normal at tempVecNormal of each vert in each face
    tempVecVecNormals[tempFaces.at(i).x].push_back(tempFaceNormals.at(i));
    tempVecVecNormals[tempFaces.at(i).y].push_back(tempFaceNormals.at(i));
    tempVecVecNormals[tempFaces.at(i).z].push_back(tempFaceNormals.at(i));
  }

  //remove unique values in tempVectorNormal list
  //make sure all face tempNormals are unique
  //have to instanciate to use operator() in std::sort
  vec3Operator vec3op;// (vec3(99, 99, 99));
  for(auto & it : tempVecVecNormals)
  {
    if(it.size() > 2)
    {
      std::sort(it.begin(), it.end(), vec3op);
    }

    if(it.size() > 1)
    {
      it.erase(std::unique(it.begin(), it.end()), it.end());
    }
  }
  
  //calculate avg of all tempNormals for each vert
  for(std::vector<vec3> & vert : tempVecVecNormals)
  {
    vec3 vec(0, 0, 0);
    for(vec3& normal: vert)
    {
      vec += normal;
    }
    
    //push back vector normal after its normalized to object
    tempvertexNormals.push_back(normalize(vec));
  }
  //push all data into GLfloat,GLuint datatypes
  //creates bounding box with min and max size of object
  for(auto& vert: tempVerts)
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

    object.verts.push_back(vert);
  }

  object.faces = tempFaces;
  object.vertexNormals = tempvertexNormals;
  object.faceNormals = tempFaceNormals;

  
  object.scale = minSize.x - maxSize.x;
  object.min = minSize;
  object.max = maxSize;

  unsigned end = filename.find_last_of(".obj");
  end -= filename.find_last_of('/') + 4;
  object.name = filename.substr(filename.find_last_of('/') + 1, end);
  
  object.center = (maxSize + minSize) / 2.0f;

  object.scale = abs(object.scale);



  //*********************************************************************//
  //generate UV

  //transform object to center of sphere/cylinder
  mat4 matrix(1.0f);
  matrix = scale(matrix, vec3(1.0f / object.scale));
  matrix = translate(matrix, vec3(-object.center.x, -object.center.y, -object.center.z));

  //Cylindrical 
  for (auto & preTransformVert : tempVerts)
  {
    float u, v;

    vec4 vert = matrix * vec4(preTransformVert, 1.0f);
    vec4 newMin = matrix * vec4(minSize, 1.0f);
    vec4 newMax = matrix * vec4(maxSize, 1.0f);

    //divide by 360 degrees   
    float theta = atan2(vert.z, vert.x);  //if u is negative add 2PI
    u = (theta + PI) / (2.0f * PI);
    
    //y not z cause its y up not z up
    v = 1 - ((vert.y - newMin.y) / (newMax.y - newMin.y));
    object.uvCylindrical.push_back({ u,v });
    if (u > 1.0f || u < 0.0f || v > 1.0f || v < 0.0f)
      std::cout << "u or v is out of range!" << std::endl;
  }

  //Spherical
  for (auto & preTransformVert : tempVerts)
  {
    float u, v;

    vec4 vert = matrix * vec4(preTransformVert, 1.0f);

    float theta = atan2(-vert.z, vert.x);  
    u = (theta + PI) / (2.0f * PI);

    v = acosf(vert.y) / PI; // / r); r = 1

    object.uvSpherical.push_back({ u,v });
    if (u > 1.0f || u < 0.0f || v > 1.0f || v < 0.0f)
      std::cout << "u or v is out of range!" << std::endl;
  }

  //Planar
  for (unsigned i = 0; i < tempVerts.size(); ++i)
  {
    //centeres object
    vec4 vert = matrix * vec4(tempVerts[i], 1.0f);
    float u, v;
    //0 = x, 1 = y, 2 = x
    vec2 uv = UVCalc(vert);

    object.uvPlanar.push_back(uv);

  }
  object.modelMatrix = translate(scale(glm::mat4(1.0f), vec3(1.0f / object.scale)), vec3(-object.center.x, -object.center.y, -object.center.z));
  
  object.setup_mesh();
  objects.push_back(object);
  //read object
  auto endTime = std::chrono::system_clock::now();
  auto chronoDt = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - start);
  object.timeToRead = chronoDt.count();

  std::cout << "Time to Read Object File: " << filename << " Took: " << object.timeToRead << " Milliseconds" << std::endl;

  
  return true;
}

void Object::setup_mesh()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(4, VBO);
  glGenBuffers(1, &IBO);
  glBindVertexArray(VAO);

  //faces
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof glm::uvec3, faces.data(), GL_STATIC_DRAW);
  //2
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  //verts
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof glm::vec3, verts.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  
  // vert normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
  glBufferData(GL_ARRAY_BUFFER, vertexNormals.size() * sizeof glm::vec3, vertexNormals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  // uvs
  glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
  glBufferData(GL_ARRAY_BUFFER, uvCylindrical.size() * sizeof glm::vec2, uvCylindrical.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  
  //face normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
  glBufferData(GL_ARRAY_BUFFER, faceNormals.size() * sizeof glm::vec3, faceNormals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindVertexArray(0); // so other commands don't accidentally fuck up our vao
}

Object& ObjectReader::GetObject(int location)
{
  return objects[location];
}

void ObjectReader::ReadFile(std::string filename)
{
  std::string file = load_file(filename);

  //append new line to end of file
  file.push_back('\n');
  std::vector < std::string> objects;

  //is -1 because the first filename does not have a 
  size_t findPos = 0;
  size_t lastPos = file.find_last_of('\n');
  do
  {
    unsigned i = 1;

    //while still filename
    while(file.size() > findPos + i + 1 && file[findPos + i] != '\n')
    {
      ++i;
    }

    //push back filename
    //only push back file if its greater than size of 3 (for suffix *.obj)
    if(i > 4)
      objects.push_back(file.substr(findPos, i));
    file.erase(findPos, i);

    findPos = file.find_first_of('\n');
    if(findPos != std::string::npos)
    {
      file.erase(findPos, 1);
    }
    else
    {
      break;
    }

    //if (file.size() == 0 || findPos == std::string::npos)
     // break;
  } while (findPos != std::string::npos && findPos != lastPos);

  //load objects into loader
  for(auto& objName: objects)
  {
    ReadObject(objName);
  }
}
