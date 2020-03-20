#ifndef COLOUR_H
#define COLOUR_H
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
  // mirrorness = reflectivity
  float mirrorness;
  float refractivity;

  Colour()
  {
    brightness = AMBIENCE;
  }
  Colour(glm::vec3 rgb)
  {
    name = "";
    brightness = 1.0f;
    red = rgb.x;
    green = rgb.y;
    blue = rgb.z;
  }
  Colour(int r, int g, int b)
  {
    name = "";
    brightness = 1.0f;
    red = r;
    green = g;
    blue = b;
  }
  Colour(int r, int g, int b, float bri)
  {
    name = "";
    brightness = bri;
    red = r;
    green = g;
    blue = b;
  }

  Colour(std::string n, int r, int g, int b, float bri)
  {
    name = n;
    red = r;
    green = g;
    blue = b;
  }
  Colour(std::string n, int r, int g, int b)
  {
    name = n;

    red = r;
    green = g;
    blue = b;
  }

  Colour averageWith(Colour col2)
  {
    float bri = (brightness + col2.brightness) / 2;
    int r = (red * red + col2.red * col2.red) / 2;
    int g = (green * green + col2.green * col2.green) / 2;
    int b = (blue * blue + col2.blue * col2.blue) / 2;
    return Colour(sqrt(r), sqrt(g), sqrt(b), bri);
  }
  void setMirrorness(float mi)
  {
    mirrorness = mi;
  }
  void setRefractivity(float re)
  {
    refractivity = re;
  }
  void setBrightness(float bri)
  {
    brightness = bri;
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