#include "map.h"
#include <cstdlib>
#include <algorithm>

#include <glm/glm.hpp>

using namespace glm;

//Структура облегчающая доступ к закольцованному массиву
struct WrappedArray {
    float *values;
    int szX;
    int szY;

    WrappedArray(int width, int height) : szX(width), szY(height)
    {
        values = new float[szX * szY];
        for (int x = 0; x < szX; x++)
        {
            for (int y = 0; y < szY; y++)
            {
                values[x + szX * y] = 0.0f;
            }
        }
    }

    ~WrappedArray()
    {
        delete[] values;
    }

    void set(int x, int y, float value)
    {
        x = (x + szX) % szX;
        y = (y + szY) % szY;
        values[x + szX * y] = value;
    }

    float get(int x, int y)
    {
        x = (x + szX) % szX;
        y = (y + szY) % szY;
        return values[x + szX * y];
    }
};

float frand()
{
    return 1.0f - 2.0f * (rand() % 512) / 512.0f;
}

//Шаг генерации ландшафта - усреднение центральной точки "ромба" в зависимости от "точек в вершинах ромба вокруг"
void diamond(WrappedArray &array, int x, int y, int size, float offset)
{
    float a = array.get(x - size, y);
    float b = array.get(x - size, y);
    float c = array.get(x, y - size);
    float d = array.get(x, y + size);

    array.set(x, y, ((a + b + c + d) / 4.0f) + offset);
}

//Аналогичная генерация в центре квадрата
void square(WrappedArray &array, int x, int y, int size, float offset)
{
    float a = array.get(x - size, y - size);
    float b = array.get(x - size, y + size);
    float c = array.get(x + size, y - size);
    float d = array.get(x + size, y + size);

    array.set(x, y, ((a + b + c + d) / 4.0f) + offset);
}

//Задание начальных значение в сетке (с некоторым шагом в ландшафте)
void seed(WrappedArray &array, int featureSize)
{
    for (int y = 0; y < array.szY; y += featureSize)
    {
        for (int x = 0; x < array.szX; x += featureSize)
        {
            array.set(x, y, frand());
        }
    }
}

//Реализация алгоритма генерации ландшафта
void divide (WrappedArray &array, const float &roughness, int size)
{
    int halfStep = size / 2;
    float scale = roughness * size;
    if (halfStep < 1) return;
    for (int y = halfStep; y <= array.szY; y += size)
    {
        for (int x = halfStep; x <= array.szX; x += size)
        {
            square(array, x, y, halfStep, frand() * scale);
        }
    }

    for (int y = 0; y <= array.szY; y += halfStep)
    {
        for (int x = (halfStep + y) % size; x <= array.szX; x += size)
        {
            diamond(array, x, y, halfStep, frand() * scale);
        }
    }
    divide(array, roughness, size / 2);
}

//Создание карты высот для внутреннего пользования
WrappedArray createHeightMapInternal(int x, int y, int featureSize)
{
    WrappedArray array(x, y);

    seed(array, featureSize);
    float roughness = 1.0f;
    divide(array, roughness, featureSize);

    return array;
}

//Создание итогового меша
Mesh createHeightmapMesh(int width, int height, int featureSize, float minHeight, float maxHeight)
{
    WrappedArray heightmap = createHeightMapInternal(width, height, featureSize);
    float minMap = heightmap.get(0, 0);
    float maxMap = heightmap.get(0, 0);
    for(int y = 0; y < heightmap.szY; ++y)
    {
        for(int x = 0; x < heightmap.szX; ++x)
        {
            minMap = std::min(minMap, heightmap.get(x, y));
            maxMap = std::max(maxMap, heightmap.get(x, y));
        }
    }

    Mesh terrain;

    // First pass - store vertices
    for(int y = 0; y < height + 1; ++y)
    {
        for(int x = 0; x < width + 1; ++x)
        {
#if 1

            const float gauss[5][5] = {{1, 4, 7, 4, 1},
                                       {4, 16, 26, 16, 4},
                                       {7, 26, 41, 26, 7},
                                       {4, 16, 26, 16, 4},
                                       {1, 4, 7, 4, 1}

            };
            float meanHeight = 0;
            for(int dx = -2; dx < 3; ++dx)
            {
                for (int dy = -2; dy < 3; ++dy)
                {
                    meanHeight += gauss[dx + 2][dy + 2] * heightmap.get(x + dx, y + dy);
                }
            }
            meanHeight /= 273;
            float nh = (meanHeight - minMap) / (maxMap - minMap);


#else
            // Normalized height
            float nh = (heightmap.get(x, y) - minMap) / (maxMap - minMap);
#endif
            glm::vec3 pos = glm::vec3{};
            pos.x = x;
            pos.y = y;
            pos.z = minHeight + (maxHeight - minHeight) * nh * nh;
            glm::vec3 norm = glm::vec3{0.0f};
            glm::vec3 col = glm::vec3{(float)x / width, (float)y / height, 1.0f};
            //glm::vec2 uv = glm::vec2{(x % (height / 16)) * 1.0f/(height/16), y % (width / 16) * 1.0f/ (width / 16)};
            glm::vec2 uv = glm::vec2{x * 1.0f/((height + 1) / 16.0f), y * 1.0f/((width + 1) / 16.0f)};
            Mesh::Vertex v{pos, norm, col, uv};
            terrain.v.push_back(v);
        }
    }
    // Second pass - create triangles, maybe calculate normals???
    for(int y = 0; y < height; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            // Lower triangle
            terrain.indices.push_back(x + (width + 1) * y);
            terrain.indices.push_back(x + (width + 1) * y + 1);
            terrain.indices.push_back(x + (width + 1) * (y + 1));

            size_t lowerInd[3] = {
                    terrain.indices[terrain.indices.size() - 3],
                    terrain.indices[terrain.indices.size() - 2],
                    terrain.indices[terrain.indices.size() - 1]};

            // Upper triangle
            terrain.indices.push_back(x + (width + 1) * (y + 1));
            terrain.indices.push_back(x + (width + 1) * y + 1);
            terrain.indices.push_back(x + (width + 1) * (y + 1) + 1);

            size_t upperInd[3] = {
                    terrain.indices[terrain.indices.size() - 3],
                    terrain.indices[terrain.indices.size() - 2],
                    terrain.indices[terrain.indices.size() - 1]};


            glm::vec3 v1 = terrain.v[lowerInd[1]].pos - terrain.v[lowerInd[0]].pos;
            glm::vec3 v2 = terrain.v[lowerInd[2]].pos - terrain.v[lowerInd[0]].pos;
            glm::vec3 lowerNormal = normalize(cross(v1, v2));

            glm::vec3 v3 = terrain.v[upperInd[0]].pos - terrain.v[upperInd[1]].pos;
            glm::vec3 v4 = terrain.v[upperInd[0]].pos - terrain.v[upperInd[2]].pos;
            glm::vec3 upperNormal = normalize(cross(v3, v4));

            for (int i = 0; i < 3; ++i)
            {
                terrain.v[lowerInd[i]].norm = lowerNormal;
                terrain.v[upperInd[i]].norm = upperNormal;
            }
        }
    }

    for(int x = 0; x < width + 1; x++)
    {
        terrain.v[x].norm = terrain.v[x  + (width + 1) * height].norm;
    }
    for(int y = 0; y < height + 1; y++)
    {
        terrain.v[(width + 1) * y].norm = terrain.v[width  + (width + 1) * y].norm;
    }

    return terrain;
}
