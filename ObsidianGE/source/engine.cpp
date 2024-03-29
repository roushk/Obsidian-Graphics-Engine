#include <pch.h>
#define _CRT_SECURE_NO_WARNINGS

#define PI 3.14159265359f
#include "engine.h"

#include "shader.hpp"
#include "GUI.h"
#include "Render.h"
#include "Material.h"
#include "Camera.h"

#include "Random.h"
#include "InputManager.h"
#include "singleton.h"


using namespace glm;

int Light::lightID = 0;
int GUI::currentCam = 0;
int GUI::currentFBO = 0;
int GUI::currentGJK = 0;

bool GUI::rotateLights = true;
bool GUI::debugDrawMode = false;
bool GUI::autoCameraRotation = false;
bool GUI::rotateCamera = true;
bool GUI::showLightSpheres = false;
bool GUI::MSMShadow = true;
bool GUI::EnableLocalLights = false;
bool GUI::overwriteParams = true;
bool GUI::NormalMapping = false;
bool GUI::ParallaxMapping = false;
float GUI::ParallaxScale = 0.2f;
bool GUI::BRDF_IBL = true;
bool GUI::SSAO = true;
int GUI::currentTextureMaps = 0;

vec3 O = { 0, 0, 0 };

int engineRun()
{
  //hardcoding aspect to 1 for now and fov to 90

  RandomInit();
  int oldMaxLights = 0;;
  const float SQRT2 = sqrt(2.0f);


  auto& render = pattern::get<Render>();
  auto& reader = pattern::get<ObjectReader>();
  auto& matReader = pattern::get<MaterialReader>();
  auto& lighting = pattern::get<SceneLighting>();

  auto& input = pattern::get<InputManager>();
  auto& gui = pattern::get<GUI>();

  render.ClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });


  // Create and compile our GLSL program from the shaders
  render.CreateShaders();


  matReader.ReadMaterial("materials/t1.ppm");
  matReader.ReadMaterial("materials/t1.ppm");

  reader.loadMultiple("filenames.txt");


  //reader.ReadObject("models/hdsphere.obj");
  //reader.ReadObject("models/hdcube.obj");
  //reader.ReadObject("models/cup.obj");
  //reader.ReadObject("models/rhino.obj");
  //reader.ReadObject("models/toadpot-shelled.obj");
  //reader.ReadObject("models/4Sphere.obj");
  //reader.ReadObject("models/cube.obj");
  //reader.ReadObject("models/sphere.obj");
  
  //reader.ReadObject("models/cube2.obj");
  //reader.ReadObject("models/sphere.obj");
  //reader.ReadObject("models/lucy_princeton.obj");

  Model FSQ;
  FSQ.vertices = { 
    {{ -1,1, 0 }, {0,0,-1}, {0.0f,1.0f}},
    {{ 1, 1, 0 }, {0,0,-1}, {1.0f,1.0f}},
    {{ 1,-1, 0 }, {0,0,-1}, {1.0f,0.0f}},
    {{ -1,-1,0 }, {0,0,-1}, {0.0f,0.0f}}
  };
  FSQ.indices = { 0,1,2, 0,2,3 };
  FSQ.setup_mesh();


  Model NormalMapPlane;
  NormalMapPlane.vertices = {
    {{ -1,1, 0 }, {0,0,1}, {0.0f,1.0f}, {1,0,0}, {0,1,0}},
    {{ 1, 1, 0 }, {0,0,1}, {1.0f,1.0f}, {1,0,0}, {0,1,0}},
    {{ 1,-1, 0 }, {0,0,1}, {1.0f,0.0f}, {1,0,0}, {0,1,0}},
    {{ -1,-1,0 }, {0,0,1}, {0.0f,0.0f}, {1,0,0}, {0,1,0}}
  };
  NormalMapPlane.indices = { 0,1,2, 0,2,3 };
  
  NormalMapPlane.setup_mesh();

  std::vector<Model> objects;

  const std::vector<glm::vec3> objectOffsets = {
    {0, -11.5f, 0}, {-0.5f, -1, 1},
    {-1.8, -1, 1}, { 2, -1, 1},
    {1, -1, 1}, {3, -1, 1}, {-3, -1, 1},
    {0,-0.5, 0},
  };
  // {-1, -0.9, 0}, {-0.5, 1.2, 0},{0.5, -1.2, 0}, {0, 0, 0} };
  const std::vector<float> objectOffsetSizes =
  { 20,1,1,1,1,1,1,0.1f,1 };
  //{ 20, 1, 1, 1, 1, 1, 1, 1, 1 };
  
  pattern::get<Model>() = reader.GetObject(2);
  auto& object = pattern::get<Model>();
  Model sphere = reader.GetObject(0);
  Model cube = reader.GetObject(1);

  gui.BindImGUI(render.gWindow, &render.gContext);

  render.LoadModel(object);
  render.SetTitle(object.name);

  //seet DT with initial value
  float dt = 0.0166667f;
  render.CreateUBOBufferObjects();
  render.GenFrameBuffers();
  render.GenGBuffer();
  render.CreateBlurShadowData();
  render.HammersleyCreateData();

  render.LoadMaterial();
  render.LoadNormalAndHeight();
  /*
  front = -z
  back  = +z
  down  = -y
  up    = +y
  left  = -x
  right = +x
   */
  std::string name = "mp_hexagon/hexagon_"; std::string fileType = "tga";
  //std::string name = "test2/test_"; std::string fileType = "jpg";

  std::string positiveX = "skyboxes/" + name + "rt." + fileType;
  std::string negativeX = "skyboxes/" + name + "lf." + fileType;
  std::string positiveY = "skyboxes/" + name + "up." + fileType;
  std::string negativeY = "skyboxes/" + name + "dn." + fileType;
  std::string positiveZ = "skyboxes/" + name + "bk." + fileType;
  std::string negativeZ = "skyboxes/" + name + "ft." + fileType;

  std::vector < std::string> skyboxNames{ positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ };
  render.LoadSkybox(skyboxNames);
  render.SetObjectShader(ssPhongShadingAndFrensel);

  std::vector<Light> lights;
  std::vector<Light> localLights;

  Light l = Light("Static Light");
  l.SetPointLight({ 0,0,0 }, { 1,1,1 }, { 0.1, 0.1, 0.1 });
  const int staticLightMax = 100;

  //regular
  //float scaleLightIntentity = 1.0f / staticLightMax;

  //adjusted for lots of lights
  //float scaleLightIntentity = 5.0f / staticLightMax;

  //  float scaleLightIntentity = 0.001f / staticLightMax;

  float scaleLightIntentity = 0.001f / staticLightMax;

  float localLightScale = 2000.0f;
  float localLightMax = 10000.0f;

  glm::vec4 localLightCenter = glm::vec4(0, -1.9f, 1.0f, 0);
  for(unsigned i = 0; i < staticLightMax; ++i)
  {
    float x = RandomValue(-localLightMax, localLightMax);
    float y = RandomValue(-localLightMax, localLightMax);
    float z = RandomValue(-localLightMax, localLightMax);
    float r = RandomValue(0, localLightMax);
    float g = RandomValue(0, localLightMax);
    float b = RandomValue(0, localLightMax);
    l.position = localLightCenter + 
      glm::vec4(x / localLightScale, y / (localLightScale * 10), z / localLightScale, 0);
    l.diffuse = glm::vec4(r, g, b ,0);
    l.diffuse *= (scaleLightIntentity);

    l.specular = 1.0f * l.diffuse;
    l.ambient *= (scaleLightIntentity);
    l.setRadius(lighting, staticLightMax);
    l.SetPositionAndScaleCalc(l.position, l.radius);
    l.SetPositionAndScaleView(l.position, 0.1f);
    localLights.push_back(l);
  }
  objects.push_back(reader.GetObject(2)); //3 is cube
  objects.push_back(reader.GetObject(0)); //0 is sphere
  objects.push_back(reader.GetObject(1)); 
  objects.push_back(reader.GetObject(3));
  objects.push_back(reader.GetObject(4));
  objects.push_back(reader.GetObject(2));
  objects.push_back(reader.GetObject(2));
  //objects.push_back(NormalMapPlane);

  //objects.push_back(NormalMapPlane);
 
  render.LoadSkydome();
  render.SetCurrentCamera(6);
  render.cameraBase.yaw(0);

  static bool pauseScene = false;


  do {
    //start frame timer
    auto start = std::chrono::system_clock::now();
    //first get controls
    //glfwSetTime(0.0f);
    //clear color
    render.ClearScreen();
    //render.SetCurrentCamera(6);
    /*************************************************************************/
    //Commands
    input.Update(dt);
    render.Update();

    //render.currentCamera
    //glFrontFace(GL_CCW);
    //glCullFace(GL_BACK);


    if (oldMaxLights != lighting.maxLights)
    {
      std::cout << "Generated new Light Positions for :" << lighting.maxLights << " Lights" << std::endl;
      //generates new positions for lights if max lights change
      lighting.lights.clear();
      for (int i = 0; i < lighting.maxLights; ++i)
      {
        Light light("Light");
        float lightRadius = 2.0f;
        light.position = { lightRadius * cos(i *(2 * PI) / float(lighting.maxLights)),  
          (lightRadius * sin(i * (2 * PI) / float(lighting.maxLights))), 4.0f , 1.0f };
        lighting.lights.push_back(light);
        //    (x0 + r cos theta, y0 + r sin theta)

      }
      oldMaxLights = lighting.maxLights;
    }
    vec3 pos{0,0,-1}; //rotate around z/origin vector
    mat4 M;
    if(gui.rotateLights == true)
    {
      M = scale(rotate(render.rotateRate * dt, pos),vec3(1.0f)); //glm::translate(vec3(1, 0, 0) - O) *
      
    }
    else
    {
      M = rotate(render.rotateRate * 0, pos); //glm::translate(vec3(1, 0, 0) - O) *
    }

    render.UpdateCamera(dt);
    
                                              //Light Updates
    if(!pauseScene)
    {
      
      for (int i = 0; i < lighting.maxLights; ++i)
      {
        lighting.lights[i].position = lighting.lights[i].position * M;
        lighting.lights[i].Update();  //updates direction for lights
      }
    }
    
/************************************************************************************************/
    //Rendering

    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_CULL_FACE); //this fixed alot of issues
    glEnable(GL_DEPTH_TEST); //this fixed same issues and more of that cull face did

    static bool reflection = false;
    static bool skybox = false;
    
    
    if(reflection)
    {
      
/************************************************************************************************/
//REFLECTION RENDER
//Render To Framebuffers
//unsigned currentFrameBuffer = 1;// gui.currentFBO;
    
    
    for(unsigned currentFrameBuffer = 0; currentFrameBuffer < 6; ++currentFrameBuffer)
    {
      render.SetCurrentShader(ssSkyboxShader);

      render.BindAndCreateFrameBuffers(currentFrameBuffer);
      render.BindFrameBufferToRenderTo(currentFrameBuffer);
      //render.SetCurrentCamera(currentFrameBuffer);
      
      
      glDisable(GL_DEPTH_TEST); //disable depth test for the skybox
      glDepthMask(GL_FALSE);    //disable writing to depth buffer aswell
      glFrontFace(GL_CW); //inverts winding so that skybox can be sceen;
      render.BindSkybox();  //bind skybox textures and actives then on textures 0 to 6

      render.SetModelOffset(0, 0, 0, 8.0f); //sets skybox to scale of 8 centered at the origin so its big
      render.LoadModel(cube);
      render.Draw(cube);

      glFrontFace(GL_CCW); //resets winding so we can see rest of the scene
      glEnable(GL_DEPTH_TEST); //enable depth test for the rest of the scene
      glDepthMask(GL_TRUE);    //ensable writing to depth buffer aswell

  /***************************************************************************/
  //Render Lights

      render.SetCurrentShader(ssLightShader);
      render.LoadModel(sphere); //binds array and element buffers

      for (int i = 0; i < lighting.activeLights; ++i)
      {
        render.LoadDiffuseForLight(lighting.lights[i]);
        render.SetModelOffset(lighting.lights[i].position, 1.0f);
        render.Draw(sphere);
      }
      //render.TakePicOfFBO(currentFrameBuffer);
    }
    }
    
    /************************************************************************************************/
    //Actual Render
    //Render Objects

    /*
    std::vector<std::pair<Model*, glm::vec3>> objOffsetCombo;
    for(unsigned i = 0; i < objects.size(); ++i)
    {
      objOffsetCombo.push_back({ &objects[i], objectOffsets[i] });
    }
    */
    render.BindDefaultFrameBuffer();
    
    //render.SetCurrentCamera(gui.currentCam);

    if(skybox)
    {
        
      render.SetCurrentShader(ssSkyboxShader);
      render.BindDefaultFrameBuffer();
      //render.SetCurrentCamera(gui.currentCam);
      glEnable(GL_TEXTURE_2D);
      //glDisable(GL_DEPTH_TEST); //disable depth test for the skybox
      glDepthMask(GL_FALSE);    //disable writing to depth buffer as well
      glFrontFace(GL_CW); //inverts winding so that skybox can be seen;
      
      render.BindSkybox();  //bind skybox textures and actives then on textures 0 to 6

      render.SetModelOffset(0, 0, 0, 1000.0f); //sets skybox to scale of 8 centered at the origin so its big
      render.LoadModel(cube);
      render.Draw(cube);

      glFrontFace(GL_CCW); //resets winding so we can see rest of the scene
      //glEnable(GL_DEPTH_TEST); //enable depth test for the rest of the scene
      glDepthMask(GL_TRUE);    //enable writing to depth buffer as well

                               /***************************************************************************/
                               //Render Lights

    }
    
    if(reflection)
    {
      
/************************************************************************************************/
    //Render Reflection object
    //render.flipX = true;
    
    //render.SetCurrentShader(ssPhongShadingAndReflection);
    render.LoadObjectShader();
    render.BindFrameBufferTextures();
    render.BindDefaultFrameBuffer();
    //render.SetCurrentCamera(gui.currentCam);
    render.BindFrameBufferTextures();
    render.BufferRefractionData();

    render.UpdateCamera(dt);
    render.SetModelOffset(gui.position); //set model at -1
    render.SetObjPositionForCamera(gui.position);
    render.LoadModel(object);

    
    render.BindMaterials(lighting);
    render.BindLightScene(lighting);  //binds lights

    render.Draw(object);
    //render.flipX = false;

    }
    //for every draw call



    //Buffer GBuffer


    //render.BindDefaultFrameBuffer();
    //render.SetCurrentCamera(gui.currentCam);

    //when objects have different textures bind per object textures
    /************************************************************************************************/
    //GBuffer
    //Bind GBuffer
    //Draw Depth Pass
    //Draw Material Pass
    //Unbind GBuffer
    //draw objects to textures
    
    render.BindAndCreateGBuffers();
    render.BindGBuffer();
    render.ClearScreen();
    render.SetCurrentShader(ssDeferredGeometry);
    render.BindNormalAndHeight();
    render.BindMaterials(lighting);
    //global lights are the initial lights
    for (unsigned i = 0; i < objects.size(); ++i)
    {
      render.SetModelOffset(objectOffsets[i], objectOffsetSizes[i]); //set model at -1
      render.SetObjPositionForCamera(objectOffsets[i]);
      render.LoadModel(objects[i]);
      render.Draw(objects[i]);
    }
    render.BindDefaultFrameBuffer();


    /************************************************************************************************/
    //SSAO Map Creation pass
    render.SetCurrentShader(ssSSAO);
    render.SSAOCreateFBO();
    render.SSAOBindFBO();
    render.LoadSSAOValues();
    //bind gbuffer
    render.BindGBufferTextures();
    render.LoadScreenSize();
    render.SetModelOffset(0, 0, 0);
    render.SetObjPositionForCamera({ 0,0,0 });

    render.LoadModel(FSQ);
    render.Draw(FSQ);

    render.BindDefaultFrameBuffer();


    /************************************************************************************************/
    //Shadow Map pass

    
    render.SetCurrentShader(ssShadowShader);
    render.BindAndCreateShadowBuffers();
    render.BindShadowBuffer();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    //render.BindMaterials(lighting);
    //global lights are the initial lights
    for (unsigned i = 0; i < objects.size(); ++i)
    {
      //render.LoadObjectShader();
      render.SetModelOffset(objectOffsets[i], objectOffsetSizes[i]); //set model at -1
      render.SetObjPositionForCamera(objectOffsets[i]);
      render.LoadModel(objects[i]);
      render.DrawShadow(objects[i], lighting.lights[0]);
    }
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    render.BindDefaultFrameBuffer();



    /************************************************************************************************/
    //Compute Shader Shadow Map Blur pass
    
    //Once
    render.BindAndCreateBlurShadowBuffers();

    //Horizontal Pass
    render.SetCurrentShader(ssComputeBlurHorizontal);
    render.BlurShadowLoadData();  //once but inside shader
    render.BlurShadowLoadHorizontal();

    glDispatchCompute((render.height * render.aspect * render.shadowScale) / 128,
      render.height * render.shadowScale, 1); // Tiles WxH image with groups sized 128x1

    //Vertical Pass
    render.SetCurrentShader(ssComputeBlurVertical);
    render.BlurShadowLoadData();
    render.BlurShadowLoadVertical();

    glDispatchCompute((render.height * render.aspect * render.shadowScale),
      (render.height * render.shadowScale) / 128, 1); // Tiles WxH image with groups sized 128x1
      


     /************************************************************************************************/
    //Compute Shader SSAO Map Blur pass

    //Once
    render.BindAndCreateSSAOBlurBuffers();

    //Horizontal Pass
    render.SetCurrentShader(ssSSAOBlurHorizontal);
    render.BlurShadowLoadData();  //once but inside shader
    render.SSAOBlurLoadHorizontal();
    render.LoadSSAOBlurValues();

    glDispatchCompute((render.height * render.aspect * render.shadowScale) / 128,
      render.height * render.shadowScale, 1); // Tiles WxH image with groups sized 128x1

    //Vertical Pass
    render.SetCurrentShader(ssSSAOBlurVertical);
    render.BlurShadowLoadData();
    render.SSAOBlurLoadVertical();
    render.LoadSSAOBlurValues();

    glDispatchCompute((render.height * render.aspect * render.shadowScale),
      (render.height * render.shadowScale) / 128, 1); // Tiles WxH image with groups sized 128x1

    //draw objects to 
    //render.SetCurrentShader() //set shader to lighting shader
    render.BindDefaultFrameBuffer();


    /************************************************************************************************/
    //Draw to Geometry Buffer
    //render.BindWidthAndHeight();
   
    //Draw light stuff to FSQ
    if (GUI::debugDrawMode)
    {
      render.SetCurrentShader(ssDeferredRendering);
      render.BlurShadowLoadDebug();
      render.SSAOBlurLoadDebug();
      render.SSAOLoadDebug();
      render.SetCurrentDebugTexture(gui.currentCam);
    }
    else if(!GUI::debugDrawMode && GUI::MSMShadow)
    {
      //TODO FIX SHADER INTO BRDF DEFERRED
      if (GUI::BRDF_IBL)
      {
        render.SetCurrentShader(ssBRDFDeferredMSM);

      }
      else
      {
        render.SetCurrentShader(ssPhongShadingDeferredShadowMSM);
        
      }
      //render.BlurShadowLoadFinalMap();
      //render.SetCurrentShader(ssPhongShadingDeferredShadowMSM);
      render.BlurShadowLoadFinalMap();
      render.LoadRoughness(render.materialRoughness);
      render.BindSkydome();
      render.HammersleyLoadData();
      render.LoadMaxDepth();
      render.SSAOLoadBlur();
      
      //render.SetCurrentDebugTexture(6);
    }
    else if (!GUI::debugDrawMode && !GUI::MSMShadow)
    {
      render.SetCurrentShader((ssPhongShadingDeferredShadow));
      //render.SetCurrentDebugTexture(6);
    }

    render.BindNormalAndHeight();

    render.BufferToneMapping();
    //bind shadow textures
    //render.SetCurrentCamera(6);
    render.BindGBufferTextures();
    render.BindMaterials(lighting);
    render.BindLightScene(lighting);  //binds lights
    render.LoadDiffuseForLight(lighting.lights[0], 0.005f);
    render.LoadScreenSize();

    render.SetModelOffset(0, 0, 0);
    render.SetObjPositionForCamera({ 0,0,0 });

    render.BindShadowTextures();

    render.LoadModel(FSQ);
    render.Draw(FSQ);
    
    //glEnable(GL_CULL_FACE);

    //Copy old depth buffer 
    //if (GUI::copyDepth)
    //  render.CopyDepthBuffer();

    render.BindGBufferTextures();
    render.BindMaterials(lighting);
    
    
    /*
    on the cpu
    you have to reverse the attenuation function to get the radius of effect
    then you have to render a sphere with that radius
    and use the lighting shader when you render the sphere
    so it only affects that area
    instead of rendering a FSQ
    a FSQ affects the whole screen
    but a sphere effects only part of the screen
    and you have to use gl_FragCoord.xy / vec2(width, height) for the uv
    cant use sphere uvs
    thats basically it
    the shader doesnt change except uvs become gl_FragCoord.xy / vec2(width, height)
    and the vertex shader takes a model matrix for the sphere instead of FSQ
     */
    
    //backup lights
    
    if (GUI::EnableLocalLights && !GUI::debugDrawMode)
    {
      render.SetCurrentShader(ssPhongShadingDeferredLightSphere);
      //shows light spheres of influence
      //render.SetCurrentShader(ssLightShader);
      //render.SetCurrentShader(ssPhongShadingDeferred);
      if(GUI::showLightSpheres)
        render.SetCurrentShader(ssLightShader);


      lights = lighting.lights;
      //glEnable(GL_BLEND);
      //glBlendEquationi(0, GL_FUNC_ADD);
      //glBlendFunc(GL_ONE, GL_ONE);
      //glBlendEquation(GL_FUNC_ADD);
      //glDisable(GL_DEPTH_TEST);
      //glEnable(GL_CULL_FACE);
      //glCullFace(GL_BACK);

      int oldActiveLights = lighting.activeLights;
      for (auto& light : localLights)
      {
        render.BindGBufferTextures();
        lighting.lights.clear();
        lighting.lights.push_back(light);
        lighting.activeLights = 1;
        render.BindLightScene(lighting);  //binds lights

        render.LoadDiffuseForLight(light, 0.005f);
        render.LoadScreenSize();

        render.SetModelOffset(light);
        render.SetObjPositionForCamera(light.position);
        render.LoadModel(sphere);
        render.Draw(sphere);
        /*

        render.SetModelOffset(vec3(0));
        render.SetObjPositionForCamera(vec3(0));
        render.LoadModel(FSQ);
        render.Draw(FSQ);
        */

        //Copy old depth buffer 
      }

      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
      //glEnable(GL_CULL_FACE);

      lighting.activeLights = oldActiveLights;
      lighting.lights = lights;
    }
    glBlendEquationi(0, GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_DEPTH_TEST);
    

    render.CopyDepthBuffer();

    render.BindDefaultFrameBuffer();


    //Skydome
    render.SetCurrentShader(ssSkydome);
    //render.SetCurrentCamera(gui.currentCam);
    //glDisable(GL_DEPTH_TEST); //disable depth test for the skybox
    //glDepthMask(GL_FALSE);    //disable writing to depth buffer aswell
    glFrontFace(GL_CW); //inverts winding so that skybox can be sceen;
    render.BufferToneMapping();
    render.BindSkydome();  //bind skybox textures and actives then on textures 0 to 6
    render.SetModelOffset(render.currentCamera.eye(), 100.0f); //sets skybox to scale of 8 centered at the origin so its big
    render.LoadModel(sphere);
    render.Draw(sphere);

    glFrontFace(GL_CCW); //resets winding so we can see rest of the scene
    //glEnable(GL_DEPTH_TEST); //enable depth test for the rest of the scene
    //glDepthMask(GL_TRUE);    //enable writing to depth buffer as well

    //glEnable(GL_DEPTH_TEST); //this fixed same issues and more of that cull face did

    //glDisable(GL_CULL_FACE);
    render.SetCurrentShader(ssPhongShadingDeferredLightSphere);
    render.BindLightScene(lighting);
    //Draw lights
    static bool renderLights = true;
    if (renderLights)
    {
      render.SetCurrentShader(ssLightShader);
      render.LoadModel(sphere); //binds array and element buffers

      //render actual lights
      for (int i = 0; i < lighting.activeLights; ++i)
      {
        render.LoadDiffuseForLight(lighting.lights[i]);
        render.SetModelOffset(lighting.lights[i].position, 1.0f);
        render.Draw(sphere);
      }
      if (GUI::EnableLocalLights)
      {

        for (auto& light : localLights)
        {
          render.LoadDiffuseForLight(light);
          render.SetModelOffsetView(light);
          render.Draw(sphere);
        }
      }
      //glClear(GL_DEPTH_BUFFER_BIT);
    }

    /***************************************************************************/
    //Render GUI
    //update and render the debug gui
    //render.SetCurrentShader(ssReflectionMap);
    gui.Update();
    gui.RenderFrame();
    
    //render.CleanUpUBOBuffers(lighting.activeLights);
    render.EndDrawing();
    // Update the light Positions
    


    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    
    
    auto end = std::chrono::system_clock::now();
    auto chronoDt = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    dt = chronoDt.count() / 1000000000.0f;


    float fps = 1.0f / (dt);


    std::string fpsTitle = " FPS = |";
    fpsTitle += std::to_string(static_cast<int>(fps));
    fpsTitle += "|";
    SDL_SetWindowTitle(render.gWindow, fpsTitle.c_str());
  } // Check if the ESC key was pressed or the window was closed
  while (!input.stop);


  return 0;
}
