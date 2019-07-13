#include <pch.h>



InputManager::InputManager()
{
  name = "Input Manager";
  lastX = mousePosition.x;
  lastY = mousePosition.y;
}

InputManager::~InputManager()
{

}

void InputManager::Update(float dt)
{
  //stops camera mouse from jumping after unclicking right clock
  //this is caused by the input not being calculated with each type all at once and
  //instead buffers the clicks in different order and causes the toggle camera
  //update thing to be called twice. The solution to this is to make a real
  //input manager but this works for now...
  bool updatedThisLoop = false;


  auto& render = pattern::get<Render>();
  auto& input = pattern::get<InputManager>();
  auto& gui = pattern::get<GUI>();
  auto& reader = pattern::get<ObjectReader>();
  auto& camera = render.currentCamera;

  //update window size
  SDL_GetWindowSize(render.gWindow, &render.windowX, &render.windowY);

  //update window position
  int windowXPos = 0;
  int windowYPos = 0;
  SDL_GetWindowPosition(render.gWindow, &windowXPos, &windowYPos);
  render.windowPosition = glm::vec2(windowXPos, windowYPos);
  
  auto& object = pattern::get<Model>();

  if (pressedLeft && gui.buttonLeftDown == false)
  {
    
    ++iteratorVal;
    iteratorVal = iteratorVal % reader.models.size();
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
    iteratorVal += reader.models.size(); //incase 0
    iteratorVal = iteratorVal % reader.models.size();
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
 
  const float speed = 20.0f * dt;

  SDL_Event event;
  //while event queue is not empty pop off and deal with
  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type)
    {
    default:
      //default event
      break;
      //if you press the close button the game shuts down the engine
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
        render.resize(event.window.data1, event.window.data2);
        
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
        camera.forward(speed * 2.0f);
        render.cameraChanged = true;
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_A)
      {
        camera.leftRight(speed * 2.0f);
        render.cameraChanged = true;
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_S)
      {
        camera.forward(-speed * 2.0f);
        render.cameraChanged = true;
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_D)
      {
        camera.leftRight(-speed * 2.0f);
        render.cameraChanged = true;
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
      {
        camera.upDown(-speed * 2.0f);
        render.cameraChanged = true;
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_LCTRL)
      {
        camera.upDown(speed * 2.0f);
        render.cameraChanged = true;
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_Q)
      {
        camera.roll(speed * 0.2f);
        render.cameraChanged = true;
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_E)
      {
        camera.roll(-speed * 0.2f);
        render.cameraChanged = true;
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
      if (event.button.button == SDL_BUTTON_RIGHT && event.button.state == SDL_PRESSED
         && updatedThisLoop == false)
      //&& event.button.type == SDL_MOUSEBUTTONDOWN)
      {
        updatedThisLoop = true;
        if (toggleCamera == false)
        {
          toggleCamera = true;
          SDL_SetRelativeMouseMode(SDL_bool(true));
        }
        else if (toggleCamera == true)
        {
          toggleCamera = false;
          SDL_SetRelativeMouseMode(SDL_bool(false));

          glm::vec2 windowRes = glm::vec2(render.height*render.aspect, render.height);

          SDL_WarpMouseGlobal(render.windowPosition.x + windowRes.x / 2,
            render.windowPosition.y + windowRes.y / 2);
        }

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
        camera.zoom(0.9f);
        render.cameraChanged = true;
        // Pull up code here!
      }
      else if (event.wheel.y < 0) // scroll down
      {
        camera.zoom(1.1f);
        render.cameraChanged = true;
        // Pull down code here!
      }
      //= glm::scale(engine.GetSystem<Render>()->cameraPos, glm::vec3(scale, scale, 1.0f));
    }
    break;
    /*SDL_MouseMotionEvent*/
    case SDL_MOUSEMOTION:
    {
      //updates mouse world and mouse screen coords

      //mouseScreenCoords = glm::vec2((-screenSize.x / 2.0f + event.motion.x) * aspect, screenSize.y / 2.0f + -event.motion.y);
      offset += glm::vec2(event.motion.xrel, event.motion.yrel);
      mousePosition = glm::vec2{ event.motion.x, event.motion.y } + offset;
    }
    break;
    }
  }

  

  float xoffset = mousePosition.x - lastX;
  float yoffset = lastY - mousePosition.y;
  lastX = mousePosition.x;
  lastY = mousePosition.y;

  if (toggleCamera)
  {
    //render.cameraChanged = true;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.pitch(yoffset);
    camera.yaw(xoffset);

    glm::vec2 windowRes = glm::vec2(render.height*render.aspect, render.height);

    SDL_WarpMouseGlobal(render.windowPosition.x + windowRes.x / 2,
      render.windowPosition.y + windowRes.y / 2);

  }
}
