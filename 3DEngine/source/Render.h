
#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Object.h"
#include "Light.h"
#include "Material.h"
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "SDL2/SDL_bits.h"
#include "SDL2/SDL.h"


#define PI 3.14159265359f
#define SQRT2OVER2 0.70710678118

#ifndef RENDER_H
#define RENDER_H
// Include GLEW
//#define _CRT_SECURE_NO_WARNINGS

struct BSPNode;
struct OctreeNode;
class Wireframe;
//#include "stbi_image_write.h"
using namespace glm;


inline void GetError()
{
  GLenum err;
  ///////////////////////////////////////////////////
  while ((err = glGetError()) != GL_NO_ERROR)
  {
    std::cout << "There was an Error: " << err << std::endl;
  }
  ///////////////////////////////////////////////////
}


enum shaderSetting
{
  ssReflectionMap,
  ssRefractionMap,
  ssFrenselApprox,
  ssPhongShadingAndFrensel,
  ssPhongShadingAndReflection,
  ssPhongShading,
  ssPhongLighting,
  ssBlinnLighting,
  ssWireframe,
  ssShadowShader,
  ssDeferredGeometry,
  ssDeferredRendering,
  ssPhongShadingDeferred,
  ssPhongShadingDeferredShadow,
  ssPhongShadingDeferredLightSphere,

  ssComputeBlurHorizontal,
  ssComputeBlurVertical,
  ssPhongShadingDeferredShadowMSM,
  ssBRDDeferredMSM,
  ssLightShader,
  ssSkyboxShader,
  ssSkydome,
  ssMaxShaders
};

enum renderSetting
{
  rsNone,
  rsFaceNormal,
  rsVertNormal
};

enum BindingPoint //UBO
{
  bpGlobal,
  bpLights,
  bpShadowblur,
  bpHammersley
};

enum UVModel
{
  uvmCylindrical,
  uvmSphereical,
  uvm6Planar
};


static const GLfloat g_vertex_buffer_data[] = {
  0.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
  1.0f, 0.0f, 0.0f,
  1.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
};

static const GLfloat g_vertex_normal_data[] = {
  0.5f, 0.0f, 0.5f,
  0.5f, 0.0f, 0.5f,
  0.5f, 0.0f, 0.5f,
  0.5f, 0.0f, 0.5f,
  0.5f, 0.0f, 0.5f,
  0.5f, 0.0f, 0.5f,
};

static const GLushort g_vertex_indices[] = {
  0, 1, 2,
  1, 4, 2,
};

enum MaterialType
{
  mtDiffuse,
  mtSpecular
};

class Render
{
public:
  Render();
  ~Render();

  std::string GetSetting();

  void SetModelOffset(float x, float y, float z, float scale_ = 1.0f);
  void SetModelOffset(vec4 pos, float scale_ = 1.0f);
  void SetModelOffset(const Light& light);
  void SetModelOffsetView(const Light& light);

  void SetModelOffset(vec3 pos, float scale_ = 1.0f);
  void LoadMaterial(Material materialSpec, Material materialDiff);
  void LoadSkybox(std::vector<std::string>& skyboxNames);

  void LoadSkydome();
  void BindSkydome();
  //Gbuffers
  void GenGBuffer();  //also gens shadow buffer
  void BindGBufferTextures();
  void BindAndCreateGBuffers();
  void BindGBuffer();

  //Frame Buffers
  void GenFrameBuffers();
  void BindAndCreateFrameBuffers(int i);
  void BindFrameBufferToRenderTo(unsigned i);
  void BindFrameBufferTextures();

  //shadow FBO stuff
  void BindShadowTextures();
  void BindAndCreateShadowBuffers();
  void BindShadowBuffer();

  //blur shadow FBO stuff
  void BindAndCreateBlurShadowBuffers();
  void BlurShadowLoadData();
  void BlurShadowLoadHorizontal();
  void BlurShadowLoadVertical();
  void BlurShadowLoadDebug(); //debug is for displaying the map in debug render
  void CreateBlurShadowData();
  void BlurShadowLoadFinalMap();


  //void BindBlurShadowTextures();
  //void BindBlurShadowBuffer();

