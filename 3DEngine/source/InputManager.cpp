#include <pch.h>



InputManager::InputManager()
{
  name = "Input Manager";
  //auto input = pattern::get<InputManager>();
  screenSize = render.screenSize;
  scaleToWorld = glm::scale(glm::vec3(
    (engine.GetSystem<Render>()->worldScale) / screenSize.x,
    (engine.GetSystem<Render>()->worldScale) / screenSize.y, 1));
  aspect = screenSize.x / screenSize.y;
}

InputManager::~InputManager()
{

}
void InputManager::Update(float dt)
{
  auto render = pattern::get<Render>();
  auto input = pattern::get<InputManager>();
  SDL_Event event;
  //while event queue is not empty pop off and deal with
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    default:
      //default event
      break;
      //if you press the close button the game shutsdown the engine
    case SDL_QUIT:
    {
      stop = true;
    }
    break;

    case SDL_WINDOWEVENT:
    {
      if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        //|| event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) //breaks everything called ALOT

      {

        engine.GetSystem<Render>()->resize(event.window.data1, event.window.data2);
        screenSize = engine.GetSystem<Render>()->screenSize;
        scaleToWorld = glm::scale(glm::vec3(
          (engine.GetSystem<Render>()->worldScale) / screenSize.x,
          (engine.GetSystem<Render>()->worldScale) / screenSize.y, 1));
        aspect = screenSize.x / screenSize.y;
      }
      else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
      {

      }

    }

    break;
    /*SDL_KeyboardEvent*/
    case SDL_KEYDOWN:
    {

      float cameraAccelSpeed = 10.0f;

      if (event.key.keysym.scancode == SDL_SCANCODE_W)
      {
        engine.GetSystem<Render>()->cameraVel += glm::vec2(0, -cameraAccelSpeed);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_A)
      {
        engine.GetSystem<Render>()->cameraVel += glm::vec2(cameraAccelSpeed, 0);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_S)
      {
        engine.GetSystem<Render>()->cameraVel += glm::vec2(0, cameraAccelSpeed);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_D)
      {
        engine.GetSystem<Render>()->cameraVel += glm::vec2(-cameraAccelSpeed, 0);
      }
      //engine.GetSystem<Render>()->cameraPos =
      //  glm::translate(engine.GetSystem<Render>()->cameraPos, glm::vec3(cameraPos, 0));
    }
    break;
    case SDL_KEYUP:

      break;

      /*SDL_MouseButtonEvent*/
    case SDL_MOUSEBUTTONDOWN:
    {
      if (event.button.button == SDL_BUTTON_RIGHT && event.button.clicks == 1)
      {
        
      }

    }
    break;
    case SDL_MOUSEBUTTONUP:
    {
      //Drag select
      if (event.button.button == SDL_BUTTON_LEFT
        && event.button.state == SDL_RELEASED)
      {

      }
    }
    break;
    /*SDL_MouseWheelEvent*/
    case SDL_MOUSEWHEEL:
    {
      float scale = 1.0f;
      if (event.wheel.y > 0) // scroll up
      {
        scale = 1.1f;
        // Pull up code here!
      }
      else if (event.wheel.y < 0) // scroll down
      {
        // Pull down code here!
        scale = 0.9f;
      }
      engine.GetSystem<Render>()->cameraScale *= scale;
      //= glm::scale(engine.GetSystem<Render>()->cameraPos, glm::vec3(scale, scale, 1.0f));
    }
    break;
    /*SDL_MouseMotionEvent*/
    case SDL_MOUSEMOTION:
    {
      //updates mouse world and mouse screen coords

      //mouseScreenCoords = glm::vec2((-screenSize.x / 2.0f + event.motion.x) * aspect, screenSize.y / 2.0f + -event.motion.y);
      mouseScreenCoords = glm::vec2((-screenSize.x + event.motion.x * 2.0f) * aspect, screenSize.y + -event.motion.y * 2.0f);

    }
    break;
}
    }
  }
