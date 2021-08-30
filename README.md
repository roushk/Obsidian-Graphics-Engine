# Obsidian-Graphics-Engine

The Obsidian Graphics Engine is a C++ OpenGL Physically Based Renering utilizing the Bidirectional reflectance distribution function for realistic lighting alongside Image Base Lighting for the ambient light in the scene. This is a academic/personal project where I test and implement lighting techniques. 
The base architecture of the engine is simple and designed to make implementings different graphics techniques easy.
There is a GUI creates using ImGUI to edit the scene and view parts of the rendering pipeline such as various intermediate buffers like depth and normals. 

There are still mistakes and issues to be fixed, such as the seams of the UV maps looking weird, but its a work in progress. 

### Final Scene
![Finalscene](https://github.com/roushk/Obsidian-Graphics-Engine/blob/master/screenshots/FinalScene.PNG?raw=true)

### Normals Intermediate
![NormalsInt](https://github.com/roushk/Obsidian-Graphics-Engine/blob/master/screenshots/NormalsInt.PNG?raw=true)

### SSAO Intermediate
![SSAOInt](https://github.com/roushk/Obsidian-Graphics-Engine/blob/master/screenshots/SSAOInt.PNG?raw=true)

### Single source light with shadows, and SSAO combined
![Single source light with shadows, and SSAO combined](https://github.com/roushk/Obsidian-Graphics-Engine/blob/master/screenshots/BasicLightingandMaterials.PNG?raw=true)

### Metal with No Normal Mapping 
![MetalNoNormalMapping](https://github.com/roushk/Obsidian-Graphics-Engine/blob/master/screenshots/MetalNoNormalMapping.PNG?raw=true)

### Metal with Normal Mapping 
![MetalNormalMapping](https://github.com/roushk/Obsidian-Graphics-Engine/blob/master/screenshots/MetalNormalMapping.PNG?raw=true)

### Metal with Parallax Mapping
![MetalParallaxMapping](https://github.com/roushk/Obsidian-Graphics-Engine/blob/master/screenshots/MetalParallaxMapping.PNG?raw=true)

