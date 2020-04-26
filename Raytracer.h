#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "Camera.h"

using namespace std;
using namespace glm;

vec3 getReflectedRay(vec3 planeNormal, vec3 viewRay);
uint getLightsInShadow(OBJ obj, vector<vec3> lightSources, vec3 point, vec3 ray, uint triIndex);
float getShadowProportion(OBJ obj, vector<vec3> lightSources, vec3 planeNormal, vec3 point, vec3 ray, uint triIndex);
vec3 getColourOfPoint(vector<vec3> lightSources, vec3 planeNormal, vec3 point, vec3 ray, vec3 Ka, vec3 Kd, vec3 Ks, float spec);
Intersection getClosestIntersection(OBJ obj, PPM ppm, vec3 ray);
void drawRaytraces(OBJ obj);

vec3 getReflectedRay(vec3 planeNormal, vec3 viewRay)
{
	planeNormal = normalize(planeNormal);
	if (dot(viewRay, planeNormal) > 0.0f)
	{
		planeNormal = -1.0f * planeNormal;
	}
	else
	{
		viewRay = -1.0f * viewRay;
	}
	vec3 reflection = viewRay - (2.0f * dot(planeNormal, viewRay) * planeNormal);
	return normalize(reflection);
}

uint getLightsInShadow(OBJ obj, vector<vec3> lightSources, vec3 point, vec3 ray, uint triIndex)
{
	uint lightsInShadow = 0;
#pragma omp parallel
#pragma omp for
	for (uint l = 0; l < lightSources.size(); l++)
	{
		vec3 shadowRay = lightSources.at(l) - point;
		float distanceToLight = length(shadowRay);
		
		bool inShadow = 0;
		for (uint c = 0; c < obj.faces.size(); c++)
		{
			ModelTriangle tri = ModelTriangle(obj.faces[c].vertices[0], obj.faces[c].vertices[1], obj.faces[c].vertices[2], MTL());
			// calculate shadow ray intersection
			vec3 eu = tri.vertices[1] - tri.vertices[0];
			vec3 ev = tri.vertices[2] - tri.vertices[0];
			vec3 sp = (point - tri.vertices[0]);
			mat3 de(-normalize(shadowRay), eu, ev);
			vec3 possibleSolution = glm::inverse(de) * sp;

			float t = possibleSolution.x;
			float u = possibleSolution.y;
			float v = possibleSolution.z;
			
			if (0.0 <= u && u <= 1.0 && 0.0 <= v && v <= 1.0 && (u + v) <= 1.0 && t > 0.4f && c != triIndex)
			{
				if (t < (distanceToLight) && (abs(t - distanceToLight) > 0.01f))
				{
					inShadow = true;
					break;
				}
			}
		}
		if (inShadow)
		{
			lightsInShadow += 1;
		}
	}
	return lightsInShadow;
}

float getShadowProportion(OBJ obj, vector<vec3> lightSources, vec3 planeNormal, vec3 point, vec3 ray, uint triIndex)
{
	uint shadows = 0;
	float totalShift = 5.0f;
	vec3 shift = totalShift * normalize(planeNormal);
	shadows += getLightsInShadow(obj, lights, point, ray, triIndex);
	shadows += getLightsInShadow(obj, lights, point + shift, ray, triIndex);
	shadows += getLightsInShadow(obj, lights, point - shift, ray, triIndex);

	float shadeProportion = 0.0f;
	if (shadows == (lightSources.size() * 3)) // all levels in shade
	{
		shadeProportion = 1.0f;
	}
	else if (shadows == 0)                    // no levels in shade
	{
		shadeProportion = 0.0f;
	}
	else                                      // some levels in shade
	{
		float shift = -1.0f * totalShift;
		while (shadows >= 1)
		{
			shift += 0.02f;
			shadows = getLightsInShadow(obj, lights, point + (shift * normalize(planeNormal)), ray, triIndex);
		}
		shadeProportion = (shift + totalShift) / (totalShift * 2.0f);
	}
	return shadeProportion;
}

