#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

mat4 Camera::getProj()
{
    if (fov != 0.0f)
    {
        return perspectiveFov(fov, targetSize.x, targetSize.y, near, far);
    }
    else
    {
        return ortho(-targetSize.x / 2, targetSize.x / 2, -targetSize.y / 2, targetSize.y / 2, near, far);
    }
}

mat4 Camera::getView()
{
    return glm::lookAt(position, position + direction, vec3(0.0f, 0.0f, 1.0f));
}
