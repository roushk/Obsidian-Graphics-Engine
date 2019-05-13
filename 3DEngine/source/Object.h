#ifndef OBJECT_H
#define OBJECT_H
#define PI 3.14159265359f
#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>


using VBO = GLuint;
using VAO = GLuint;
using IBO = GLuint;

struct Vertex
{
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

using Index = unsigned;

struct Texture
{
  std::string textureName = "DEFAULT_TEXTURE_NAME";
  GLuint texture = -1;
};

enum class MeshUVSetting
{
  cylinder,
  sphere,
  planar,
  custom
};

class Mesh
{
public:
  Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<Index>& indices,
       const std::vector<glm::vec2>& cyl, const std::vector<glm::vec2>& sphere, const std::vector<glm::vec2>& planar);

  Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<Index>& indices);

  VAO get_vao() const;
  VBO get_vbo() const;
  IBO get_ibo() const;

  std::string name;
  std::vector<Vertex> vertices;
  std::vector<Index> indices;

  //so can swap between UV coords
  MeshUVSetting uvSetting = MeshUVSetting::planar;

  //if uvSetting is custom then no coords in these
  std::vector<glm::vec2> uvCylindrical;
  std::vector<glm::vec2> uvSpherical;
  std::vector<glm::vec2> uvPlanar;

  // TODO: Cleanup meshes on unload
  void change_uv_coord_mapping(MeshUVSetting newSetting);

  glm::mat4 meshMatrix = glm::mat4(1.0f);
private:

  VAO vao = -1;
  VBO vbo = -1;
  IBO ibo = -1;


  void setup_mesh();
};


struct Model
{
  std::string modelName;
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  std::vector<std::pair<VAO, std::size_t>> meshes;
  std::vector<Mesh> meshData;
  glm::vec3 halfExtents;
};

class ObjectReader
{
  Model load_model(const std::string& path);

  void process_node(aiNode* node, const aiScene* scene, Model& m);

  glm::vec2 uv_calc(glm::vec3 point);

  Mesh process_mesh(aiMesh* mesh, const aiScene* scene);

public:
  Model load(const std::string& filename) noexcept;
  void loadMultiple(const std::string& filename);
};



#endif