vec3 getColourOfPoint(vector<vec3> lightSources, vec3 planeNormal, vec3 point, vec3 ray, vec3 Ka, vec3 Kd, vec3 Ks, float spec)
{
	vec3 Ip = vec3(60.0f, 60.0f, 60.0f); // point intensity
	vec3 Ia = vec3(0.05f, 0.05f, 0.05f); // ambient intensity
	vec3 I  = vec3(1.00f, 1.00f, 1.00f); // final intensity

	for (uint l = 0; l < lightSources.size(); l++)
	{
		vec3 lightVec = (lights.at(l) - point);
		float lightDropOff = 10 / length(lightVec); // (1 / (4 * M_PI * length(lightVec) * length(lightVec)))

		// Ambient
		vec3 ambient = Ia * Ka;

		// Lambert
		vec3 normal = normalize(planeNormal);
		float lightDotNormal = dot(normalize(lightVec), normal);
		if (lightDotNormal < 0.0f)
		{
			normal *= -1.0f;
			lightDotNormal = dot(normalize(lightVec), normal);
		}
		vec3 lambert = lightDropOff * Ip * Kd * lightDotNormal;

		// Phong
		vec3 phong;
		vec3 flipRay = -1.0f * ray;
		vec3 reflection = lightVec - (2.0f * (dot(lightVec, normalize(planeNormal)) * normalize(planeNormal)));
		float reflectionDotView = dot(normalize(reflection), normalize(flipRay));
		if (reflectionDotView < 0.0f) // no reflection
		{
			phong = vec3(0.0f, 0.0f, 0.0f);
		}
		else                          // reflection
		{
			phong = lightDropOff * Ip * Ks * pow(reflectionDotView, spec);
		}

		I = I * (ambient + lambert + phong);
		if (I.x > 255.0f)
		{
			I.x = 255.0f;
		}
		if (I.y > 255.0f)
		{
			I.y = 255.0f;
		}
		if (I.z > 255.0f)
		{
			I.z = 255.0f;
		}
	}
	return I;
}

Intersection getClosestIntersection(OBJ obj, vec3 ray, vec3 point, int rayBounces)
{
	Intersection closest = Intersection();
	closest.distanceToCamera = INFINITY;

#pragma omp parallel
#pragma omp for
	for (uint c = 0; c < obj.faces.size(); c++)
	{
		ModelTriangle tri = obj.faces[c];
		// calculate light ray intersection
		vec3 eu = tri.vertices[1] - tri.vertices[0];
		vec3 ev = tri.vertices[2] - tri.vertices[0];
		vec3 sp = (point - tri.vertices[0]);
		mat3 de(-ray, eu, ev);
		vec3 possibleSolution = glm::inverse(de) * sp;

		float t = abs(possibleSolution.x);
		float u = possibleSolution.y;
		float v = possibleSolution.z;

		if (0.0 <= u && u <= 1.0 && 0.0 <= v && v <= 1.0 && (u + v) <= 1.0 && t >= 1.0f)
		{
			if (t < closest.distanceToCamera)
			{
				vec3 u_tri = u * (tri.vertices[1] - tri.vertices[0]);
				vec3 v_tri = v * (tri.vertices[2] - tri.vertices[0]);
				
				vec3 point = (u_tri + v_tri) + tri.vertices[0];
				vec3 planeNorm = cross(eu, ev);

				vec3 Ka;
				tri.mtl.getKa(u, v, obj.textureTris[c], Ka);
				vec3 Kd;
				tri.mtl.getKd(u, v, obj.textureTris[c], Kd);
				vec3 Ks;
				tri.mtl.getKs(u, v, obj.textureTris[c], Ks);
				
				float spec = tri.mtl.specularity;
				float brightness = 1.0;
				Colour pixelCol;
				if (tri.mtl.mirrorness > 0.01f && rayBounces > 0)
				{
					vec3 reflectedRay = getReflectedRay(planeNorm, ray);
					Intersection intersection = getClosestIntersection(obj, reflectedRay, point, rayBounces - 1);
					if (intersection.distanceToCamera < INFINITY)
					{
						pixelCol = intersection.colour;
					}
					else
					{
						pixelCol = Colour(0, 0, 0);
					}
				}
				else
				{
					vec3 col = getColourOfPoint(lights, planeNorm, point, ray, Ka, Kd, Ks, spec);
					pixelCol = Colour(col.x, col.y, col.z, 1.0f);
					if (mode == 3)      // hard shadows
					{
						uint shadows = getLightsInShadow(obj, lights, point, ray, c);
						if (shadows == lights.size())
						{
							pixelCol.brightness = 0.2f;
						}
					}
					else if (mode >= 4) // soft shadows
					{
						float shadeProportion = getShadowProportion(obj, lights, planeNorm, point, ray, c);
						brightness -= pow(shadeProportion, 0.5f);
						pixelCol.brightness = brightness;
					}
				}
				closest = Intersection(point, t, tri, pixelCol);
			}
		}
	}
	return closest;
}

