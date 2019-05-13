/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: shader.hpp
Purpose: header to Load shaders from file
Language: C++ MSVC
Platform: VS 141, OpenGL 4.3 compatabile device driver, Win10
Author: Pushpak Karnick

End Header --------------------------------------------------------*/
#ifndef SHADER_HPP
#define SHADER_HPP

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

GLuint LoadPipeline(const char * vertex_file_path,const char * fragment_file_path, GLuint * programIDs );

#endif
