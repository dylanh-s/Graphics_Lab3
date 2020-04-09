#ifndef TEXTURE_H
#define TEXTURE_H
#include <iostream>
#include <vector>

class PPM
{
public:
	std::vector<std::vector<uint32_t>> image;
	int width = 0;
	int height = 0;
	int colour = 0;
	PPM() {}
};
#endif