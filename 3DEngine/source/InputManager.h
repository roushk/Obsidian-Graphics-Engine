#pragma once

class InputManager
{
public:
  InputManager();
  ~InputManager();

  void Update(float dt);

  std::string name;

  glm::vec2 mouseWorldCoords;

  bool stop = false;
  glm::vec2 mousePosition{0,0};
  glm::vec2 offset{0,0};

  //bool firstMouse = true;
  int lastY, lastX;


  bool toggleCamera = false;
  bool oldPos = false;
  float sensitivity = 0.05f;
  //float sensitivity = 0.05;


  bool pressedLeft = false;
  bool pressedRight = false;
  unsigned iteratorVal = 0;


};