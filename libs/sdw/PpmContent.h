#ifndef PPMCONTENT_H
#define PPMCONTENT_H
#include <iostream>
#include <vector>

class PpmContent
{
public:
	std::vector<std::vector<uint32_t>> image;
	int width = 0;
	int height = 0;
	int colour = 0;
	PpmContent() {}
};
#endif