#ifndef INTERSECTION_H
#define INTERSECTION_H
#include <glm/glm.hpp>
#include <iostream>

using namespace std;
using namespace glm;

class Intersection
{
public:
    glm::vec3 intersectionPoint;
    float distanceToCamera;
    ModelTriangle intersectedTriangle;
    Colour colour;

    Intersection()
    {
    }

    Intersection(glm::vec3 point, float distance, ModelTriangle triangle, Colour col)
    {
        intersectionPoint = point;
        distanceToCamera = distance;
        intersectedTriangle = triangle;
        colour = col;
    }
};
#endif