

#include "ObjContent.h"
// #include <fstream>
// #include <vector>
// #include <unordered_map>

class ObjReader
{
private:
	std::string fname;

public:
	ObjReader() {}
	ObjContent read(std::string f);
};