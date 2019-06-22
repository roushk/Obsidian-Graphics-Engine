#ifndef GUI_H
#define GUI_H
#define PI 3.14159265359f


#include <glm/glm.hpp>


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

  GUI();
  ~GUI();

  void SetStyle();
  void BindImGUI(SDL_Window* window, SDL_GLContext* context);
  void RenderFrame();
  void Update(float dt = 0);

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
