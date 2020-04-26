#ifndef MATERIAL_H
#define MATERIAL_H
#include "TextureTriangle.h"
#include "Texture.h"
#include "Colour.h"
#include <glm/glm.hpp>
#include <iostream>

using namespace std;
using namespace glm;

class MTL
{
private:
	glm::vec3 intToVec(uint32_t rgb)
	{
		int r = (rgb >> 16) & 0xFF;
		int g = (rgb >> 8) & 0xFF;
		int b = rgb & 0xFF;
		return glm::vec3(r, g, b);
	}

	bool getKa(float u, float v, glm::vec3 &col)
	{
		// printf("ka\n");
		if (Ka_is_texture)
		{
			int W = Ka_ppm->width;
			int H = Ka_ppm->height;
			float x = u * (W - 1);
			float y = H - 1 - v * (H - 1);
			uint32_t uint_col = Ka_ppm->image[y][x];
			col = intToVec(uint_col);
			glm::vec3 col_mul = (col / 255.0f) * (Ka / 255.0f);
			col = col_mul * 255.0f;
			return 0;
		}
		else
		{
			return -1;
		}
	}

	bool getKd(float u, float v, glm::vec3 &col)
	{
		// printf("kd\n");
		if (Kd_is_texture)
		{
			int W = Kd_ppm->width;
			int H = Kd_ppm->height;
			float x = u * (W - 1);
			float y = H - 1 - v * (H - 1);
			uint32_t uint_col = Kd_ppm->image[y][x];
			col = intToVec(uint_col);
			glm::vec3 col_mul = (col / 255.0f) * (Kd / 255.0f);
			col = col_mul * 255.0f;
			return 0;
		}
		else
		{
			return -1;
		}
	}

	bool getKs(float u, float v, glm::vec3 &col)
	{
		// printf("ks\n");
		if (Ks_is_texture)
		{
			int W = Ks_ppm->width;
			int H = Ks_ppm->height;
			float x = u * (W - 1);
			float y = H - 1 - v * (H - 1);
			uint32_t uint_col = Ks_ppm->image[y][x];
			col = intToVec(uint_col);
			glm::vec3 col_mul = (col / 255.0f) * (Ks / 255.0f);
			col = col_mul * 255.0f;
			return 0;
		}
		else
		{
			return -1;
		}
	}

public:
	std::string name;
	
	glm::vec3 Ka;
	bool Ka_is_texture = false;
	PPM *Ka_ppm;

	glm::vec3 Kd;
	bool Kd_is_texture = false;
	PPM *Kd_ppm;

	glm::vec3 Ks;
	bool Ks_is_texture = false;
	PPM *Ks_ppm;

	float specularity = 100.0f;
	float mirrorness = 0.0f;

	MTL() {}

	MTL(std::string n)
	{
		name = n;
	}

	void setKaToColour(glm::vec3 col)
	{
		// Ka_is_texture = false;
		Ka = col;
	}

	void setKdToColour(glm::vec3 col)
	{
		// Kd_is_texture = false;
		Kd = col;
	}

	void setKsToColour(glm::vec3 col)
	{
		// Ks_is_texture = false;
		Ks = col;
	}

	void setSpecularity(float exp)
	{
		specularity = exp;
	}

	void setKaToTexture(PPM *ppm)
	{
		Ka_is_texture = true;
		Ka_ppm = ppm;
	}

	void setKdToTexture(PPM *ppm)
	{
		Kd_is_texture = true;
		Kd_ppm = ppm;
	}

	void setKsToTexture(PPM *ppm)
	{
		Ks_is_texture = true;
		Ks_ppm = ppm;
	}

	bool getKa(glm::vec3 &col)
	{
		if (Ka_is_texture)
		{
			return -1;
		}
		else
		{
			col.x = Ka.x;
			col.y = Ka.y;
			col.z = Ka.z;
			return 0;
		}
	}

	bool getKd(glm::vec3 &col)
	{
		if (Kd_is_texture)
		{
			return -1;
		}
		else
		{
			col.x = Kd.x;
			col.y = Kd.y;
			col.z = Kd.z;
			return 0;
		}
	}

	bool getKs(glm::vec3 &col)
	{
		if (Ks_is_texture)
		{
			return -1;
		}
		else
		{
			col.x = Ks.x;
			col.y = Ks.y;
			col.z = Ks.z;
			return 0;
		}
	}

	bool getKa(float u, float v, TextureTriangle tri, glm::vec3 &col)
	{
		if (!Ka_is_texture)
		{
			col = Ka;
			return -1;
		}
		glm::vec2 v0 = glm::vec2(tri.vertices[0].x, tri.vertices[0].y);
		glm::vec2 v1 = glm::vec2(tri.vertices[1].x, tri.vertices[1].y);
		glm::vec2 v2 = glm::vec2(tri.vertices[2].x, tri.vertices[2].y);

		glm::vec2 u_tex = u * (v1 - v0);
		glm::vec2 v_tex = v * (v2 - v0);

		glm::vec2 point_texture = (u_tex + v_tex) + v0;
		glm::vec3 Kacol;
		getKa(point_texture.x, point_texture.y, Kacol);
		col = Kacol;
		return 0;
	}

	bool getKd(float u, float v, TextureTriangle tri, glm::vec3 &col)
	{
		if (!Kd_is_texture)
		{
			col = Kd;
			return -1;
		}
		glm::vec2 v0 = glm::vec2(tri.vertices[0].x, tri.vertices[0].y);
		glm::vec2 v1 = glm::vec2(tri.vertices[1].x, tri.vertices[1].y);
		glm::vec2 v2 = glm::vec2(tri.vertices[2].x, tri.vertices[2].y);

		glm::vec2 u_tex = u * (v1 - v0);
		glm::vec2 v_tex = v * (v2 - v0);

		glm::vec2 point_texture = (u_tex + v_tex) + v0;
		glm::vec3 Kdcol;
		getKd(point_texture.x, point_texture.y, Kdcol);
		col = Kdcol;
		return 0;
	}

	bool getKs(float u, float v, TextureTriangle tri, glm::vec3 &col)
	{
		if (!Ks_is_texture)
		{
			col = Ks;
			return -1;
		}
		glm::vec2 v0 = glm::vec2(tri.vertices[0].x, tri.vertices[0].y);
		glm::vec2 v1 = glm::vec2(tri.vertices[1].x, tri.vertices[1].y);
		glm::vec2 v2 = glm::vec2(tri.vertices[2].x, tri.vertices[2].y);

		glm::vec2 u_tex = u * (v1 - v0);
		glm::vec2 v_tex = v * (v2 - v0);

		glm::vec2 point_texture = (u_tex + v_tex) + v0;
		glm::vec3 Kscol;
		getKs(point_texture.x, point_texture.y, Kscol);
		col = Kscol;
		return 0;
	}
};
#endif