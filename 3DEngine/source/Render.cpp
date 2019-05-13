#define PI 3.14159265359f
#define GLM_ENABLE_EXPERIMENTAL

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <chrono>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
#include "Render.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Projection.h"
#include "SOIL/SOIL.h"
#include "shader.hpp"

#include "Wireframe.h"
#include "Octree.h"
#include "BSPTree.h"

Render::Render()
{
  //mvTransform = glm::mat4(1.0);
  //vTransform = glm::mat4(1.0);
}

Render::~Render()
{
  // Cleanup VBO
  glDeleteBuffers(5, vertexbuffers);
  glDeleteBuffers(2, uboHandle);

  //glDeleteVertexArrays(1, &VertexArrayID);
  glDeleteProgram(programIDs[0]);
  glDeleteProgram(programIDs[1]);
  glDeleteProgram(programIDs[2]);
  glDeleteProgram(programIDs[3]);
  glDeleteProgram(programIDs[4]);
  glDeleteProgram(programIDs[5]);

  glDeleteTextures(1, &specularMaterialID);
  glDeleteTextures(1, &diffuseMaterialID);

  glDeleteBuffers(6, FrameBuffers);
  glDeleteTextures(6, RenderedTextures);
  glDeleteBuffers(6, FBODepthBuffers);

  glDeleteBuffers(1, &Gbuffer);
  glDeleteTextures(6, GBufferTexture);
  glDeleteBuffers(1, &GBufferDepthBuffer);


  // Close OpenGL window and terminate GLFW
  glfwTerminate();
}

void Render::GenGBuffer()
{
  glGenFramebuffers(1, &Gbuffer);
  glGenTextures(6, GBufferTexture);
  glGenRenderbuffers(1, &GBufferDepthBuffer);
}

std::string Render::GetSetting()
{
  return renderSetting;
}

void Render::SetModelOffset(float x, float y, float z, float scale_)
{
  modelTransform = glm::mat4(1.0f);
  modelTransform = translate(modelTransform, vec3(x, y, z));

  modelTransform = scale(modelTransform, vec3(scale_));
}

void Render::SetModelOffset(vec4 pos, float scale_)
{
  modelTransform = glm::mat4(1.0f);
  modelTransform = translate(modelTransform, vec3(pos));

  modelTransform = scale(modelTransform, vec3(scale_));
}

void Render::SetModelOffset(vec3 pos, float scale_)
{
  modelTransform = glm::mat4(1.0f);
  modelTransform = translate(modelTransform, pos);

  modelTransform = scale(modelTransform, vec3(scale_));
}

void Render::LoadMaterial(Material materialSpec, Material materialDiff)
{
  glUseProgram(programID);


  glGenTextures(1, &diffuseMaterialID);
  glBindTexture(GL_TEXTURE_2D, diffuseMaterialID);
  //pixels are in RGB format as floats
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, materialDiff.width,
               materialDiff.height, 0, GL_RGB, GL_FLOAT, materialDiff.pixels.data());


  glGenTextures(1, &specularMaterialID);
  glBindTexture(GL_TEXTURE_2D, specularMaterialID);
  //pixels are in RGB format as floats
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, materialSpec.width,
               materialSpec.height, 0, GL_RGB, GL_FLOAT, materialSpec.pixels.data());
  //glEnable(GL_TEXTURE_2D);
  //glDisable(GL_TEXTURE_2D);
  //glTexGenf()
}

