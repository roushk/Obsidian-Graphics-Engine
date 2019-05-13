#include "BoundingEllipsoidPCA.h"
#include "Object.h"

BoundingEllipsoidPCA::BoundingEllipsoidPCA(Object& obj)
{
  center = obj.modelMatrix * glm::vec4(obj.center, 1);  //want BV in world space
}

void BoundingEllipsoidPCA::Update(float dt, glm::mat4 transform)
{
}
