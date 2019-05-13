#include "BoundingVolume.h"
#include "Object.h"

BoundingVolume::BoundingVolume(Object& obj)
{
  center = obj.modelMatrix * glm::vec4(obj.center, 1);  //want BV in world space
}