void Render::LoadSkybox(std::vector<std::string>& skyboxNames)
{
  for (unsigned i = 0; i < skyboxNames.size(); ++i)
  {
    if (i == 2 || i == 3) //y+ or y-
      skyboxTextureID[i] = SOIL_load_OGL_texture(skyboxNames[i].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                                                 SOIL_FLAG_TEXTURE_REPEATS);
    else
      skyboxTextureID[i] = SOIL_load_OGL_texture(skyboxNames[i].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                                                 SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y);
    glBindTexture(GL_TEXTURE_2D, skyboxTextureID[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
}

void Render::GenFrameBuffers()
{
  glGenFramebuffers(6, FrameBuffers);
  glGenTextures(6, RenderedTextures);
  glGenRenderbuffers(6, FBODepthBuffers);
}

void Render:: BindAndCreateGBuffers()
{
  GLuint numBuffers = 5;
  glBindFramebuffer(GL_FRAMEBUFFER, Gbuffer);
  int width = height * aspect;

  //Per GBuffer
  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GBufferTexture[0], 0);
  //End Per GBuffer

  //Per GBuffer
  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GBufferTexture[1], 0);
  //End Per GBuffer

  //Per GBuffer
  glActiveTexture(GL_TEXTURE10);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[2]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GBufferTexture[2], 0);
  //End Per GBuffer

  //Per GBuffer
  glActiveTexture(GL_TEXTURE11);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[3]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GBufferTexture[3], 0);
  //End Per GBuffer

  //Per GBuffer
  glActiveTexture(GL_TEXTURE12);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[4]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GBufferTexture[4], 0);
  //End Per GBuffer


  glDrawBuffers(numBuffers, DrawGBuffers);

  //Bind depth buffer
  glBindRenderbuffer(GL_RENDERBUFFER, GBufferDepthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, GBufferDepthBuffer);

  //check for completeness
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "GBuffer Bind and Create Failed" << std::endl;
  }
}

void Render::BindGBufferTextures()
{
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[0]);
  glUniform1i(glGetUniformLocation(programID, "gPositionMap"), 2);
  glBindSampler(GL_TEXTURE2, glGetUniformLocation(programID, "gPositionMap"));

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[1]);
  glUniform1i(glGetUniformLocation(programID, "gNormalMap"), 3);
  glBindSampler(GL_TEXTURE3, glGetUniformLocation(programID, "gNormalMap"));

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[2]);
  glUniform1i(glGetUniformLocation(programID, "gDiffuseMap"), 4);
  glBindSampler(GL_TEXTURE4, glGetUniformLocation(programID, "gDiffuseMap"));

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[3]);
  glUniform1i(glGetUniformLocation(programID, "gSpecularMap"), 5);
  glBindSampler(GL_TEXTURE5, glGetUniformLocation(programID, "gSpecularMap"));

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[4]);
  glUniform1i(glGetUniformLocation(programID, "gAmbientMap"), 6);
  glBindSampler(GL_TEXTURE6, glGetUniformLocation(programID, "gAmbientMap"));
  
}

void Render::BindGBuffer()
{
  //bind frame buffer i 
  glBindFramebuffer(GL_FRAMEBUFFER, Gbuffer);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "G buffer bind Failed" << std::endl;
  }

  glViewport(0, 0, height * aspect, height);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //draw scene
}

void Render::BindAndCreateFrameBuffers(int i)
{
  glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffers[i]);
  glActiveTexture(GL_TEXTURE8 + i);

  glBindTexture(GL_TEXTURE_2D, RenderedTextures[i]);
  int width = height * aspect;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, width, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindRenderbuffer(GL_RENDERBUFFER, FBODepthBuffers[i]);

  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, width);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, FBODepthBuffers[i]);

  glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, RenderedTextures[i], 0);

  DrawBuffers = {GL_COLOR_ATTACHMENT0};

  GLuint numBuffers = 1;
  glDrawBuffers(numBuffers, &DrawBuffers);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Frame buffer " << i << " Failed" << std::endl;
  }
}

void Render::BindFrameBufferToRenderTo(unsigned i)
{
  //bind frame buffer i 
  glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffers[i]);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Frame buffer " << i << " Failed" << std::endl;
  }

  glViewport(0, 0, height * aspect, height * aspect);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //draw scene
}

