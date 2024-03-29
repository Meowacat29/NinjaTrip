// Header
#include "mouse.hpp"
#include "icon.hpp"
#include <iostream>

#include <cmath>
#if _WIN32
const double SIZE_MODIFIER = 0.35f;
#elif __APPLE__
const double SIZE_MODIFIER = 0.5f;
#elif __linux__
// linux
#endif

Texture Mouse::mouse_texture;

bool Mouse::init()
{
	// Load shared texture
	if (!mouse_texture.is_valid())
	{
		if (!mouse_texture.load_from_file(ui_path("cursor.png")))
		{
			fprintf(stderr, "Failed to load mouse texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture (since if you halve the height u get the middle
	float wr = mouse_texture.width * SIZE_MODIFIER;
	float hr = mouse_texture.height * SIZE_MODIFIER;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[1].texcoord = { 1.f, 1.f,  };
	vertices[2].position = { +wr, -hr, -0.01f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.01f };
	vertices[3].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 0.4f;
	m_scale.y = 0.4f;
	m_rotation = 0.f;

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Mouse::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);
	effect.release(); // according to https://piazza.com/class/jp1oa2b3dx04b4?cid=257
}

void Mouse::draw(const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);
	transform_rotate(m_rotation);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mouse_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Mouse::get_position()const
{
	return m_position;
}

void Mouse::set_position(vec2 position) {
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the mouse
vec2 Mouse::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	std::cout<<" hwight:"<<mouse_texture.width;
		std::cout<<" width:"<<mouse_texture.width;

	return { std::fabs(m_scale.x) * mouse_texture.width, 
		std::fabs(m_scale.y) * mouse_texture.height};
}

bool Mouse::collides_with(const Icon& icon)
{
	if(abs(m_position.x - icon.get_position().x) < icon.get_bounding_box().x /2 
		&& abs(m_position.y - icon.get_position().y) < icon.get_bounding_box().y /2)
		return true;
	return false;
}
