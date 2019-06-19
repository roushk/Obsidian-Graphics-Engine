#pragma once

class InputManager
{
public:
  InputManager();
  ~InputManager();

  void Update(float dt);

  std::string name;
  //std::queue<InputEvent> inputEvents;

  glm::vec2 screenSize;

  glm::vec2 mouseScreenCoords;
  glm::vec2 mouseWorldCoords;

  glm::mat4 scaleToWorld = glm::scale(glm::vec3(10.0f / 800.0f, 10.0f / 600.0f, 1));

  float aspect = screenSize.x / screenSize.y;

  bool stop = false;

};