void Render::BindFrameBufferTextures()
{
  //Loads X
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[0]);
  glUniform1i(glGetUniformLocation(programID, "positiveX"), 2);
  glBindSampler(GL_TEXTURE2, glGetUniformLocation(programID, "positiveX"));

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[1]);
  glUniform1i(glGetUniformLocation(programID, "negativeX"), 3);
  glBindSampler(GL_TEXTURE3, glGetUniformLocation(programID, "negativeX"));

  //Loads Y
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[2]);
  glUniform1i(glGetUniformLocation(programID, "positiveY"), 4);
  glBindSampler(GL_TEXTURE4, glGetUniformLocation(programID, "positiveY"));

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[3]);
  glUniform1i(glGetUniformLocation(programID, "negativeY"), 5);
  glBindSampler(GL_TEXTURE5, glGetUniformLocation(programID, "negativeY"));

  //Loads Z
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[4]);
  glUniform1i(glGetUniformLocation(programID, "positiveZ"), 6);
  glBindSampler(GL_TEXTURE6, glGetUniformLocation(programID, "positiveZ"));

  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[5]);
  glUniform1i(glGetUniformLocation(programID, "negativeZ"), 7);
  glBindSampler(GL_TEXTURE7, glGetUniformLocation(programID, "negativeZ"));
}

void Render::BindDefaultFrameBuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, height * aspect, height);
}

void Render::BindSkybox()
{
  glUseProgram(programID);

  //Loads X
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[0]);
  glUniform1i(glGetUniformLocation(programID, "positiveX"), 2);
  glBindSampler(GL_TEXTURE2, glGetUniformLocation(programID, "positiveX"));

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[1]);
  glUniform1i(glGetUniformLocation(programID, "negativeX"), 3);
  glBindSampler(GL_TEXTURE3, glGetUniformLocation(programID, "negativeX"));

  //Loads Y
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[2]);
  glUniform1i(glGetUniformLocation(programID, "positiveY"), 4);
  glBindSampler(GL_TEXTURE4, glGetUniformLocation(programID, "positiveY"));


  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[3]);
  glUniform1i(glGetUniformLocation(programID, "negativeY"), 5);
  glBindSampler(GL_TEXTURE5, glGetUniformLocation(programID, "negativeY"));


  //Loads Z
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[4]);
  glUniform1i(glGetUniformLocation(programID, "positiveZ"), 6);
  glBindSampler(GL_TEXTURE6, glGetUniformLocation(programID, "positiveZ"));


  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[5]);
  glUniform1i(glGetUniformLocation(programID, "negativeZ"), 7);
  glBindSampler(GL_TEXTURE7, glGetUniformLocation(programID, "negativeZ"));
}

void Render::BindMaterials(SceneLighting& lighting)
{
  //glUseProgram(programID);

  //loads diffuse
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, specularMaterialID);
  glUniform1i(glGetUniformLocation(programID, "Kdiffuse"), 0);

  //loads specular
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, diffuseMaterialID);
  glUniform1i(glGetUniformLocation(programID, "Kspecular"), 1);

  //loads ambient and emissive from GUI
  glUniform3fv(glGetUniformLocation(programID, "Kambient"), 1,
               glm::value_ptr(lighting.global.KMaterialambient)); //works
  glUniform3fv(glGetUniformLocation(programID, "Kemissive"), 1,
               glm::value_ptr(lighting.global.KMaterialemissive)); //works
}

void Render::LoadModel(Wireframe& object)
{
  glUseProgram(programID);
  glBindVertexArray(object.VAO);
  glUniform4fv(glGetUniformLocation(programID, "color"), 1, glm::value_ptr(object.color));

}


