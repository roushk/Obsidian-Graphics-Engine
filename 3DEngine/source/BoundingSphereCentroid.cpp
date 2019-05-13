#include "BoundingSphereCentroid.h"
#include "Object.h"

BoundingSphereCentroid::BoundingSphereCentroid(Object& _obj) : BoundingVolume(_obj), obj(_obj)
{
  center = obj.modelMatrix * glm::vec4(obj.center, 1);  //want BV in world space
  min = (obj.modelMatrix * glm::vec4(obj.min, 1));
  max = (obj.modelMatrix * glm::vec4(obj.max, 1));

  radius = distance(center, max);
}

BoundingSphereCentroid::BoundingSphereCentroid(const std::vector<std::pair<Object*, glm::vec3>>& _obj) : BoundingVolume(*_obj.front().first), obj(*_obj.front().first)
{
  min = _obj[0].first->modelMatrix * glm::vec4(_obj[0].first->min, 1);
  max = _obj[0].first->modelMatrix * glm::vec4(_obj[0].first->max, 1);
  min += _obj[0].second;
  max += _obj[0].second;

  for (unsigned i = 0; i < _obj.size(); ++i)
  {
    glm::vec3 objmin = _obj[i].first->modelMatrix * glm::vec4(_obj[i].first->min, 1);
    glm::vec3 objmax = _obj[i].first->modelMatrix * glm::vec4(_obj[i].first->max, 1);

    objmin += _obj[i].second;
    objmax += _obj[i].second;

    if (objmin.x < min.x)
      min.x = objmin.x;
    if (objmin.y < min.y)
      min.y = objmin.y;
    if (objmin.z < min.z)
      min.z = objmin.z;

    if (objmax.x > max.x)
      max.x = objmax.x;
    if (objmax.y > max.y)
      max.y = objmax.y;
    if (objmax.z > max.z)
      max.z = objmax.z;
  }

  center = (max + min) / 2.0f;
  radius = distance(center, max);
}

void BoundingSphereCentroid::Update(float dt, glm::vec3 offset = glm::vec3(0, 0, 0))
{
  //recalc bounding box on update in case of rotation or scale of object
  //want BV in world space and model things are in model space so 
  //point = world transform * model to world * model coord
  center = (obj.modelMatrix * glm::vec4(obj.center, 1));
  min = (obj.modelMatrix * glm::vec4(obj.min, 1));
  max = (obj.modelMatrix * glm::vec4(obj.max, 1));
  
  center += offset;
  min += offset;
  max += offset;
  
  radius = distance(center, max);

}
