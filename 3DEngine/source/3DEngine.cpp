//*****************************************************//
// Coleman Jonas
// 3DEngine
//*****************************************************//

#include <pch.h>


int engine(int argc, char** argv)
{
  /*
  utils::initialize();

  {
    game::Game dave;
    dave.initialize();
    dave.run();
    dave.destroy();
  }

  utils::uninitialize();
  */
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
  std::cout << "test" << std::endl;
  try
  {
    return engine(argc, argv);
  }
  catch (const std::exception& e)
  {
    MessageBox(GetActiveWindow(), e.what(), "Error", MB_OK | MB_ICONERROR);
    return EXIT_FAILURE;
  }
  catch (const std::string& e)
  {
    MessageBox(GetActiveWindow(), e.c_str(), "Error", MB_OK | MB_ICONERROR);
    return EXIT_FAILURE;
  }
  catch (const char* e)
  {
    MessageBox(GetActiveWindow(), e, "Error", MB_OK | MB_ICONERROR);
    return EXIT_FAILURE;
  }
  catch (...)
  {
    MessageBox(GetActiveWindow(), "An error has occurred.", "Error", MB_OK | MB_ICONERROR);
    return EXIT_FAILURE;
  }
}