void Render::LoadModel(Object& object)
{
  glUseProgram(programID);
  glBindVertexArray(object.VAO);

 /*
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
  glBufferData(GL_ARRAY_BUFFER, object.verts.size() * sizeof(vec3), object.verts.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[1]);
  glBufferData(GL_ARRAY_BUFFER, object.vertexNormals.size() * sizeof(vec3), object.vertexNormals.data(),
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexbuffers[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.faces.size() * sizeof(uvec3), object.faces.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[3]);
  glBufferData(GL_ARRAY_BUFFER, object.faceNormals.size() * sizeof(vec3), object.faceNormals.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[4]);

  //selcts UV model to use for texturing
  if (currentUVModel == uvmCylindrical)
    glBufferData(GL_ARRAY_BUFFER, object.uvCylindrical.size() * sizeof(vec2), object.uvCylindrical.data(),
                 GL_STATIC_DRAW);
  else if (currentUVModel == uvmSphereical)
    glBufferData(GL_ARRAY_BUFFER, object.uvSpherical.size() * sizeof(vec2), object.uvSpherical.data(), GL_STATIC_DRAW);
  else if (currentUVModel == uvm6Planar)
    glBufferData(GL_ARRAY_BUFFER, object.uvPlanar.size() * sizeof(vec2), object.uvPlanar.data(), GL_STATIC_DRAW);
    */
  }

void Render::ClearScreen()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::CreateShaders()
{
  //programIDs[ssBlinnLighting] = LoadShaders("Shaders/BlinnLighting.vert", "Shaders/BlinnLighting.frag");
  //programIDs[ssPhongLighting] = LoadShaders("Shaders/PhongLighting.vert", "Shaders/PhongLighting.frag");
  //programIDs[ssPhongShading] = LoadShaders("Shaders/PhongShading.vert", "Shaders/PhongShading.frag");
  programIDs[ssLightShader] = LoadShaders("Shaders/LightShader.vert", "Shaders/LightShader.frag");
  //programIDs[ssSkyboxShader] = LoadShaders("Shaders/SkyboxShader.vert", "Shaders/SkyboxShader.frag");
  //programIDs[ssPhongShadingAndReflection] = LoadShaders("Shaders/PhongShadingAndReflection.vert",
  //                                                      "Shaders/PhongShadingAndReflection.frag");
  //programIDs[ssReflectionMap] = LoadShaders("Shaders/ReflectionMap.vert", "Shaders/ReflectionMap.frag");
  //programIDs[ssRefractionMap] = LoadShaders("Shaders/RefractionMap.vert", "Shaders/RefractionMap.frag");
  //programIDs[ssFrenselApprox] = LoadShaders("Shaders/Frensel.vert", "Shaders/Frensel.frag");
  //programIDs[ssPhongShadingAndFrensel] = LoadShaders("Shaders/PhongShadingAndFrensel.vert",
  //                                                   "Shaders/PhongShadingAndFrensel.frag");
  programIDs[ssWireframe] = LoadShaders("Shaders/WireframeShader.vert", "Shaders/WireframeShader.frag");

  programIDs[ssDeferredGeometry] = LoadShaders("Shaders/DeferredGeometry.vert", "Shaders/DeferredGeometry.frag");
  programIDs[ssDeferredRendering] = LoadShaders("Shaders/DeferredRendering.vert", "Shaders/DeferredRendering.frag");

  programIDs[ssPhongShadingDeferred] = LoadShaders("Shaders/DeferredRendering.vert", "Shaders/PhongShadingDeferred.frag");
  programID = programIDs[ssLightShader];
}

void Render::CreateBuffers()
{
  glGenBuffers(5, vertexbuffers);
}

void Render::LoadDiffuseForLight(Light& light)
{
  glUniform3fv(glGetUniformLocation(programID, "diffuse"), 1, glm::value_ptr(light.diffuse));
}
void Render::LoadDiffuseForLight(glm::vec4& light)
{
  glUniform3fv(glGetUniformLocation(programID, "diffuse"), 1, glm::value_ptr(light));
}


void Render::SetCurrentShader(int shader)
{
  programID = programIDs[shader];
  currentShader = shader;
  glUseProgram(programID);
}

