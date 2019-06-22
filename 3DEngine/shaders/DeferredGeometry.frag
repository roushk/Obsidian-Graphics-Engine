/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: PhongShading.frag
Purpose: Implementation of phong shader
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_2
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 6/30/18
End Header --------------------------------------------------------*/

#version 430

in VS_OUT
{
  vec4 position;
  vec4 normal;
  vec2 texCoords;
} fs_in;

//corrolates DIRECTLY to the COLOR_ATTACHMENT
layout (location = 0) out vec4 ViewPosOut;  //COLOR_ATTACH 0 + emissive x
layout (location = 1) out vec4 NormalOut;   //COLOR_ATTACH 1 + emissive y
layout (location = 2) out vec4 DiffuseOut;  //COLOR_ATTACH 2 + emissive z
layout (location = 3) out vec4 SpecularOut; //COLOR_ATTACH 3 + shininess
layout (location = 4) out vec3 AmbientOut;  //COLOR_ATTACH 4
layout (location = 5) out vec3 DepthOut;    //DEPTH_ATTACH 1

uniform sampler2D Kdiffuse;
uniform sampler2D Kspecular;
uniform vec3 Kemissive; //Emissive: No texture, user specified value [RGB]
uniform vec3 Kambient;  //Ambient: No texture, user specified value [RGB]


void main()
{
  ViewPosOut.rgb = fs_in.position.xyz;
  
  DiffuseOut.rgb = texture(Kdiffuse, fs_in.texCoords).xyz;
  SpecularOut.rgb = texture(Kspecular, fs_in.texCoords).xyz;

  NormalOut.rgb = fs_in.normal.xyz;

  AmbientOut.rgb = Kambient;

  ViewPosOut.a = Kemissive.r;
  NormalOut.a = Kemissive.g;
  DiffuseOut.a = Kemissive.b;

  SpecularOut.a = SpecularOut.r * SpecularOut.r;  //ns = spec.r^2
  

  //TexCoordOut = vec3(fs_in.texCoords, 0.0); 

}
