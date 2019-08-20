#include "bladeTrap.hpp"

#include <cmath>
#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
bool BladeTrap::init()
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	FILE* mesh_file = fopen(mesh_path("blade_trap_small.mesh"), "r");
	if (mesh_file == nullptr) {
		std::cout << "Failed to open mesh file: blade_trap_small.mesh" << std::endl;
		return false;
	}

	// Reading vertices and colors
	size_t number_vertices;
	fscanf(mesh_file, "%zu\n", &number_vertices);
	for (size_t i = 0; i < number_vertices; ++i) {
		float x, y, z;
		float _u[3]; // unused
		int r, g, b;
		fscanf(mesh_file, "%f %f %f %f %f %f %d %d %d\n", &x, &z, &y, _u, _u + 1, _u + 2, &r, &g, &b);
		Vertex vertex;
		vertex.position = { x, y, -z };
		vertex.color = { (float)r / 255, (float)g / 255, (float)b / 255 };
		vertices.push_back(vertex);
	}

	// Reading associated indices
	size_t num_indices;
	fscanf(mesh_file, "%zu\n", &num_indices);
	for (size_t i = 0; i < num_indices; ++i)
	{
		// int unused;
		int idx[3];
		int junk;
		fscanf(mesh_file, "%d %d %d %d\n", &junk, idx, idx + 1, idx + 2); // ONE MORE %d at begning
		indices.push_back((uint16_t)idx[0]);
		indices.push_back((uint16_t)idx[1]);
		indices.push_back((uint16_t)idx[2]);
	}

	// Done reading
	fclose(mesh_file);

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl")))
		return false;

	// Setting initial values
	m_scale.x = 3.f;
	m_scale.y = 3.f;
	m_rotation = 0.f;
	m_num_indices = indices.size();
	m_color = vec3{ 1.f, 1.f, 1.f };
	return true;
}

void BladeTrap::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);

	effect.release(); // according to https://piazza.com/class/jp1oa2b3dx04b4?cid=257

}

void BladeTrap::update(float ms)
{
	float rotation_speed = 0.008;
	m_rotation += rotation_speed * ms;
	m_rotation = fmod(m_rotation ,2 * 3.1415926);
}

void BladeTrap::draw(const mat3& projection)
{
	transform_begin();
	transform_translate(m_position);
	transform_rotate(m_rotation);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

	// !!! Salmon Color
	float color[] = { m_color.x, m_color.y, m_color.z };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, (GLsizei)m_num_indices, GL_UNSIGNED_SHORT, nullptr);

}

vec2 BladeTrap::get_position() const
{
	return m_position;
}

void BladeTrap::set_position(vec2 position)
{
	m_position = position;
}

float BladeTrap::get_radius() const
{
	return 52.0;
}