#ifndef COLOUR_H
#define COLOUR_H
#include <glm/glm.hpp>
#include <iostream>

#define AMBIENCE 0.2f

class Colour
{
private:
public:
  std::string name;
  int red;
  int green;
  int blue;
  float brightness;
  float mirrorness;
  float refractivity;

  Colour()
  {
    brightness = AMBIENCE;
  }
  
  Colour(glm::vec3 rgb)
  {
    name = "";
    red = rgb.x;
    green = rgb.y;
    blue = rgb.z;
    brightness = 1.0f;
  }

  Colour(glm::vec3 rgb, float bri)
  {
    name = "";
    red = rgb.x;
    green = rgb.y;
    blue = rgb.z;
    brightness = bri;
  }

  Colour(std::string n, glm::vec3 rgb)
  {
    name = n;
    red = rgb.x;
    green = rgb.y;
    blue = rgb.z;
    brightness = 1.0f;
  }

  Colour(std::string n, glm::vec3 rgb, float bri)
  {
    name = n;
    red = rgb.x;
    green = rgb.y;
    blue = rgb.z;
    brightness = bri;
  }

  Colour(int r, int g, int b)
  {
    name = "";
    red = r;
    green = g;
    blue = b;
    brightness = 1.0f;
  }

  Colour(int r, int g, int b, float bri)
  {
    name = "";
    red = r;
    green = g;
    blue = b;
    brightness = bri;
  }

  Colour(std::string n, int r, int g, int b)
  {
    name = n;
    red = r;
    green = g;
    blue = b;
    brightness = 1.0f;
  }

  Colour(std::string n, int r, int g, int b, float bri)
  {
    name = n;
    red = r;
    green = g;
    blue = b;
    brightness = bri;
  }

  void setMirrorness(float mi)
  {
    mirrorness = mi;
  }

  void setRefractivity(float re)
  {
    refractivity = re;
  }

  void setBrightness(float br)
  {
    brightness = br;
  }

  Colour average(Colour col)
  {
    float bri = (brightness + col.brightness) / 2;
    int r = (red * red + col.red * col.red) / 2;
    int g = (green * green + col.green * col.green) / 2;
    int b = (blue * blue + col.blue * col.blue) / 2;
    return Colour(sqrt(r), sqrt(g), sqrt(b), bri);
  }

  uint32_t pack()
  {
    if (brightness > 1.0f)
    {
      brightness = 1.0f;
    }
    if (brightness < AMBIENCE)
    {
      brightness = AMBIENCE;
    }
    uint32_t colour = (255 << 24) + (int(red * brightness) << 16) + (int(green * brightness) << 8) + int(blue * brightness);
    return colour;
  }
};

std::ostream &operator<<(std::ostream &os, const Colour &colour)
{
  os << colour.name << " [" << colour.red << ", " << colour.green << ", " << colour.blue << "]" << std::endl;
  return os;
}
#endif