void Render::BindLightScene(SceneLighting& lighting)
{
  //****************************************//
  //load once

  //load global data

  GLuint uboIndex2 = glGetUniformBlockIndex(programID, "Global");
  if (uboIndex2 != GL_INVALID_INDEX)
  {
    GLint uboSize2;
    //get block size
    glGetActiveUniformBlockiv(programID, uboIndex2, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize2);
    const GLchar* names2[] = {
      //needed for all light types

      "Global.AttParam",
      "Global.FogColor",
      "Global.Iglobal",
      "Global.near",
      "Global.far",
      "Global.Kglobal",
      "Global.activeLights"
    };
    GLuint indices2[7];
    glGetUniformIndices(programID, 7, names2, indices2);

    GLint offset2[7];
    glGetActiveUniformsiv(programID, 7, indices2, GL_UNIFORM_OFFSET, offset2);

    glUniformBlockBinding(programID, uboIndex2, bpGlobal);

    lighting.global.near = nearPlane;
    lighting.global.far = farPlane;

    //allocate block size of entire array
    if (uboBuffer[1] == NULL)
      uboBuffer[1] = (GLubyte *)malloc(uboSize2);


    memcpy(uboBuffer[1] + offset2[0], glm::value_ptr(lighting.global.AttenParam), sizeof(vec4));
    memcpy(uboBuffer[1] + offset2[1], glm::value_ptr(lighting.global.FogColor), sizeof(vec4));
    memcpy(uboBuffer[1] + offset2[2], glm::value_ptr(lighting.global.Iglobal), sizeof(vec4));

    memcpy(uboBuffer[1] + offset2[3], &lighting.global.near, sizeof(float));
    memcpy(uboBuffer[1] + offset2[4], &lighting.global.far, sizeof(float));
    memcpy(uboBuffer[1] + offset2[5], &lighting.global.Kglobal, sizeof(float));

    memcpy(uboBuffer[1] + offset2[6], &lighting.activeLights, sizeof(int));

    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle[1]);
    glBufferData(GL_UNIFORM_BUFFER, uboSize2, uboBuffer[1],
                 GL_STATIC_DRAW);

    // Step 7. Bind the base of the buffer
    glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex2, uboHandle[1]);
  }


  std::vector<LightData> lights;

  //pushes light data onto individual arrays from light structs
  for (int i = 0; i < lighting.activeLights; ++i)
  {
    LightData light = lighting.lights[i]; //intentional object slicing to just get data
    lights.push_back(light);
  }
  for (int i = lighting.activeLights; i < lighting.maxLights; ++i)
  {
    LightData light; //intentional object slicing to just get data
    lights.push_back(light);
  }
  //****************************************//
  //load array of lights Global, Material
  //get block index
  GLuint uboIndex = glGetUniformBlockIndex(programID, "LightBlock"); //uboIndex = block_index
  if (uboIndex != GL_INVALID_INDEX)
  {
    GLint uboSize; //is max size (aka 8 lights)
    //get block size
    glGetActiveUniformBlockiv(programID, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);

    glUniformBlockBinding(programID, uboIndex, bpLights);

    if (uboSize != sizeof(LightData) * lighting.maxLights)
      std::cout << "UBO and Light Data Struct size mismatch" << std::endl;

    int size = sizeof(LightData);

    memcpy(uboBuffer[0], lights.data(), uboSize);

    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle[0]);
    glBufferData(GL_UNIFORM_BUFFER, uboSize, uboBuffer[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex, uboHandle[0]);
  }

  // Load camera
  //glm::vec3 cameraPos(0,0,-2);// = camera.eye();
}

void Render::CreateUBOBufferObjects()
{
  glGenBuffers(2, uboHandle);
  uboBuffer[0] = (GLubyte*)malloc(sizeof(LightData) * 8);
}

void Render::CleanUpUBOBuffers(int activeLights)
{
  free(uboBuffer[1]);
  if (activeLights > 0)
    free(uboBuffer[0]);
}

void Render::SetCurrentDebugTexture(unsigned debugTextureNext)
{
  glUniform1ui(glGetUniformLocation(programID, "debugTexture"), debugTextureNext);

}

