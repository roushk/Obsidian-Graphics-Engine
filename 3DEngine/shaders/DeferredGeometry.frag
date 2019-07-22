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
  vec3 tangent;
  vec3 bitangent;
} fs_in;

//corrolates DIRECTLY to the COLOR_ATTACHMENT
layout (location = 0) out vec4 ViewPosOut;  //COLOR_ATTACH 0 + emissive x
layout (location = 1) out vec4 NormalOut;   //COLOR_ATTACH 1 + emissive y
layout (location = 2) out vec4 DiffuseOut;  //COLOR_ATTACH 2 + emissive z
layout (location = 3) out vec4 SpecularOut; //COLOR_ATTACH 3
layout (location = 4) out vec3 tangentOut;  //COLOR_ATTACH 4
layout (location = 5) out vec3 bitangentOut;  //COLOR_ATTACH 4

layout (location = 6) out vec3 DepthOut;    //DEPTH_ATTACH 1

uniform sampler2D Kdiffuse;
uniform sampler2D Kspecular;
uniform vec3 Kemissive; //Emissive: No texture, user specified value [RGB]
uniform vec3 Kambient;  //Ambient: No texture, user specified value [RGB]
uniform sampler2D normalMap;
uniform sampler2D heightMap;


mat3 createNormalMatrix(vec3 tangent, vec3 modelNormal)
{

  /*
  vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
  vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
  vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
  mat3 TBN = mat3(T, B, N)*/

  vec3 T = normalize(tangent);
  vec3 N = normalize(modelNormal);
 // T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  return mat3(T, B, N);
}

void main()
{
  ViewPosOut.xyz = fs_in.position.xyz;
  
  DiffuseOut.rgb = texture(Kdiffuse, fs_in.texCoords).xyz;
  SpecularOut.rgb = texture(Kspecular, fs_in.texCoords).xyz;


  mat3 TBN = createNormalMatrix(fs_in.tangent.rgb, fs_in.normal.rgb);
  vec3 norm = texture(normalMap, fs_in.texCoords).rgb;
  norm = normalize(norm * 2.0f - 1.0f);
  norm = normalize(TBN * norm);


  NormalOut.rgb = norm;

  tangentOut = normalize(fs_in.tangent);
  bitangentOut = normalize(fs_in.bitangent);
  
  //AmbientOut.rgb = Kambient;

  //ViewPosOut.a = Kemissive.r;
  //NormalOut.a = Kemissive.g;
  //DiffuseOut.a = Kemissive.b;
  

  //TexCoordOut = vec3(fs_in.texCoords, 0.0); 

}
