#include <fstream>
#include <vector>
#include <unordered_map>
using namespace std;

class ObjContent
{
private:
	std::vector<ModelTriangle> faces;
	std::unordered_map<std::string, Colour> palette;

public:
	ObjContent() {}

	void addFace(ModelTriangle face)
	{
		faces.push_back(face);
	}

	void addColour(Colour col, std::string name)
	{
		palette[name] = col;
	}
};