void drawRaytrace(OBJ obj)
{
	vector<vec2> aliasPattern;
	aliasPattern.push_back(vec2(0.0f, 0.0f)); 	// float foo = 10.0f;
	aliasPattern.push_back(vec2(0.5f, 0.0f)); 	// aliasPattern.push_back(vec2(2.0f / foo, 2.0f / foo));
	aliasPattern.push_back(vec2(-0.5f, 0.0f));  // aliasPattern.push_back(vec2(7.0f / foo, 1.0f / foo));
	aliasPattern.push_back(vec2(0.0f, 0.5f)); 	// aliasPattern.push_back(vec2(3.0f / foo, 7.0f / foo));
	aliasPattern.push_back(vec2(0.0f, -0.5f));  // aliasPattern.push_back(vec2(8.0f / foo, 6.0f / foo));

#pragma omp parallel
#pragma omp for
	for (int x = 0; x < WIDTH; x++)
	{
		float progress = float(x * HEIGHT) / float(WIDTH * HEIGHT) * 100;
		cout << progress << "%\n\n";
		for (int y = 0; y < HEIGHT; y++)
		{
			vec3 ray;
			vector<Colour> colours;
			Intersection intersection;

			float xp = -(x - w);
			float yp = (y - h);

			if (mode != 5 ) // without anti-aliasing
			{
				// calculate ray for each pixel
				ray = vec3(xp, yp, FOCAL_LENGTH) * glm::inverse(cameraRotation);
				ray = glm::normalize(ray);
				intersection = getClosestIntersection(obj, ray, cameraPosition, 3);
				if (intersection.distanceToCamera < INFINITY)
				{
					window.setPixelColour(x, y, -0.5, intersection.colour.pack());
				}
			}
			else            // with anti-aliasing 
			{
				for (uint a = 0; a < aliasPattern.size(); a++)
				{
					// calculate ray for each sub-pixel
					ray = vec3(xp + aliasPattern.at(a).x, yp + aliasPattern.at(a).y, FOCAL_LENGTH) * glm::inverse(cameraRotation);
					ray = glm::normalize(ray);
					intersection = getClosestIntersection(obj, ray, cameraPosition, 3);
					if (intersection.distanceToCamera < INFINITY)
					{
						colours.push_back(intersection.colour);
					}
				}
				// average colour of each ray
				if (colours.size() > 0)
				{
					Colour avgCol = colours.at(0);
					for (uint i = 1; i < colours.size(); i++)
					{
						Colour avgColPrime = avgCol.average(colours.at(i));
						avgCol = avgColPrime;
					}
					window.setPixelColour(x, y, -0.5, avgCol.pack());
				}
			}
			
		}
	}
	cout << "100%\n\n";
}
#endif