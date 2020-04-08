#ifndef CANVASTRIANGLE_H
#define CANVASTRIANGLE_H
#include "CanvasPoint.h"
#include "Colour.h"
#include <iostream>

class CanvasTriangle
{
public:
  TexturePoint vertices[3];
  Colour colour;

  CanvasTriangle()
  {
  }

  CanvasTriangle(TexturePoint v0, TexturePoint v1, TexturePoint v2)
  {
    vertices[0] = v0;
    vertices[1] = v1;
    vertices[2] = v2;
  }
};

std::ostream &operator<<(std::ostream &os, const CanvasTriangle &triangle)
{
  os << triangle.vertices[0] << triangle.vertices[1] << triangle.vertices[2] << std::endl;
  return os;
}
#endif