  void BindDefaultFrameBuffer();
  void BindSkybox();
  void BindMaterials(SceneLighting& lighting);
  void LoadModel(Model& object); // Object object); 
  void LoadModel(Wireframe& object);
  
  void ClearScreen();
  void CreateShaders();

  void CreateBuffers();

  void LoadDiffuseForLight(Light& light, float scale = 1.0f);
  void LoadDiffuseForLight(glm::vec4& light, float scale = 1.0f);

  void LoadRoughness(float roughness);
  void SetCurrentShader(int shader);

  //loads data for every light as well as data for the global scene
  void BindLightScene(SceneLighting& lighting);

  void CreateUBOBufferObjects();

  void CleanUpUBOBuffers(int activeLights);
  void SetCurrentDebugTexture(unsigned debugTextureNext);

  bool InitRender();

  void CopyDepthBuffer();
  void BufferRefractionData();
  
  void LoadScreenSize();
  void Update();
  void BindModelBuffer();
  void UpdateCamera(float dt);
  void Draw(Model& object);
  void Draw(Wireframe& object);

  void EndDrawing();
  void ClearColor(vec4 color);
  void SetTitle(std::string object);
  void DrawShadow(const Model& object, const Light& light);


  GLuint LoadHDRimage(std::string filename, bool irr = false);
  /*
  void TakePicOfFBO(int i)
  {
    GLenum format = GL_RGB;
    unsigned * data = new unsigned((height * aspect) * height * 3 * sizeof(unsigned));

    glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, data);
    std::string filename;
    filename = "FBO_" + i;
    stbi_write_png(filename.c_str(), height*aspect, height, 3, data, height*aspect * 3 * sizeof(unsigned));
  }

  GLbyte * data;
  */
  bool flipX = false;
  bool cameraChanged = false;
  mat4 inverseCamRotate;
  
  SDL_Window* gWindow;
  SDL_GLContext gContext;
  bool creationSuccess;
  int height = 1;

  //width/height, width = aspect * height
  float aspect = 1;

  float nearPlane = 1.0f;
  float farPlane = 120.0f;


  GLuint skyboxTextureID[6];  //texture ID's for the skybox
  GLuint specularMaterialID;
  GLuint diffuseMaterialID;

  //skydome
  GLuint skydomeID[13];
  GLuint skydomeIDIRR[13];


  int currentUVModel = 0;
  unsigned setting = rsNone;

  void resize(int w, int h);

  void AddLight(Light light);
  void ClearLights();
  void SetObjPositionForCamera(vec3 pos);

  void SetCurrentCamera(int cam);

  void SetObjectShader(int shader);
  
  void LoadObjectShader();
  void HammersleyCreateData();

  void HammersleyLoadData();
  void LoadMaxDepth();

  //initial aspect is 1024.0f / 768.0f
  Camera currentCamera;
  Camera cameraBase = Camera(vec4{ 0, 0, 5, 0 }, vec4{ 0, 0, -1, 0 }, vec4{ 0,1,0,0 }, PI / 2.0f , 1024.0f / 768.0f, nearPlane, farPlane);
  Camera cameras[6]
  {
    //same order as textures posx, negx, posy, negy, posz, negz
    //view vec os other dir b/c is lookAt
    Camera(vec4{ 0, 0, 0, 0 }, vec4{  1, 0, 0, 1 }, vec4{ 0, 1,0,1 }, glm::radians(90.0f), 1.0f, nearPlane, farPlane),
    Camera(vec4{ 0, 0, 0, 0 }, vec4{ -1, 0, 0, 1 }, vec4{ 0, 1,0,1 }, glm::radians(90.0f), 1.0f, nearPlane, farPlane),
    Camera(vec4{ 0, 0, 0, 0 }, vec4{ 0,  1, 0, 1 }, vec4{ 0,0, 1,1 }, glm::radians(90.0f), 1.0f, nearPlane, farPlane),
    Camera(vec4{ 0, 0, 0, 0 }, vec4{ 0, -1, 0, 1 }, vec4{ 0,0,-1,1 }, glm::radians(90.0f), 1.0f, nearPlane, farPlane),
    Camera(vec4{ 0, 0, 0, 0 }, vec4{ 0, 0,  1, 1 }, vec4{ 0, 1,0,1 }, glm::radians(90.0f), 1.0f, nearPlane, farPlane),
    Camera(vec4{ 0, 0, 0, 0 }, vec4{ 0, 0, -1, 1 }, vec4{ 0, 1,0,1 }, glm::radians(90.0f), 1.0f, nearPlane, farPlane)
  };

