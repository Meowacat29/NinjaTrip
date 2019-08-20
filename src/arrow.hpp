#pragma once

#include "common.hpp"

class Arrow : public Renderable
{
	// Shared between all arrow, no need to load one for each instance
	static Texture arrow_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// Update position of arrow
	void update(float ms);

	// Renders the arrow
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current arrow position
	vec2 get_position()const;

	// Sets the new arrow position
	void set_position(vec2 position);

	// Returns the arrow' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	vec2 m_size;
};