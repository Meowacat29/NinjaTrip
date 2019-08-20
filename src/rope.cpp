// Header
#include "rope.hpp"
# define M_PI           3.14159265358979323846  /* pi */
# define TIMER_LENGTH   3000.f

#include <cmath>

Texture Rope::rope_texture;

const float RANGE = M_PI/3.f;

bool Rope::init()
{
	// Load shared texture
	if (!rope_texture.is_valid())
	{
		if (!rope_texture.load_from_file(textures_path("rope.png")))
		{
			fprintf(stderr, "Failed to load rope texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture (since if you halve the height u get the middle
	float wr = rope_texture.width * 0.5f;
	float hr = rope_texture.height;
  m_size = {wr, hr};

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[1].texcoord = { 1.f, 1.f,  };
	vertices[2].position = { +wr, 0.f, -0.01f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, 0.f, -0.01f };
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
	m_scale.x = 1.f;
	m_scale.y = 2.f;
	m_rotation = 0.f;
	m_rotation_range = {-RANGE, RANGE};
	m_rotation_velocity = 0.f;
	m_swinging = false;
	m_rotation_acceleration = 0.f;
	m_timer = 0.f;
	m_swingDistance = 0;

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Rope::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);

	effect.release(); // according to https://piazza.com/class/jp1oa2b3dx04b4?cid=257
}

void Rope::update(float ms)
{
	// Move rope along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	m_rotation_velocity += m_rotation_acceleration * (ms/1000);
	float step = m_rotation_velocity * (ms/1000);
	if (m_swinging)
	{
		if (m_rotation == 0.f && m_rotation_velocity == 0)
			m_rotation_velocity = m_initial_velocity;
		else if (m_rotation_velocity >= 0.f)
			m_rotation_acceleration = 4.f * m_rotation + 0.5f;
		else
			m_rotation_acceleration = 4.f * m_rotation - 0.5f;
		m_timer = TIMER_LENGTH;
	}
	else
	{
		m_rotation_acceleration = 0.f;
		if (m_rotation == 0.f || m_timer <= 0.f)
		{
			m_rotation_range = {-RANGE, RANGE};
			m_rotation = 0.f;
			m_rotation_velocity = 0.f;
		}
		else if (m_rotation_velocity >= 0.f)
		{
			m_timer -= ms;
			m_rotation_velocity -= 10.f / m_timer;
			m_rotation_range = {-RANGE * m_timer / TIMER_LENGTH, RANGE * m_timer / TIMER_LENGTH};
		}
		else
		{
			m_timer -= ms;
			m_rotation_velocity += 10.f / m_timer;
			m_rotation_range = {-RANGE * m_timer / TIMER_LENGTH, RANGE * m_timer / TIMER_LENGTH};
		}
	}

	float new_rotation = m_rotation + step;
	if (new_rotation <= m_rotation_range.x)
	{
		float difference = new_rotation - m_rotation_range.x;
		new_rotation = m_rotation_range.x - difference;
		m_rotation_velocity = -m_rotation_velocity;
	}
	else if (new_rotation >= m_rotation_range.y)
	{
		float difference = new_rotation - m_rotation_range.y;
		new_rotation = m_rotation_range.y - difference;
		m_rotation_velocity = -m_rotation_velocity;
	}
	m_rotation = new_rotation;
}

void Rope::draw(const mat3& projection)
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
	glBindTexture(GL_TEXTURE_2D, rope_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Rope::get_position()const
{
	return m_position;
}

void Rope::set_position(vec2 position)
{
	m_position = position;
}

vec2 Rope::calculate_current_position(float y)const
{
	float newX = m_position.x - sin(m_rotation) * y;
	float newY = m_position.y + cos(m_rotation) * y;
	return {newX, newY};
}

// Returns the local bounding coordinates scaled by the current size of the rope
vec2 Rope::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * m_size.x, std::fabs(m_scale.y) * m_size.y };
}

bool Rope::is_swinging()const
{
	return m_swinging;
}

void Rope::set_swinging(bool swing)
{
	m_swinging = swing;
}

void Rope::set_initial_velocity(float v)
{
	m_initial_velocity = v;
}

float Rope::get_swingDistance()
{
	return m_swingDistance;
}

void Rope::set_swingDistance(float f)
{
	m_swingDistance = f;
}