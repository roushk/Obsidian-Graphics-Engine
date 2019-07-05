/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: LightShader.frag
Purpose: Shader for lights face color 
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_2
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 7/2/18
End Header --------------------------------------------------------*/

#version 430

//simple pass through
in VS_OUT
{
    vec4 position;
} fs_in;

//out vec4 color;

//layout (location = 0) out vec4 depth;  //COLOR_ATTACH 0 + emissive x

void main()
{
  //gl_FragData[0] = fs_in.position;  //(only the w coordinate will be used)
}