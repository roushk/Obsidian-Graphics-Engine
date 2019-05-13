
#ifndef GUI_H
#define GUI_H
#define PI 3.14159265359f

#include <vector>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#include "Object.h"
#include "Light.h"
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
