#include "OrientedBoundingBoxPCA.h"
#include "Object.h"

OrientedBoundingBoxPCA::OrientedBoundingBoxPCA(Object& obj)
{
  center = obj.modelMatrix * glm::vec4(obj.center, 1);  //want BV in world space
}

void OrientedBoundingBoxPCA::Update(float dt, glm::mat4 transform)
{
}
