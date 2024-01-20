#ifndef TERRAIN_MAP_H
#define TERRAIN_MAP_H

#include <vector>
#include "render/Mesh.h"

Mesh createHeightmapMesh(int x, int y, int featureSize, float minHeight = 0.0f, float maxHeight = 1.0f);

#endif //TERRAIN_MAP_H
