#ifndef UTILS_H
#define UTILS_H
#include <algorithm>
#include <vector>

vector<double> interpolate1D(double from, double to, int n);
vector<vec3> interpolate3D(vec3 from, vec3 to, int n);
std::string* split(std::string line, char delim);

vector<double> interpolate1D(double from, double to, int n)
{
	vector<double> v;
	double step = (to - from) / (double)(n - 1);
	v.push_back(from);

	for (int i = 0; i < n - 1; i++)
	{
		v.push_back(v.back() + step);
	}
	return v;
}

vector<vec3> interpolate3D(vec3 from, vec3 to, int n)
{
	vector<vec3> v;
	vec3 step = (to - from) / (float)(n - 1);
	v.push_back(from);

	for (int i = 0; i < n - 1; i++)
	{
		v.push_back(v.back() + step);
	}
	return v;
}

std::string* split(std::string line, char delim)
{
    int numberOfTokens = count(line.begin(), line.end(), delim) + 1;
    std::string *tokens = new std::string[numberOfTokens];
    int currentPosition = 0;
    int nextIndex = 0;
    for(int i=0; i<numberOfTokens ;i++) {
        nextIndex = line.find(delim, currentPosition);
        tokens[i] = line.substr(currentPosition,nextIndex-currentPosition);
        currentPosition = nextIndex + 1;
    }
    return tokens;
}
#endif