#include <pch.h>



InputManager::InputManager()
{
  name = "Input Manager";
}

InputManager::~InputManager()
{

}

void InputManager::Update(float dt)
{
  auto& render = pattern::get<Render>();
  auto& input = pattern::get<InputManager>();
  auto& gui = pattern::get<GUI>();
  auto& reader = pattern::get<ObjectReader>();
  auto& camera = render.currentCamera;


  int windowX = 0;
  int windowY = 0;
  SDL_GetWindowPosition(render.gWindow, &windowX, &windowY);
  render.position = glm::vec2(windowX, windowY);
  
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
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_A)
      {
        camera.leftRight(speed * 2.0f);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_S)
      {
        camera.forward(-speed * 2.0f);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_D)
      {
        camera.leftRight(-speed * 2.0f);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
      {
        camera.upDown(-speed * 2.0f);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_LCTRL)
      {
        camera.upDown(speed * 2.0f);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_Q)
      {
        camera.roll(speed * 0.8f);
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_E)
      {
        camera.roll(-speed * 0.8f);
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
      && event.button.type == SDL_MOUSEBUTTONDOWN)
      {
        if (toggleCamera == false)
        {
          toggleCamera = true;
          SDL_SetRelativeMouseMode(SDL_bool(true));
        }
        else if (toggleCamera == true)
        {
          toggleCamera = false;
          SDL_SetRelativeMouseMode(SDL_bool(false));

          glm::vec2 windowPos = render.position;
          glm::vec2 windowRes = glm::vec2(render.height*render.aspect, render.height);

          SDL_WarpMouseGlobal(windowPos.x + windowRes.x / 2,
            windowPos.y + windowRes.y / 2);
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
        // Pull up code here!
      }
      else if (event.wheel.y < 0) // scroll down
      {
        camera.zoom(1.1f);
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
      mousePosition = glm::vec2{ event.motion.x, event.motion.y };
    }
    break;
    }
  }


  if (firstMouse)
  {
    lastX = mousePosition.x;
    lastY = mousePosition.y;
    firstMouse = false;
  }

  float xoffset = mousePosition.x - lastX;
  float yoffset = lastY - mousePosition.y;
  lastX = mousePosition.x;
  lastY = mousePosition.y;

  if (toggleCamera)
  {


    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw = xoffset;
    pitch = yoffset;
    /*
    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
    glm::vec3 back;
    back.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    back.y = sin(glm::radians(pitch));
    back.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    //mainCamera.cameraRight
    //mainCamera.cameraUp
    camera.right_vector = -glm::normalize(back);
    camera.back_vector =
      -glm::normalize(glm::cross(camera.up_vector, camera.back_vector));
    */
    camera.pitch(pitch);
    camera.yaw(-yaw);


    //utils::update_camera("mainCamera", mainCamera);
    //utils::set_camera(registryKey);

    glm::vec2 windowPos = render.position;
    glm::vec2 windowRes = glm::vec2(render.height*render.aspect, render.height);

    SDL_WarpMouseGlobal(windowPos.x + windowRes.x / 2,
      windowPos.y + windowRes.y / 2);
    /*
    SDL_WarpMouseInWindow(
      pattern::get<utils::Window>().get_window_ptr(),
      windowRes.x / 2, windowRes.y / 2);
     */
  }
}
