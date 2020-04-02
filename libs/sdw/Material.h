#ifndef MATERIAL_H
#define MATERIAL_H
#include <iostream>
#include <glm/glm.hpp>
#include <PpmContent.h>
#include <CanvasTriangle.h>

class Material
{
private:
	glm::vec3 uintToVec3(uint32_t rgb)
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
			float y = v * (H - 1);
			// printf("W = %i\n", W - 1);
			// printf("H = %i\n", H - 1);
			// printf("U = %f\n", u);
			// printf("V = %f\n", v);
			uint32_t uint_col = Ka_ppm->image[y][x];
			col = uintToVec3(uint_col);
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
			float y = v * (H - 1);

			uint32_t uint_col = Kd_ppm->image[y][x];
			col = uintToVec3(uint_col);
			// std::cout << col << std::endl;
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
		// printf("kd\n");
		if (Ks_is_texture)
		{
			int W = Ks_ppm->width;
			int H = Ks_ppm->height;
			float x = u * (W - 1);
			float y = v * (H - 1);
			uint32_t uint_col = Ks_ppm->image[y][x];
			col = uintToVec3(uint_col);
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
	PpmContent *Ka_ppm;

	glm::vec3 Kd;
	bool Kd_is_texture = false;
	PpmContent *Kd_ppm;

	glm::vec3 Ks;
	bool Ks_is_texture = false;
	PpmContent *Ks_ppm;

	float specularExponent = 100.0f;
	float mirrorness = 0.0f;

	Material() {}

	Material(std::string n)
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

	void setSpecularExponent(float exp)
	{
		specularExponent = exp;
	}

	void setKaToTexture(PpmContent *ppm)
	{
		Ka_is_texture = true;
		Ka_ppm = ppm;
	}
	void setKdToTexture(PpmContent *ppm)
	{
		Kd_is_texture = true;
		Kd_ppm = ppm;
	}
	void setKsToTexture(PpmContent *ppm)
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

	bool getKa(float u, float v, CanvasTriangle tri, glm::vec3 &col)
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
		// printf("point_texture = %f,%f\n", point_texture.x, point_texture.y);
		glm::vec3 toReturn;
		getKa(point_texture.x, point_texture.y, toReturn);
		col = toReturn;
		return 0;
	}

	bool getKd(float u, float v, CanvasTriangle tri, glm::vec3 &col)
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
		glm::vec3 toReturn;
		getKd(point_texture.x, point_texture.y, toReturn);
		col = toReturn;
		return 0;
	}
	bool getKs(float u, float v, CanvasTriangle tri, glm::vec3 &col)
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
		// printf("point_texture = %f,%f\n", point_texture.x, point_texture.y);
		glm::vec3 toReturn;
		getKs(point_texture.x, point_texture.y, toReturn);
		col = toReturn;
		return 0;
	}
};

#endif