#pragma once

#include "common.hpp"

class Icon : public Renderable
{
static 	Texture icon_texture;
static 	Texture icon_texture_start;
static 	Texture icon_texture_quit;
static 	Texture icon_texture_back;
static 	Texture icon_texture_pause;
static 	Texture icon_texture_info;
static 	Texture icon_texture_resume;
static 	Texture icon_texture_menu;
static 	Texture icon_texture_restart;
static 	Texture icon_texture_yes;
static 	Texture icon_texture_no;
static 	Texture icon_texture_save;
static 	Texture icon_texture_ok;
static 	Texture icon_texture_l0;
static 	Texture icon_texture_l1;
static 	Texture icon_texture_l2;
static 	Texture icon_texture_l3;
static 	Texture icon_texture_l4;
static 	Texture icon_texture_l5;
static 	Texture icon_texture_s0;
static 	Texture icon_texture_s1;
static 	Texture icon_texture_s2;
static 	Texture icon_texture_s3;
static 	Texture icon_texture_s4;
static 	Texture icon_texture_s5;
static 	Texture icon_texture_s6;
static 	Texture icon_texture_s7;
static 	Texture icon_texture_s8;
static 	Texture icon_texture_s9;
static 	Texture icon_texture_key;
static 	Texture icon_texture_goldcoin;
static 	Texture icon_texture_h0;
static 	Texture icon_texture_h1;
static 	Texture icon_texture_h2;
static 	Texture icon_texture_h3;

public:

	// Creates all the associated render resources and default transform
	bool init(char icon);

	// Releases all the associated resources
	void destroy();

	// Renders the icon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current icon position
	vec2 get_position()const;

	char get_type()const;

	// Sets the new icon position
	void set_position(vec2 position);

	// sets the position for score according to digit
	void set_digit_position(int digit);

	// Returns the icon' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	bool load_texture(Texture& texture, const char* path);

	void mouse_hover_on();

	void mouse_hover_off();

	void switch_to_normal();

	void switch_to_hover();

	void update_key_status(bool visibility);
private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	char m_type;
	float m_width;
	float m_height;
	bool m_hover;

};