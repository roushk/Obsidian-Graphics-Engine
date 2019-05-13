#include <pch.h>
#include "Object.h"
#include "singleton.h"
#include "loadfile.h"

Mesh::Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<Index>& indices,
           const std::vector<glm::vec2>& cyl, const std::vector<glm::vec2>& sphere,
           const std::vector<glm::vec2>& planar): name(name), vertices(vertices), indices(indices),
                                                  uvCylindrical(cyl), uvSpherical(sphere), uvPlanar(planar)
{
  uvSetting = MeshUVSetting::sphere;
  setup_mesh();
}

Mesh::Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<Index>& indices):name(name), vertices(vertices), indices(indices)
{
  uvSetting = MeshUVSetting::custom;
  setup_mesh();
}


VAO Mesh::get_vao() const
{
  return vao;
}

VBO Mesh::get_vbo() const
{
  return vbo;
}

IBO Mesh::get_ibo() const
{
  return ibo;
}

void Mesh::change_uv_coord_mapping(MeshUVSetting newSetting)
{
  uvSetting = newSetting;
  if (uvSetting == MeshUVSetting::custom)
    throw(
      "UV setting cannot be set to custom, has to have custom UV coordinates loaded in at starting. check the mesh to make sure all verts have UV coords"
    );
  for (unsigned i = 0; i < vertices.size(); ++i)
  {
    if (uvSetting == MeshUVSetting::cylinder)
      vertices[i].texCoords = uvCylindrical[i];
    if (uvSetting == MeshUVSetting::sphere)
      vertices[i].texCoords = uvSpherical[i];
    if (uvSetting == MeshUVSetting::planar)
      vertices[i].texCoords = uvPlanar[i];
  }

  //need to re bind tex coords
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof Vertex, vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof Index, indices.data(), GL_STATIC_DRAW);
  // tex coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof Vertex, reinterpret_cast<void*>(offsetof(Vertex, texCoords)));

  glBindVertexArray(0); // so other commands don't accidentally fuck up our vao
}

void Mesh::setup_mesh()
{
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ibo);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof Vertex, vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof Index, indices.data(), GL_STATIC_DRAW);
  // positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, nullptr);
  // normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, reinterpret_cast<void*>(offsetof(Vertex, normal)));
  // tex coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof Vertex, reinterpret_cast<void*>(offsetof(Vertex, texCoords)));

  glBindVertexArray(0); // so other commands don't accidentally fuck up our vao
}

Model ObjectReader::load_model(const std::string& path)
{
  Assimp::Importer importer;
  Model newModel{};
  const auto scene = importer.ReadFile(path.c_str(),
                                       aiProcessPreset_TargetRealtime_Quality | aiProcess_SortByPType |
                                       aiProcess_PreTransformVertices);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cout << "couldn't load model from path: " << path << std::endl;
  }
  process_node(scene->mRootNode, scene, newModel);
  //Process materials and generate UV coordinates

  return newModel;
}

void ObjectReader::process_node(aiNode* node, const aiScene* scene, Model& m)
{
  for (unsigned i = 0; i < node->mNumMeshes; ++i)
  {
    const auto mesh = scene->mMeshes[node->mMeshes[i]];
    m.meshData.emplace_back(process_mesh(mesh, scene));
    m.meshData.back().change_uv_coord_mapping(MeshUVSetting::planar);
  }
  for (unsigned i = 0; i < node->mNumChildren; ++i)
  {
    process_node(node->mChildren[i], scene, m);
  }
}

