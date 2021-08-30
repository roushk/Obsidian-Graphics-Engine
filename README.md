# Obsidian-Graphics-Engine

The Obsidian Graphics Engine is a C++ OpenGL PBR, BRDF, IBL and is is a academic/personal project where I test and implement lighting techniques. 
The base architecture of the engine is simple and designed to make implementings different graphics techniques easy.
There is a GUI creates using ImGUI to edit the scene and view parts of the rendering pipeline such as various intermediate buffers like depth and normals. 

Specifically the engine is a C++ custom engine created with OpenGL for the graphics. The engine is a deferred rendering pipeline with Physically Based Renering utilizing the Bidirectional Reflectance Distribution Function for realistic materials and lighting alongside Image Base Lighting for the ambient lightiong in the scene. In addition to High Dynamic Range and tone mapping with other advanced rendering techniques such as Normal and Parallax Mapping, Screen Space Ambient Occlusion, and Moment Shadow Mapping.

There are still mistakes and issues to be fixed, such as the seams of the UV maps looking weird, but its a work in progress. 

### Final Scene
![Finalscene](https://github.com/roushk/Obsidian-Graphics-Engine/blob/main/screenshots/FinalScene.PNG?raw=true)

### Normals Intermediate
![NormalsInt](https://github.com/roushk/Obsidian-Graphics-Engine/blob/main/screenshots/NormalsInt.PNG?raw=true)

### SSAO Intermediate
![SSAOInt](https://github.com/roushk/Obsidian-Graphics-Engine/blob/main/screenshots/SSAOInt.PNG?raw=true)

### Single source light with shadows, and SSAO combined
![Single source light with shadows, and SSAO combined](https://github.com/roushk/Obsidian-Graphics-Engine/blob/main/screenshots/BasicLightingandMaterials.PNG?raw=true)

### Metal with No Normal Mapping 
![MetalNoNormalMapping](https://github.com/roushk/Obsidian-Graphics-Engine/blob/main/screenshots/MetalNoNormalMapping.PNG?raw=true)

### Metal with Normal Mapping 
![MetalNormalMapping](https://github.com/roushk/Obsidian-Graphics-Engine/blob/main/screenshots/MetalNormalMapping.PNG?raw=true)

### Metal with Parallax Mapping
![MetalParallaxMapping](https://github.com/roushk/Obsidian-Graphics-Engine/blob/main/screenshots/MetalParallaxMapping.PNG?raw=true)

