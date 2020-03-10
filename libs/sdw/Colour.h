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

  Colour()
  {
    brightness = AMBIENCE;
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