bool Render::InitRender()
{
  // Initialise GLFW
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  if (!glfwInit())
  {
    std::cout << "Failed to initialize GLFW" << std::endl;
    getchar();
    return false;
  }

  // Open a window and create its OpenGL context 1024, 768
  window = glfwCreateWindow(1024, 768, "CS350: Assignment 1", NULL, NULL);
  if (window == NULL)
  {
    std::cout <<
      "Failed to open GLFW window. If you have an Intel GPU, they are not 4.0 compatible. Try the 2.1 version of the tutorials.\n" << std::endl;
    getchar();
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK)
  {
    std::cout << "Failed to initialize GLEW" << std::endl;
    getchar();
    glfwTerminate();
    return false;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  Update();
}

void Render::CopyDepthBuffer()
{
  int width = height * aspect;
  glBindFramebuffer(GL_READ_FRAMEBUFFER, Gbuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
  glBlitFramebuffer(
    0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST
  );
}

void Render::BufferRefractionData()
{
  glUniform1f(glGetUniformLocation(programID, "highlightTightness"), highlightTightness);
  glUniform1f(glGetUniformLocation(programID, "transCoeff"), transmissionCoefficient);
}

void Render::Update()
{
  glfwPollEvents();
  int width;
  glfwGetWindowSize(window, &width, &height);
  glViewport(0, 0, width, height);
  float oldAspect = aspect;
  aspect = float(width) / float(height);
  if (aspect != oldAspect)
  {
    cameraBase = Camera(vec4{0, 0, 5, 0}, vec4{0, 0, -1, 1}, vec4{0, 1, 0, 1}, PI / 2.0f, aspect, nearPlane, farPlane);
  }

  Camera newCameras[6]
  {
    //same order as textures posx, negx, posy, negy, posz, negz
    //view vec os other dir b/c is lookAt
    //i have NO IDEA why SQRT2OVER2 works for the aspect ratio its not even 768/1024 its like 768/1086                      //orieng y up, x up
    Camera(vec4{objectPos, 0}, vec4{1, 0, 0, 1}, vec4{0, 1, 0, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane), //orient y -z
    Camera(vec4{objectPos, 0}, vec4{-1, 0, 0, 1}, vec4{0, 1, 0, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane), //orient y z
    Camera(vec4{objectPos, 0}, vec4{0, 1, 0, 1}, vec4{0, 0, -1, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane), //orient -z x
    Camera(vec4{objectPos, 0}, vec4{0, -1, 0, 1}, vec4{0, 0, 1, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane), //orient z x  //need to flip
    Camera(vec4{objectPos, 0}, vec4{0, 0, 1, 1}, vec4{0, 1, 0, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane), //orient y x
    Camera(vec4{objectPos, 0}, vec4{0, 0, -1, 1}, vec4{0, 1, 0, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane) //orient y -x
  };
  for (unsigned i = 0; i < 6; ++i)
  {
    cameras[i] = newCameras[i];
  }
}


void Render::BindModelBuffer()
{
  glUseProgram(programID);

  // Use our shader


  // 1st attribute buffer : vertex positions (model space)
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
  glVertexAttribPointer(
    0, // attribute 0. MUST match the layout in the shader.
    3, // size
    GL_FLOAT, // type
    GL_FALSE, // normalized?
    0, // stride
    (void*)0 // array buffer offset
  );

  if (currentShader != ssLightShader)
  {
    // 2nd attribute buffer : FACE normals (model space)
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[1]);
    glVertexAttribPointer(
      1, // attribute 0. MUST match the layout in the shader.
      3, // size
      GL_FLOAT, // type
      GL_FALSE, // normalized?
      0, // stride
      (void*)0 // array buffer offset
    );

    // 3rd attribute buffer : uv coords
  }
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[4]);
  glVertexAttribPointer(
    2, // attribute 2. MUST match the layout in the shader.
    2, // size
    GL_FLOAT, // type
    GL_FALSE, // normalized?
    0, // stride
    (void*)0 // array buffer offset
  );
}

void Render::UpdateCamera(float dt)
{
  vec3 pos{0, 1, 0}; //rotate around z/origin vector

  eyePos = rotate(rotateRate * dt, pos) * eyePos;
  lookAt = rotate(rotateRate * dt, pos) * lookAt;

  inverseCamRotate = rotate(rotateRate * dt, pos);
}

void Render::Draw(OctreeNode* object)
{
  glUseProgram(programID);

  if(object->verts.size() > 0)
  {
      
    glUniform4fv(glGetUniformLocation(programID, "color"), 1, glm::value_ptr(object->color));

    // Uniform transformation (vertex shader)

    projectionMatrix = cameraToNDC(currentCamera);
    if (flipX == true)
      projectionMatrix = scale(projectionMatrix, vec3(-1, 1, 1));
    viewMatrix = worldToCamera(currentCamera);

    glUniformMatrix4fv(glGetUniformLocation(programID, "projectionMatrix"), 1, GL_FALSE,
      glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    //already in world coords

  }

  Draw_rec(object);
}

void Render::Draw(BSPNode* object)
{
  glUseProgram(programID);

  if (object->verts.size() > 0)
  {

    glUniform4fv(glGetUniformLocation(programID, "color"), 1, glm::value_ptr(object->color));

    // Uniform transformation (vertex shader)

    projectionMatrix = cameraToNDC(currentCamera);
    if (flipX == true)
      projectionMatrix = scale(projectionMatrix, vec3(-1, 1, 1));
    viewMatrix = worldToCamera(currentCamera);

    glUniformMatrix4fv(glGetUniformLocation(programID, "projectionMatrix"), 1, GL_FALSE,
      glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    //already in world coords

  }

  Draw_rec(object);
}

void Render::Draw_rec(BSPNode* object)
{
  glUseProgram(programID);

  if (object->verts.size() > 0)
  {
    glBindVertexArray(object->VAO);
    glUniform4fv(glGetUniformLocation(programID, "color"), 1, glm::value_ptr(object->color));
    glDrawArrays(GL_TRIANGLES, 0, object->verts.size());
  }
  //recursively draw all other children
  for (auto* child : object->children)
  {
    Draw_rec(child);
  }
}

void Render::Draw_rec(OctreeNode* object)
{
  glUseProgram(programID);

  if (object->verts.size() > 0)
  {
    glBindVertexArray(object->VAO);
    glUniform4fv(glGetUniformLocation(programID, "color"), 1, glm::value_ptr(object->color));
    glDrawArrays(GL_TRIANGLES, 0, object->verts.size());
  }
  //recursively draw all other children
  for (auto* child : object->children)
  {
    Draw_rec(child);
  }
}

void Render::Draw_Wireframe(OctreeNode* object)
{
  glUseProgram(programID);

  glUniform4fv(glGetUniformLocation(programID, "color"), 1, glm::value_ptr(object->colorWireframe));
 

  projectionMatrix = cameraToNDC(currentCamera);
  if (flipX == true)
    projectionMatrix = scale(projectionMatrix, vec3(-1, 1, 1));
  viewMatrix = worldToCamera(currentCamera);

  glUniformMatrix4fv(glGetUniformLocation(programID, "projectionMatrix"), 1, GL_FALSE,
    glm::value_ptr(projectionMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
  //already in world coords

  //recursively draw all other children
  Draw_Wireframe_rec(object);

}

void Render::Draw_Wireframe_rec(OctreeNode* object)
{
  //draw wireframe
  glBindVertexArray(object->wireframe_VAO);
  glUniform4fv(glGetUniformLocation(programID, "color"), 1, glm::value_ptr(object->colorWireframe));

  glDrawArrays(GL_LINES, 0, object->wireframe_verts.size());

  //recursively draw all other children
  for (auto* child : object->children)
  {
    Draw_Wireframe_rec(child);
  }
}

void Render::Draw(Wireframe& object)
{
  glUseProgram(programID);

  // Uniform transformation (vertex shader)

  projectionMatrix = cameraToNDC(currentCamera);
  if (flipX == true)
    projectionMatrix = scale(projectionMatrix, vec3(-1, 1, 1));
  viewMatrix = worldToCamera(currentCamera);

  glUniformMatrix4fv(glGetUniformLocation(programID, "projectionMatrix"), 1, GL_FALSE,
    glm::value_ptr(projectionMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
  //already in world coords
  //glUniformMatrix4fv(glGetUniformLocation(programID, "modelTransform"), 1, GL_FALSE, glm::value_ptr(modelTransform));

  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  //2 not 3 because lines not faces
  glDrawElements(GL_LINES, object.faces.size() * 2, GL_UNSIGNED_INT, 0);
}

void Render::Draw(Object& object)
{
  glUseProgram(programID);

  // Uniform transformation (vertex shader)

  projectionMatrix = cameraToNDC(currentCamera);
  if (flipX == true)
    projectionMatrix = scale(projectionMatrix, vec3(-1, 1, 1));
  viewMatrix = worldToCamera(currentCamera);

  glUniformMatrix4fv(glGetUniformLocation(programID, "projectionMatrix"), 1, GL_FALSE,
                     glm::value_ptr(projectionMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "modelTransform"), 1, GL_FALSE, glm::value_ptr(modelTransform));
  glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(object.modelMatrix));

  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glDrawElements(GL_TRIANGLES, object.faces.size() * 3, GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle

  /*
  if (setting == rsFaceNormal)
  {
    //glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[1]);
    glVertexAttribPointer(
      0, // attribute 0. MUST match the layout in the shader.
      3, // size
      GL_FLOAT, // type
      GL_FALSE, // normalized?
      0, // stride
      (void*)0 // array buffer offset
    );
    glDrawElements(GL_LINES, object.faces.size() * 3, GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
    //rebind old buffer
    //BindModelBuffer();
    glEnableVertexAttribArray(1);
  }
  else if (setting == rsVertNormal)
  {
    //glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[3]);
    glVertexAttribPointer(
      0, // attribute 0. MUST match the layout in the shader.
      3, // size
      GL_FLOAT, // type
      GL_FALSE, // normalized?
      0, // stride
      (void*)0 // array buffer offset
    );

    glDrawElements(GL_LINES, object.faces.size() * 3, GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
    //rebind old buffer
    //BindModelBuffer();
    glEnableVertexAttribArray(1);
  }
    */
}

void Render::EndDrawing()
{
  //glDisableVertexAttribArray(0);
  //glDisableVertexAttribArray(1);
  //glDisableVertexAttribArray(2);
  // Swap buffers
  glfwSwapBuffers(window);
}

void Render::ClearColor(vec4 color)
{
  glClearColor(color.x, color.y, color.z, color.w);
}

void Render::SetTitle(std::string object)
{
  std::string title = "Rendering: ";
  title += object;
  title += " Setting: ";
  title += renderSetting;
  glfwSetWindowTitle(window, title.c_str());
}

void Render::AddLight(Light light)
{
  lights.push_back(light);
}

void Render::ClearLights()
{
  lights.clear();
}

void Render::SetObjPositionForCamera(vec3 pos)
{
  objectPos = pos;
}

void Render::SetCurrentCamera(int cam)
{
  if (cam == 6)
    currentCamera = cameraBase;
  else
    currentCamera = cameras[cam];

  glUniform3f(glGetUniformLocation(programID, "camera"),
    currentCamera.eye().x, currentCamera.eye().y, currentCamera.eye().z);
}

void Render::SetObjectShader(int shader)
{
  objectShader = shader;
}

void Render::LoadObjectShader()
{
  SetCurrentShader(objectShader);
};

