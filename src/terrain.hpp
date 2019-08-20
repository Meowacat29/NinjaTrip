#pragma once

#include "common.hpp"

class Terrain : public Renderable
{
	// Shared between all terrain, no need to load one for each instance
	static Texture terrain_texture_0;
	static Texture terrain_texture_1;
	static Texture terrain_texture_2;
	static Texture terrain_texture_3;
	static Texture terrain_texture_4;

	static Texture terrain_texture_black;
public:
	// Creates all the associated render resources and default transform
	bool init(char type);

	// Releases all the associated resources
	void destroy();

	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the terrain
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current terrain position
	vec2 get_position()const;

	// Sets the new terrain position
	void set_position(vec2 position);

	// Returns the terrain' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	char get_type()const;

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	vec2 m_size;
	char m_type;// the type of terrain - regular(snow), solid black;
	int m_sprite;// random sprite index for each instance
};