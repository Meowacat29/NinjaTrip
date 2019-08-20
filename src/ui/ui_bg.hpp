#pragma once

#include "common.hpp"

class Ui_bg : public Renderable
{
	// Shared between all ui_bg, no need to load one for each instance
	static Texture ui_bg_texture;
	static Texture ui_bg_texture_main;
	static Texture ui_bg_texture_pause;
 	static Texture ui_bg_texture_death;
	static Texture ui_bg_texture_instruction;
	static Texture ui_bg_texture_stageClear;
	static Texture ui_bg_texture_save;
	static Texture ui_bg_texture_load;
	
public:
	// Creates all the associated render resources and default transform
	bool init(char type);

	// Releases all the associated resources
	void destroy();

	// Renders the ui_bg
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current ui_bg position
	vec2 get_position()const;

	// Sets the new ui_bg position
	void set_position(vec2 position);

	// Returns the ui_bg' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	bool load_texture(Texture& texture, const char* path);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	char m_type;

};