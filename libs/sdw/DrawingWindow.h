#ifndef DRAWINGWINDOW_H
#define DRAWINGWINDOW_H
#include <SDL2/SDL.h>
#include <iostream>
#include <limits>

class DrawingWindow
{

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

public:
  int height;
  int width;
  uint32_t *pixelBuffer;
  float *depthBuffer;

  DrawingWindow();
  DrawingWindow(int w, int h, bool fullscreen);
  void destroy();
  void renderFrame();
  bool pollForInputEvents(SDL_Event *event);
  void setPixelColour(int x, int y, float depth, uint32_t colour);
  void setPixelDepth(int x, int y, float depth);
  float getPixelDepth(int x, int y);
  uint32_t getPixelColour(int x, int y);
  void clearPixels();

  void printMessageAndQuit(const char *message, const char *error)
  {
    if (error == NULL)
    {
      std::cout << message << std::endl;
      exit(0);
    }
    else
    {
      std::cout << message << " " << error << std::endl;
      exit(1);
    }
  }
};
#endif