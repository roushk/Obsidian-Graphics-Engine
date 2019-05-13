/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: GUI.h
Purpose: Header for ImGui GUI system 
Language: C++ MSVC
Platform: VS 141, OpenGL 4.3 compatabile device driver, Win10
Project: coleman.jonas_CS350_1
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 6/20/18
End Header --------------------------------------------------------*/



#ifndef GUI_H
#define GUI_H
#define PI 3.14159265359f

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <vector>
#include "Object.h"
#include "Light.h"
#include <GLFW/glfw3.h>
#include "imgui/imgui_internal.h"
#include "Render.h"

enum class BoundingBoxes : int
{
  AABB,
  BSCentroid,
  BSRitter,
  BSLarsson,
  BSPCA,
  BEPCA,
  OBBPCA,

  BVH_AABB,
  BVH_BS,
  NONE

};
/*create new light and set type with load and name and provide data*/
class GUI
{
public:

  GUI(ObjectReader& reader_, SceneLighting& lighting_, Render& render_);

  void SetStyle();
  void BindImGUI(GLFWwindow* window);
  void RenderFrame();
  void Update(float dt = 0);
  ~GUI();

  bool buttonLeftDown = false;
  bool buttonRightDown = false;
  bool rotateLights = true;
  vec3 position{0,0,0};
  static int currentCam;
  static int currentFBO;
  static int currentGJK;

  static BoundingBoxes currentBoundingMethod;
  static bool debugDrawMode;
  static bool copyDepth;
  static bool topDown;
  static bool reloadOctree;
  static bool renderOctree;
  static bool reloadBSPTree;
  static bool renderBSPTree;
  static bool renderModels;
private:
  SceneLighting& lighting;
  ObjectReader& reader;
  Render& render;
};
#endif
