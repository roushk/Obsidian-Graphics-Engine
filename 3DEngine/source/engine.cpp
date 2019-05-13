#include <pch.h>

#define PI 3.14159265359f

#include "shader.hpp"
#include "GUI.h"
#include "Render.h"
#include "Material.h"
#include "Camera.h"

#include "Random.h"


using namespace glm;

int Light::lightID = 0;
int GUI::currentCam = 0;
int GUI::currentFBO = 0;
BoundingBoxes GUI::currentBoundingMethod = BoundingBoxes::AABB;
int GUI::currentGJK = 0;

bool GUI::debugDrawMode = false;
bool GUI::copyDepth = true;
bool GUI::topDown = false;
bool GUI::reloadOctree = false;
bool GUI::renderOctree = false;
bool GUI::reloadBSPTree = false;
bool GUI::renderBSPTree = true;
bool GUI::renderModels = false;

vec3 O = { 0, 0, 0 };

int main(void)
{
  //hardcoding aspect to 1 for now and fov to 90
  
  RandomInit();
  int oldMaxLights = 0;;

  
  const float SQRT2 = sqrt(2.0f);
  Render render;
  

  if (!render.InitRender())
  {
    //crashed release???
    //return -1;
  }

  render.ClearColor(vec4(0.7f, 0.7f, 0.7f, 0.7f));
  // Create and compile our GLSL program from the shaders
  render.CreateShaders();
  render.CreateBuffers();

  ObjectReader reader;
  MaterialReader matReader;
  SceneLighting lighting;
  
  matReader.ReadMaterial("materials/metal_roof_diff_512x512.ppm");
  matReader.ReadMaterial("materials/metal_roof_spec_512x512.ppm");

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

  Mesh FSQ;
  FSQ.verts =         { { -1,1,0 }, { 1,1,0 }, { 1,-1,-1 }, {-1,-1,0} };
  FSQ.uvCylindrical = { {0.0f,1.0f},{1.0f,1.0f},{1.0f,0.0f},{0.0f,0.0f} };
  FSQ.faces =         { {0,1,2}, {0,2,3} };
  FSQ.setup_mesh();

  std::vector<Object> objects;

  const std::vector<glm::vec3> objectOffsets = {
    {-2, -2, 0},{2, -2, 0},
    {-2,  2, 0},{ 2,  2, 0},
    {1, 0.9, 0},{-1, -0.9, 0},
    {-0.5, 1.2, 0},{0.5, -1.2, 0}, {0, 0, 0} };

  Object object = reader.GetObject(3);
  Object sphere = reader.GetObject(0);

  Object lowPolySphere = reader.GetObject(7);


  Object cube = reader.GetObject(1);




  GUI gui(reader, lighting, render);
  gui.BindImGUI(render.window);

  render.LoadModel(object);
  render.SetTitle(object.name);

  bool pressedLeft = false;
  bool pressedRight = false;

  bool pressedB = false;
  unsigned iteratorVal = 0;
  float dt = 0;
  render.CreateUBOBufferObjects();
  render.GenFrameBuffers();
  render.GenGBuffer();

  render.LoadMaterial(matReader.GetMaterial(0), matReader.GetMaterial(1));

  dvec2 cursorPos(0,0);
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

  

  objects.push_back(reader.GetObject(0));
  objects.push_back(reader.GetObject(1));
  objects.push_back(reader.GetObject(2));
  objects.push_back(reader.GetObject(3));
  objects.push_back(reader.GetObject(2));
  //objects.push_back(reader.GetObject(4));
  objects.push_back(reader.GetObject(0));
  objects.push_back(reader.GetObject(3));
  objects.push_back(reader.GetObject(1));
  objects.push_back(object);



  //createPointCloudFromObj(objects[0], pointCloud, objectOffsets[0]);

  //ball creation shoot ball
  glm::vec3 ballForwardVec = -render.currentCamera.back();
  Object ball = lowPolySphere;
  
  render.cameraBase.yaw(0);

  static bool pauseScene = false;

  do {
    //start frame timer
    auto start = std::chrono::system_clock::now();
    bool controls = true;
    static bool shootBall = false;
    //first get controls
    if (controls)
    {

      if (pressedLeft && gui.buttonLeftDown == false)
      {
        ++iteratorVal;
        iteratorVal = iteratorVal % reader.MaxObjects();
        object = reader.GetObject(iteratorVal);

        render.SetTitle(object.name);
        pressedLeft = false;
      }
      if (gui.buttonLeftDown == true)
      {
        pressedLeft = true;
      }

      if (pressedRight && gui.buttonRightDown == false)
      {
        --iteratorVal;
        iteratorVal += reader.MaxObjects(); //incase 0
        iteratorVal = iteratorVal % reader.MaxObjects();
        object = reader.GetObject(iteratorVal);

        render.SetTitle(object.name);
        pressedRight = false;
      }
      if (gui.buttonRightDown == true)
      {
        pressedRight = true;
      }

      //toggle between normal, color, none
      /*
      if (pressedB && glfwGetKey(render.window, GLFW_KEY_B) == GLFW_RELEASE)
      {
        render.setting++;
        render.setting = render.setting % 3;

        render.SetTitle(object.name);
        pressedB = false;
      }
      */
      if (glfwGetKey(render.window, GLFW_KEY_B) == GLFW_PRESS)
      {
        pressedB = true;
      }
      const float speed = 1.0f * dt;
      if (glfwGetKey(render.window, GLFW_KEY_W) == GLFW_PRESS)
      {
        render.cameraBase.forward(speed * 2.0f);
      }
      if (glfwGetKey(render.window, GLFW_KEY_S) == GLFW_PRESS)
      {
        render.cameraBase.forward(-speed * 2.0f);
      }

      if (glfwGetKey(render.window, GLFW_KEY_A) == GLFW_PRESS)
      {
        render.cameraBase.leftRight(speed * 2.0f);
      }
      if (glfwGetKey(render.window, GLFW_KEY_D) == GLFW_PRESS)
      {
        render.cameraBase.leftRight(-speed * 2.0f);
      }

      if (glfwGetKey(render.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      {
        render.cameraBase.upDown(-speed * 2.0f);
      }
      if (glfwGetKey(render.window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
      {
        render.cameraBase.upDown(speed * 2.0f);
      }


      if (glfwGetKey(render.window, GLFW_KEY_Q) == GLFW_PRESS)
      {
        render.cameraBase.roll(speed * 0.8f);
      }
      if (glfwGetKey(render.window, GLFW_KEY_E) == GLFW_PRESS)
      {
        render.cameraBase.roll(-speed * 0.8f);
      }
      dvec2 newCursorPos;

      glfwGetCursorPos(render.window, &newCursorPos.x, &newCursorPos.y);

      if (glfwGetMouseButton(render.window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
      {

        if (newCursorPos.x > cursorPos.x)
        {
          render.cameraBase.yaw(-speed);
        }
        if (newCursorPos.x < cursorPos.x)
        {
          render.cameraBase.yaw(speed);
        }

        if (newCursorPos.y > cursorPos.y)
        {
          render.cameraBase.pitch(-speed);
        }
        if (newCursorPos.y < cursorPos.y)
        {
          render.cameraBase.pitch(speed);
        }

      }
      

      cursorPos = newCursorPos;
    }
    
    if (glfwGetKey(render.window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
      shootBall = true;
      ball = Sphere(render.cameraBase.eye(), shootSphere.scale);
      ballForwardVec = -render.cameraBase.back();
    }
    if (glfwGetKey(render.window, GLFW_KEY_R) == GLFW_PRESS)
    {
      pauseScene = false;
      ball.center = glm::vec3(-1000, -1000, -1000);
      ballForwardVec = glm::vec3(0, 0, 0);
    }


    //glfwSetTime(0.0f);
    //clear color
    render.ClearScreen();
    render.SetCurrentCamera(6);
    
    /*************************************************************************/
    //Commands
    render.Update();
    

    if (oldMaxLights != lighting.maxLights)
    {
      std::cout << "Generated new Light Positions for :" << lighting.maxLights << " Lights" << std::endl;
      //generates new positions for lights if max lights change
      lighting.lights.clear();
      for (int i = 0; i < lighting.maxLights; ++i)
      {
        Light light("Light");
        float lightRadius = 2.0f;
        light.position = { lightRadius * cos(i *(2 * PI) / float(lighting.maxLights)),  lightRadius * sin(i * (2 * PI) / float(lighting.maxLights)), 0 , 1.0f };
        lighting.lights.push_back(light);
        //    (x0 + r cos theta, y0 + r sin theta)

      }
      oldMaxLights = lighting.maxLights;
    }
    vec3 pos{0,0,-1}; //rotate around z/origin vector
    mat4 M;
    if(gui.rotateLights == true)
      M = scale(rotate(render.rotateRate * dt, pos),vec3(1.0f)); //glm::translate(vec3(1, 0, 0) - O) *
    else
      M = rotate(render.rotateRate * 0, pos); //glm::translate(vec3(1, 0, 0) - O) *

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
      //render.BindModelBuffer();
      render.Draw(cube);

      glFrontFace(GL_CCW); //resets winding so we can see rest of the scene
      glEnable(GL_DEPTH_TEST); //enable depth test for the rest of the scene
      glDepthMask(GL_TRUE);    //ensable writing to depth buffer aswell

  /***************************************************************************/
  //Render Lights

      render.SetCurrentShader(ssLightShader);
      render.LoadModel(sphere); //binds array and element buffers
      //render.BindModelBuffer();

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
    objects.pop_back();

    //updates last objects aabb
    objects.push_back(object);

    std::vector<std::pair<Object*, glm::vec3>> objOffsetCombo;
    for(unsigned i = 0; i < objects.size(); ++i)
    {
      objOffsetCombo.push_back({ &objects[i], objectOffsets[i] });
    }

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

      render.SetModelOffset(0, 0, 0, 90.0f); //sets skybox to scale of 8 centered at the origin so its big
      render.LoadModel(cube);
      //render.BindModelBuffer();
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
    //render.BindModelBuffer();

    
    render.BindMaterials(lighting);
    render.BindLightScene(lighting);  //binds lights

    render.Draw(object);
    //render.flipX = false;

    }
    //for every draw call

    //Buffer GBuffer
    render.BindAndCreateGBuffers();
    render.BindGBuffer();
    render.ClearColor({ 0.5f, 0.5f, 0.5f, 0.5f });
    render.ClearScreen();

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
    
    render.SetCurrentShader(ssDeferredGeometry);
    render.BindMaterials(lighting);
    for (unsigned i = 0; i < objects.size(); ++i)
    {
      //render.LoadObjectShader();
      if (i == objects.size() - 1)
        render.SetModelOffset(gui.position);
      else
        render.SetModelOffset(vec3(0, 0, 0));

      render.SetModelOffset(objectOffsets[i]); //set model at -1
      render.SetObjPositionForCamera(objectOffsets[i]);
      render.LoadModel(objects[i]);
      //render.BindModelBuffer();
      if (GUI::renderModels)
      {
        render.Draw(objects[i]);
      }
    }
    

    //clear old values out of FBO0 if any left
    //render.ClearScreen();

    //draw objects to 
    //render.SetCurrentShader() //set shader to lighting shader
    render.BindDefaultFrameBuffer();

    //Draw light stuff to FSQ
    if (GUI::debugDrawMode)
      render.SetCurrentShader(ssDeferredRendering);
    else
      render.SetCurrentShader((ssPhongShadingDeferred));
    render.SetCurrentDebugTexture(gui.currentCam);
    render.SetCurrentCamera(6);
    render.BindGBufferTextures();
    render.BindMaterials(lighting);
    render.BindLightScene(lighting);  //binds lights

    render.SetModelOffset(0.0f, 0, 0);
    render.SetObjPositionForCamera({ 0,0,0 });

    render.LoadModel(FSQ);
    //render.BindModelBuffer();
    render.Draw(FSQ);

    //Copy old depth buffer 
    if (GUI::copyDepth)
      render.CopyDepthBuffer();
    render.BindDefaultFrameBuffer();

    //Draw lights
    static bool renderLights = true;
    if (renderLights)
    {
      render.SetCurrentShader(ssLightShader);
      render.LoadModel(sphere); //binds array and element buffers
      //render.BindModelBuffer();

      //render actual lights
      for (int i = 0; i < lighting.activeLights; ++i)
      {
        render.LoadDiffuseForLight(lighting.lights[i]);
        render.SetModelOffset(lighting.lights[i].position, 1.0f);
        render.Draw(sphere);
      }
      //glClear(GL_DEPTH_BUFFER_BIT);
    }

    
    if (shootBall)
    {
      if (!pauseScene)
      {
        ball.center += ballForwardVec * 10.0f * dt;
      }

      render.SetModelOffset(ball.center, 0.1f);
      render.SetCurrentShader(ssLightShader);
      glm::vec4 redColor(0.5, 0, 0, 1);
      render.LoadDiffuseForLight(redColor);

      render.LoadModel(ball);
      render.Draw(ball);

      if(!pauseScene)
      {
        bool pause = false;

        if(pause)
        {
          pauseScene = true;
        }
      }

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
    auto end = std::chrono::system_clock::now();
    auto chronoDt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    dt = chronoDt.count() / 1000.f;

    
    
  } // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(render.window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(render.window) == 0);


  return 0;
}