  //same order as textures posx, negx, posy, negy, posz, negz
  //view vec os other dir b/c is lookAt
  GLuint FrameBuffers[6];

  GLuint RenderedTextures[6];

  GLuint FBODepthBuffers[6];

  GLuint shadowFBO[1];  //shadow map output FBO
  GLuint shadowTexture[1];  //depth map
  GLenum shadowBuffers[1] { GL_COLOR_ATTACHMENT0 };

  //shadow map -> blurShadowTexture[0] -> blurShadowTexture[1]
  //none -> horizontal -> horizontal and vertical
  //GLuint blurShadowFBO[1];  //shadow map output FBO
  GLuint blurShadowTexture[2];  //depth map
  GLenum blurShadowBuffers[1]{ GL_COLOR_ATTACHMENT0 };
  GLuint shadowBlurUBOHandle[1];


  GLuint HammersleyUBOHandle[1];
  static const int HammersleyConst = 20;  //samples count

  struct HammersleyBlock {
    float N = HammersleyConst;
    float hammersley[2 * HammersleyConst];
  };
  HammersleyBlock hammersleyBlock;


  static const int blurValue = 20;
  float weights[blurValue * 2 + 1];

  float max_depth = 10.0f;
  float scalarLevel = 1.0f;

  //dont need color buffer only depth buffer
  //GLuint shadowRBO[1]; //shadow render buffer object
  float shadowScale = 2.0f; //shadow resolution
  GLenum DrawBuffers;
  GLenum DrawGBuffers[6]
  { GL_COLOR_ATTACHMENT0, 
    GL_COLOR_ATTACHMENT1, 
    GL_COLOR_ATTACHMENT2, 
    GL_COLOR_ATTACHMENT3, 
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5 };
  GLuint Gbuffer, GBufferDepthBuffer;
  GLuint GBufferTexture[6];
  //material settings in GBuffer
  //USING
  //position 12 | emissive r  4
  //normal   12 | emissive g  4
  //diffuse  12 | emissive b  4
  //specular 12 | shiny       1
  //ambient  12 |

  //suggested
  //pos   | position  12 | 
  //color | normal    12 | empty     | ARGB8
  //aux0  | diffuse   12 | empty     | ARGB8
  //aux1  | specular  12 | shininess | ARGB8
  //depth | depth        |           | R24f

  std::vector<LightData> lightDatas;


  float rotateRate = 2.0f * PI / 20.0f;

  int objectShader = 0;

  float transmissionCoefficient = 1.0f;
  float highlightTightness = 0.5f;

  glm::vec2 windowPosition{ 0,0 };  //window position


  float materialRoughness = 1.0f;


  int windowX = 0;
  int windowY = 0;

private:
  //objectPos is used for the eye point of the reflection cameras
  vec3 objectPos;
  vec4 eyePos{0, 0, 2, 0};
  vec4 lookAt{0, 0, -1, 0 };

  int currentShader = 0;
  GLuint programIDs[ssMaxShaders];

  GLuint programID;
  
  
  GLfloat theta = 0.0;

  GLuint vertexbuffers[5];
  /*
    0 is verts
    1 is vert normals
    2 is faces
    3 is face normals
    4 is uv's
   */

  GLenum err;

  std::string renderSetting;
  // = glm::mat4(1.0f); initialzies to identity matrix
  glm::mat4 projectionMatrix = glm::mat4(1.0f);
  glm::mat4 viewMatrix = glm::mat4(1.0f);
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glm::mat4 modelTransform = glm::mat4(1.0f);
  glm::mat4 shadowMatrix = glm::mat4(1.0f);

  std::vector<Light> lights;
  
  //Iambient 
  vec3 ambientLight = vec3(0, 0, 0);

  GLuint uboHandle[2];
  GLubyte* uboBuffer[2] = { NULL };

  

};
#endif

//TODO
/*
  - Abstract texture loading so that it loads from slot 0 to whatever
    instead of setting the GL_TEXTURE# per thing

 */