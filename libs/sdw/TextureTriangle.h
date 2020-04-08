#ifndef TEXTURETRIANGLE_H
#define TEXTURETRIANGLE_H
#include "TexturePoint.h"
#include "Colour.h"
#include <glm/glm.hpp>
#include <iostream>
#include <string>

class TextureTriangle
{
public:
  glm::vec3 vertices[3];
  TexturePoint texturePoints[3];
  std::string textureName;

  TextureTriangle()
  {
  }

  TextureTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, TexturePoint vt0, TexturePoint vt1, TexturePoint vt2, std::string textureName)
  {
    vertices[0] = v0;
    vertices[1] = v1;
    vertices[2] = v2;
    texturePoints[0] = vt0;
    texturePoints[1] = vt1;
    texturePoints[2] = vt2;
  }
};

std::ostream &operator<<(std::ostream &os, const TextureTriangle &triangle)
{
  os << "(" << triangle.vertices[0].x << ", " << triangle.vertices[0].y << ", " << triangle.vertices[0].z << ")" << std::endl;
  os << "(" << triangle.vertices[1].x << ", " << triangle.vertices[1].y << ", " << triangle.vertices[1].z << ")" << std::endl;
  os << "(" << triangle.vertices[2].x << ", " << triangle.vertices[2].y << ", " << triangle.vertices[2].z << ")" << std::endl;
  os << std::endl;
  return os;
}
#endif