glm::vec2 ObjectReader::uv_calc(glm::vec3 point)
{
  float absX = abs(point.x);
  float absY = abs(point.y);
  float absZ = abs(point.z);

  bool XPositive = point.x > 0 ? true : false;
  bool YPositive = point.y > 0 ? true : false;
  bool ZPositive = point.z > 0 ? true : false;

  float largestLine = 0;
  glm::vec2 uv{0, 0};

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

Mesh ObjectReader::process_mesh(aiMesh* mesh, const aiScene* scene)
{
  std::string name(mesh->mName.C_Str());

  glm::vec3 maxSize(0, 0, 0);
  glm::vec3 minSize(0, 0, 0);
  //bool texCoordsExist = true;
  std::vector<Vertex> vertices;
  std::vector<Index> indices;

  for (unsigned i = 0; i < mesh->mNumVertices; ++i)
  {
    Vertex vert;
    vert.pos.x = mesh->mVertices[i].x;
    vert.pos.y = mesh->mVertices[i].y;
    vert.pos.z = mesh->mVertices[i].z;
    vert.normal.x = mesh->mNormals[i].x;
    vert.normal.y = mesh->mNormals[i].y;
    vert.normal.z = mesh->mNormals[i].z;

    if (vert.pos.x > maxSize.x)
      maxSize.x = vert.pos.x;
    if (vert.pos.y > maxSize.y)
      maxSize.y = vert.pos.y;
    if (vert.pos.z > maxSize.z)
      maxSize.z = vert.pos.z;

    if (vert.pos.x < minSize.x)
      minSize.x = vert.pos.x;
    if (vert.pos.y < minSize.y)
      minSize.y = vert.pos.y;
    if (vert.pos.z < minSize.z)
      minSize.z = vert.pos.z;

    vertices.push_back(vert);
  }

  for (unsigned i = 0; i < mesh->mNumFaces; ++i)
  {
    const auto face = mesh->mFaces[i];
    if (face.mNumIndices < 3)
      continue;
    indices.push_back(face.mIndices[0]);
    indices.push_back(face.mIndices[1]);
    indices.push_back(face.mIndices[2]);
  }


  glm::vec3 center = (maxSize + minSize) / 2.0f;
  float scale = abs(minSize.x - maxSize.x);

  glm::mat4 matrix(1.0f);
  matrix = glm::scale(matrix, glm::vec3(1.0f / scale));
  matrix = translate(matrix, glm::vec3(-center.x, -center.y, -center.z));

  std::vector<glm::vec2> uvCylindrical;
  std::vector<glm::vec2> uvSpherical;
  std::vector<glm::vec2> uvPlanar;

  float u, v;

  //Cylindrical 
  for (auto& preTransformVert : vertices)
  {
    glm::vec4 vert = matrix * glm::vec4(preTransformVert.pos, 1.0f);
    glm::vec4 newMin = matrix * glm::vec4(minSize, 1.0f);
    glm::vec4 newMax = matrix * glm::vec4(maxSize, 1.0f);

    //divide by 360 degrees   
    float theta = atan2(vert.z, vert.x); //if u is negative add 2PI
    u = (theta + PI) / (2.0f * PI);

    //y not z cause its y up not z up
    v = 1 - ((vert.y - newMin.y) / (newMax.y - newMin.y));
    uvCylindrical.push_back({u, v});
    if (u > 1.0f || u < 0.0f || v > 1.0f || v < 0.0f)
      std::cout << "u or v is out of range!" << std::endl;
  }

  //Spherical
  for (auto& preTransformVert : vertices)
  {
    glm::vec4 vert = matrix * glm::vec4(preTransformVert.pos, 1.0f);

    float theta = atan2(-vert.z, vert.x);
    u = (theta + PI) / (2.0f * PI);

    v = acosf(vert.y) / PI; // / r); r = 1

    uvSpherical.push_back({u, v});
    if (u > 1.0f || u < 0.0f || v > 1.0f || v < 0.0f)
      std::cout << "u or v is out of range!" << std::endl;
  }

  //Planar
  for (auto& preTransformVert : vertices)
  {
    glm::vec4 vert = matrix * glm::vec4(preTransformVert.pos, 1.0f);
    //0 = x, 1 = y, 2 = x
    glm::vec2 uv = uv_calc(vert);
    uvPlanar.push_back(uv);
  }
  return Mesh(name, vertices, indices, uvCylindrical, uvSpherical, uvPlanar);
}

Model ObjectReader::load(const std::string& filename) noexcept
{
  const auto md = load_model("assets/models/fbx/" + filename);
  Model info;

  glm::vec3 minSize(0, 0, 0);
  glm::vec3 maxSize(0, 0, 0);

  for (const auto& mesh : md.meshData)
  {
    /*
      So here is where we ran unto a WONDERFUL issue with the editor/sizing models where the walls would not easily line up
     this is fixed by in Maya creating a plane that represents the 1x1(possibly 1x1x1) unit square the mesh will be scaled down too
     this is to allow things such as uncentered walls and corners to line up properly. the debug is not added to the actual render but
     is used in the 1x1 calculation. We decided the easiest way is solving this with the DEBUG_PLANE. Think of it as a meter stick
     used to measure and scale walls properly. Its only needed for room walls and things that need pixel perfect matching aka walls,
     and things that are on the grid that need to match other things on the grid. - Coleman
   */

    if (mesh.name != std::string("DEBUG_PLANE"))
      info.meshes.emplace_back(std::make_pair(mesh.get_vao(), mesh.indices.size()));

    //generate model to world transform
    //gets the min and max sizes of each

    for (const auto& vert : mesh.vertices)
    {
      if (vert.pos.x > maxSize.x)
        maxSize.x = vert.pos.x;
      if (vert.pos.y > maxSize.y)
        maxSize.y = vert.pos.y;
      if (vert.pos.z > maxSize.z)
        maxSize.z = vert.pos.z;

      if (vert.pos.x < minSize.x)
        minSize.x = vert.pos.x;
      if (vert.pos.y < minSize.y)
        minSize.y = vert.pos.y;
      if (vert.pos.z < minSize.z)
        minSize.z = vert.pos.z;
    }
  }
  info.modelName = filename;
  glm::vec3 scale = abs(minSize - maxSize);

  info.halfExtents = scale / 2.0f;

  //scale the object by the largest axis
  float maxVal = std::max(scale.x, std::max(scale.y, scale.z));

  glm::vec3 center = (maxSize + minSize) / 2.0f;

  info.modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / maxVal));
  info.modelMatrix = translate(info.modelMatrix, glm::vec3(-center.x, -center.y, -center.z));

  return info;
}

void ObjectReader::loadMultiple(const std::string& filename)
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
    while (file.size() > findPos + i + 1 && file[findPos + i] != '\n')
    {
      ++i;
    }

    //push back filename
    //only push back file if its greater than size of 3 (for suffix *.obj)
    if (i > 4)
      objects.push_back(file.substr(findPos, i));
    file.erase(findPos, i);

    findPos = file.find_first_of('\n');
    if (findPos != std::string::npos)
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
  for (auto& objName : objects)
  {
    load(objName);
  }
  

}
