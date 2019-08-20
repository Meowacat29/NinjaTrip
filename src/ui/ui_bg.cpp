// Header
#include "ui_bg.hpp"

#include <cmath>

#if _WIN32
	const int width = 1200;
	const int height = 600;
	const double SIZE_MODIFIER = 0.25f;
	const bool IS_WIN = true;
#elif __APPLE__
	const int width = 1200;
	const int height = 600;
	const double SIZE_MODIFIER = 0.5f;
	const bool IS_WIN = false;
#elif __linux__
    // linux
#endif


Texture Ui_bg::ui_bg_texture;
Texture Ui_bg::ui_bg_texture_main;
Texture Ui_bg::ui_bg_texture_pause;
Texture Ui_bg::ui_bg_texture_death;
Texture Ui_bg::ui_bg_texture_instruction;
Texture Ui_bg::ui_bg_texture_stageClear;
Texture Ui_bg::ui_bg_texture_save;
Texture Ui_bg::ui_bg_texture_load;

bool Ui_bg::init(char type)
{
	// Load shared texture
	char const *path;
	//load textures and set position
	switch(type){
		case (char) 'm':
			{
				path = ui_path("bg_main.png");
				load_texture(ui_bg_texture_main, path);
				break;
			}
		case (char) 'p':
			{
				path = ui_path("bg_pause.png");
				load_texture(ui_bg_texture_pause, path);
				break;
			}
		case (char) 'd':
			{
				path = ui_path("bg_death.png");
				load_texture(ui_bg_texture_death, path);
				break;
			}
		case (char) 'i':
			{
				path = ui_path("bg_instruction.png");
				load_texture(ui_bg_texture_instruction, path);
				break;
			}
		case (char) 'c':
			{
				path = ui_path("bg_stageClear.png");
				load_texture(ui_bg_texture_stageClear, path);
				break;
			}
		case (char) 's':
			{
				path = ui_path("bg_save.png");
				load_texture(ui_bg_texture_save, path);
				break;
			}
		case (char) 'l':
			{
				path = ui_path("bg_load.png");
				load_texture(ui_bg_texture_load, path);
				break;
			}
		delete path;
	}

	// The position corresponds to the center of the texture (since if you halve the height u get the middle
	float wr = ui_bg_texture.width * SIZE_MODIFIER;
	float hr = ui_bg_texture.height * SIZE_MODIFIER;

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
	if (IS_WIN) {
		m_scale.x = 0.63f;
		m_scale.y = 0.57f;
		m_position = { width / 4 + 15.f, height / 4};
	}
	else {
		m_scale.x = 0.63f;
		m_scale.y = 0.57f;
		m_position = { width / 2 + 30.f, height / 2 };
	}

	m_rotation = 0.f;
	m_type = type;
	return true;
}

bool Ui_bg::load_texture(Texture& texture, const char* path){
	if (!texture.is_valid()){
		if (!ui_bg_texture.load_from_file(path)
			|| ! texture.load_from_file(path))
		{
			fprintf(stderr, "Failed to load icon texture!");
			return false;
		}
	}
	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Ui_bg::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);

	effect.release(); // according to https://piazza.com/class/jp1oa2b3dx04b4?cid=257

}

void Ui_bg::draw(const mat3& projection)
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
	char type = m_type;
	switch(type){
		case(char) 'm':
			glBindTexture(GL_TEXTURE_2D, ui_bg_texture_main.id);
			break;
		case(char) 'p':
			glBindTexture(GL_TEXTURE_2D, ui_bg_texture_pause.id);
			break;
		case(char) 'd':
			glBindTexture(GL_TEXTURE_2D, ui_bg_texture_death.id);
			break;
		case(char) 'i':
			glBindTexture(GL_TEXTURE_2D, ui_bg_texture_instruction.id);
			break;
		case(char) 'c':
			glBindTexture(GL_TEXTURE_2D, ui_bg_texture_stageClear.id);
			break;
		case(char) 's':
			glBindTexture(GL_TEXTURE_2D, ui_bg_texture_save.id);
			break;
		case(char) 'l':
			glBindTexture(GL_TEXTURE_2D, ui_bg_texture_load.id);
			break;
	}

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Ui_bg::get_position()const
{
	return m_position;
}

void Ui_bg::set_position(vec2 position) {
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the ui_bg
vec2 Ui_bg::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * ui_bg_texture.width, std::fabs(m_scale.y) * ui_bg_texture.height };
}