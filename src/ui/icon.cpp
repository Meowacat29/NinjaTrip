// Header
#include "icon.hpp"
#include <iostream>
#include <cmath>


#if _WIN32
	const int width = 1200;
	const int height = 600;
	const int w_ctr = width / 4;
	const int h_ctr = height / 4;
	const vec2 START_POS = { w_ctr , h_ctr};
	const vec2 QUIT_POS = { w_ctr , h_ctr + 50.f };
	const vec2 BACK_POS = { w_ctr + 100.f, h_ctr + 100.f };
	const vec2 LEVEL_POS = { w_ctr - 120.f, h_ctr };
	const vec2 SAVE_POS = { width / 4 , h_ctr - 30.f };
	const vec2 RESUME_POS = { w_ctr , h_ctr + 50.f };
	const vec2 RESTART_POS = { w_ctr , h_ctr + 20.f };
	const vec2 MENU_POS = { w_ctr , h_ctr + 80.f };
	const vec2 OK_POS = { w_ctr , h_ctr + 100.f };
	const vec2 YES_POS = { width / 4 , h_ctr + 10.f };
	const vec2 NO_POS = { width / 2 , h_ctr + 55.f };
	const vec2 KEY_POS = { 45.f, 22.f };
	const vec2 GOLDCOIN_POS = { 40.f, 50.f };
	const vec2 SCORE_DIGIT1_POS = { 55.f, 40.f };
	const vec2 SCORE_DIGIT2_POS = { 61.5f, 40.f };
	const vec2 HEALTH_POS = {55.f, 60.f};

	const vec2 SCORE_SCALE = { 0.2f, 0.2f };
	const vec2 LEVELBOX_SCALE = { 0.2f, 0.2f };
	const vec2 HEALTH_SCALE = {0.25f, 0.2f};

	const float extra_sclar = 0.5;

	const float INTERVAL = 50.f; //level selection box interval
#elif __APPLE__
	const int width = 1200;
	const int height = 600;
	const int w_ctr = width / 2 ;
	const int h_ctr = height / 2 ;
	const vec2 START_POS = { w_ctr , h_ctr };
	const vec2 QUIT_POS = { w_ctr , h_ctr + 100.f };
	const vec2 BACK_POS = { w_ctr + 400.f, h_ctr + 200.f };
	const vec2 LEVEL_POS = { w_ctr - 200.f, h_ctr };
	const vec2 SAVE_POS = { width / 2 , h_ctr - 60.f };
	const vec2 RESUME_POS = { w_ctr , h_ctr + 10.f };
	const vec2 RESTART_POS = { w_ctr , h_ctr + 80.f };
	const vec2 MENU_POS = { w_ctr , h_ctr + 150.f };
	const vec2 OK_POS = { w_ctr , h_ctr + 200.f };
	const vec2 YES_POS = { width / 2 , h_ctr + 20.f };
	const vec2 NO_POS = { width / 2 , h_ctr + 110.f };
	const vec2 KEY_POS = { 90.f, 45.f };
	const vec2 GOLDCOIN_POS = { 80.f, 80.f };
	const vec2 SCORE_DIGIT1_POS = { 110.f, 80.f };
	const vec2 SCORE_DIGIT2_POS = { 123.f, 80.f };
	const vec2 HEALTH_POS = {110.f, 120.f};

	const vec2 SCORE_SCALE = { 0.4f, 0.4f };
	const vec2 LEVELBOX_SCALE = { 0.4f, 0.4f };
	const vec2 HEALTH_SCALE = {0.5, 0.4f};

	const float extra_sclar = 1.0;
	const float INTERVAL = 100.f; //level selection box interval
#elif __linux__
    // linux
#endif

Texture Icon::icon_texture;
Texture Icon::icon_texture_start;
Texture Icon::icon_texture_quit;
Texture Icon::icon_texture_back;
Texture Icon::icon_texture_pause;
Texture Icon::icon_texture_info;
Texture Icon::icon_texture_resume;
Texture Icon::icon_texture_menu;
Texture Icon::icon_texture_ok;
Texture Icon::icon_texture_restart;
Texture Icon::icon_texture_yes;
Texture Icon::icon_texture_no;
Texture Icon::icon_texture_save;
Texture Icon::icon_texture_l0;
Texture Icon::icon_texture_l1;
Texture Icon::icon_texture_l2;
Texture Icon::icon_texture_l3;
Texture Icon::icon_texture_l4;
Texture Icon::icon_texture_l5;
Texture Icon::icon_texture_s0;
Texture Icon::icon_texture_s1;
Texture Icon::icon_texture_s2;
Texture Icon::icon_texture_s3;
Texture Icon::icon_texture_s4;
Texture Icon::icon_texture_s5;
Texture Icon::icon_texture_s6;
Texture Icon::icon_texture_s7;
Texture Icon::icon_texture_s8;
Texture Icon::icon_texture_s9;
Texture Icon::icon_texture_key;
Texture Icon::icon_texture_goldcoin;
Texture Icon::icon_texture_h0;
Texture Icon::icon_texture_h1;
Texture Icon::icon_texture_h2;
Texture Icon::icon_texture_h3;


bool Icon::init(char type)
{
	char const *path;
	//load textures and set position
		switch(type){
			case (char) '0':
			{
				char const *path = ui_path("1.png");
				load_texture(icon_texture_l0, path);
				m_width = icon_texture_l0.width;
				m_height = icon_texture_l0.height;
				m_scale = LEVELBOX_SCALE;
				m_position = {LEVEL_POS.x , LEVEL_POS.y};
				break;
			}

			case (char) '1':
			{
				char const *path = ui_path("2.png");
				load_texture(icon_texture_l1, path);
				m_width = icon_texture_l1.width;
				m_height = icon_texture_l1.height;
				m_position = {LEVEL_POS.x + ((int)type - 48) * INTERVAL, LEVEL_POS.y};
				m_scale = LEVELBOX_SCALE;
				break;
			}

			case (char) '2':
			{
				char const *path = ui_path("3.png");
				load_texture(icon_texture_l2, path);
				m_width = icon_texture_l2.width;
				m_height = icon_texture_l2.height;
				m_position = {LEVEL_POS.x + ((int)type - 48) * INTERVAL, LEVEL_POS.y};
				m_scale = LEVELBOX_SCALE;
				break;
			}

			case (char) '3':
			{
				char const *path = ui_path("4.png");
				load_texture(icon_texture_l3, path);
				m_width = icon_texture_l3.width;
				m_height = icon_texture_l3.height;
				m_position = {LEVEL_POS.x + ((int)type - 48) * INTERVAL ,LEVEL_POS.y};
				m_scale = LEVELBOX_SCALE;
				break;
			}

			case (char) '4':
			{
				char const *path = ui_path("5.png");
				load_texture(icon_texture_l4, path);
				m_width = icon_texture_l4.width;
				m_height = icon_texture_l4.height;
				m_position = {LEVEL_POS.x + ((int)type - 48) * INTERVAL, LEVEL_POS.y};
				m_scale = LEVELBOX_SCALE;
				break;
			}

			case (char) '5':
			{
				char const *path = ui_path("6.png");
				load_texture(icon_texture_l5, path);
				m_width = icon_texture_l5.width;
				m_height = icon_texture_l5.height;
				m_position = {LEVEL_POS.x + ((int)type - 48) * INTERVAL, LEVEL_POS.y};
				m_scale = LEVELBOX_SCALE;
				break;
			}

			case (char)'s':
			{
				char const *path = ui_path("start.png");
				load_texture(icon_texture_start, path);
				m_width = icon_texture_start.width;
				m_height = icon_texture_start.height;
				m_position = START_POS;
				m_scale = { extra_sclar * 0.45f, extra_sclar * 0.45f};
				break;
			}

			case (char)'q':
			{
				char const *path = ui_path("quit.png");
				load_texture(icon_texture_quit,path);
				m_width = icon_texture_quit.width;
				m_height = icon_texture_quit.height;
				m_position = QUIT_POS;
				m_scale = {extra_sclar * 0.45f, extra_sclar * 0.45f};
				break;
			}

			case (char)'b':
			{
				char const *path = ui_path("back.png");
				load_texture(icon_texture_back, path);
				m_width = icon_texture_back.width;
				m_height = icon_texture_back.height;
				m_position = BACK_POS;
				m_scale = { extra_sclar * 0.45f, extra_sclar * 0.45f};
				break;
			}

			case (char)'p':
			{
				char const *path = ui_path("pause.png");
				load_texture(icon_texture_pause, path);
				m_width = icon_texture_pause.width;
				m_height = icon_texture_pause.height;
				m_position = {width - 80.f, 45.f};
				m_scale = { extra_sclar * 0.25f, extra_sclar * 0.25f};
				break;
			}

			case (char)'i':
			{
				char const *path = ui_path("info.png");
				load_texture(icon_texture_info, path);
				m_width = icon_texture_info.width;
				m_height = icon_texture_info.height;
				m_position = {width - 150.f, 45.f};
				m_scale = { extra_sclar * 0.25f, extra_sclar * 0.25f};
				break;
			}

			case (char)'c':
			{
				char const *path = ui_path("resume.png");
				load_texture(icon_texture_resume, path);
				m_width = icon_texture_resume.width;
				m_height = icon_texture_resume.height;
				m_position = RESUME_POS;
				m_scale = { extra_sclar * 0.45f, extra_sclar * 0.45f};
				break;
			}

			case (char)'r':
			{
				char const *path = ui_path("restart.png");
				load_texture(icon_texture_restart, path);
				m_width = icon_texture_restart.width;
				m_height = icon_texture_restart.height;
				m_position = RESTART_POS;
				m_scale = { extra_sclar * 0.45f, extra_sclar * 0.45f};
				break;
			}

			case (char)'m':
			{
				char const *path = ui_path("menu.png");
				load_texture(icon_texture_menu, path);
				m_width = icon_texture_menu.width;
				m_height = icon_texture_menu.height;
				m_position = MENU_POS;
				m_scale = { extra_sclar * 0.45f, extra_sclar * 0.45f};
				break;
			}

			case (char)'o':
			{
				char const *path = ui_path("ok.png");
				load_texture(icon_texture_ok, path);
				m_width = icon_texture_ok.width;
				m_height = icon_texture_ok.height;
				m_position = OK_POS;
				m_scale = { extra_sclar * 0.45f, extra_sclar * 0.45f};
				break;
			}

			case (char)'y':
			{
				char const *path = ui_path("yes.png");
				load_texture(icon_texture_yes, path);
				m_width = icon_texture_yes.width;
				m_height = icon_texture_yes.height;
				m_position = YES_POS;
				m_scale = { extra_sclar * 0.45f, extra_sclar * 0.45f};
				break;
			}

			case (char)'n':
			{
				char const *path = ui_path("no.png");
				load_texture(icon_texture_no, path);
				m_width = icon_texture_no.width;
				m_height = icon_texture_no.height;
				m_position = NO_POS;
				m_scale = { extra_sclar * 0.45f, extra_sclar * 0.45f};
				break;
			}

			case (char)'v': //save
			{
				char const *path = ui_path("save.png");
				load_texture(icon_texture_save, path);
				m_width = icon_texture_save.width;
				m_height = icon_texture_save.height;
				m_position = SAVE_POS;
				m_scale = { extra_sclar * 0.45f, extra_sclar * 0.45f};
				break;
			}

			case (char)'k': //key
			{
				char const *path = textures_path("key_transparent.png");
				load_texture(icon_texture_key, path);
				m_width = icon_texture_key.width;
				m_height = icon_texture_key.height;
				m_position = KEY_POS;
				m_scale = { extra_sclar * 0.35f, extra_sclar * 0.35f};
				break;
			}

			case (char)'g': //goldcoin
			{
				char const *path = gold_coin_path("goldCoin0.png");
				load_texture(icon_texture_goldcoin, path);
				m_width = icon_texture_goldcoin.width;
				m_height = icon_texture_goldcoin.height;
				m_position = GOLDCOIN_POS;
				m_scale = { extra_sclar * 0.5f, extra_sclar *  0.5f};
				break;
			}

			case (char)'A': //score 0
			{
				char const *path = score_path("0.png");
				load_texture(icon_texture_s0, path);
				m_width = icon_texture_s0.width;
				m_height = icon_texture_s0.height;
				m_scale = SCORE_SCALE;
				break;
			}
			case (char)'B': //score 1
			{
				char const *path = score_path("1.png");
				load_texture(icon_texture_s1, path);
				m_width = icon_texture_s1.width;
				m_height = icon_texture_s1.height;
				m_scale = SCORE_SCALE;
				break;
			}
			case (char)'C': //score 2
			{
				char const *path = score_path("2.png");
				load_texture(icon_texture_s2, path);
				m_width = icon_texture_s2.width;
				m_height = icon_texture_s2.height;
				m_scale = SCORE_SCALE;
				break;
			}
			case (char)'D': //score 3
			{
				char const *path = score_path("3.png");
				load_texture(icon_texture_s3, path);
				m_width = icon_texture_s3.width;
				m_height = icon_texture_s3.height;
				m_scale = SCORE_SCALE;
				break;
			}
			case (char)'E': //score 4
			{
				char const *path = score_path("4.png");
				load_texture(icon_texture_s4, path);
				m_width = icon_texture_s4.width;
				m_height = icon_texture_s4.height;
				m_scale = SCORE_SCALE;
				break;
			}
			case (char)'F': //score 5
			{
				char const *path = score_path("5.png");
				load_texture(icon_texture_s5, path);
				m_width = icon_texture_s5.width;
				m_height = icon_texture_s5.height;
				m_scale = SCORE_SCALE;
				break;
			}
			case (char)'G': //score 6
			{
				char const *path = score_path("6.png");
				load_texture(icon_texture_s6, path);
				m_width = icon_texture_s6.width;
				m_height = icon_texture_s6.height;
				m_scale = SCORE_SCALE;
				break;
			}
			case (char)'H': //score 7
			{
				char const *path = score_path("7.png");
				load_texture(icon_texture_s7, path);
				m_width = icon_texture_s7.width;
				m_height = icon_texture_s7.height;
				m_scale = SCORE_SCALE;
				break;
			}
			case (char)'I': //score 8
			{
				char const *path = score_path("8.png");
				load_texture(icon_texture_s8, path);
				m_width = icon_texture_s8.width;
				m_height = icon_texture_s8.height;
				m_scale = SCORE_SCALE;
				break;
			}
			case (char)'J': //score 9
			{
				char const *path = score_path("9.png");
				load_texture(icon_texture_s9, path);
				m_width = icon_texture_s9.width;
				m_height = icon_texture_s9.height;
				m_scale = SCORE_SCALE;
				break;
			}
			case (char)'W': //health 0 - empty
			{
				char const *path = health_path("health_0.png");
				load_texture(icon_texture_h0, path);
				m_width = icon_texture_h0.width;
				m_height = icon_texture_h0.height;
				m_position = HEALTH_POS;
				m_scale = HEALTH_SCALE;
				break;
			}
			case (char)'X': //health 1
			{
				char const *path = health_path("health_1.png");
				load_texture(icon_texture_h1, path);
				m_width = icon_texture_h1.width;
				m_height = icon_texture_h1.height;
				m_position = HEALTH_POS;
				m_scale = HEALTH_SCALE;
				break;
			}
			case (char)'Y': //health 2 
			{
				char const *path = health_path("health_2.png");
				load_texture(icon_texture_h2, path);
				m_width = icon_texture_h2.width;
				m_height = icon_texture_h2.height;
				m_position = HEALTH_POS;
				m_scale = HEALTH_SCALE;
				break;
			}
			case (char)'Z': //health 3 - full
			{
				char const *path = health_path("health_3.png");
				load_texture(icon_texture_h3, path);
				m_width = icon_texture_h3.width;
				m_height = icon_texture_h3.height;
				m_position = HEALTH_POS;
				m_scale = HEALTH_SCALE;
				break;
			}
	}

	// The position corresponds to the center of the texture (since if you halve the height u get the middle
	float wr = m_width * 0.5f;
	float hr = m_height * 0.5f;

	//goldcoin animation spritesheet
	if(type == 'g')
		wr = m_width * 0.5f / 9; //c_division = 9

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

	if(type == 'g'){
		//load animation fragment shader for goldcoin
		if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("motion.fs.glsl")))
			return false;
	}else{
		// Load regular shaders
		if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
			return false;
	}

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_rotation = 0.f;
	m_type = type;
	m_hover = false;
	//delete(path);
	return true;

}

bool Icon::load_texture(Texture& texture, const char* path){
	if (!texture.is_valid()){
		if (!icon_texture.load_from_file(path)
			|| ! texture.load_from_file(path))
		{
			fprintf(stderr, "Failed to load icon texture!");
			std::cout << (void *) path;
			return false;
		}
	}
	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Icon::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);

	effect.release(); // according to https://piazza.com/class/jp1oa2b3dx04b4?cid=257
}

void Icon::draw(const mat3& projection)
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
	
	//goldcoin animation
	if(get_type() == 'g'){ 
		int c_division = 9;
		GLuint division_uloc = glGetUniformLocation(effect.program, "division");
		GLuint time_uloc = glGetUniformLocation(effect.program, "time");
		glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
		glUniform1f(division_uloc, (float)(c_division * 1.0f));
	}

	// Enabling and binding texture to slot 0
	char type = get_type();
	glActiveTexture(GL_TEXTURE0);

	switch(type){
		case(char) '0':
			glBindTexture(GL_TEXTURE_2D, icon_texture_l0.id);
			break;
		case(char) '1':
			glBindTexture(GL_TEXTURE_2D, icon_texture_l1.id);
			break;
		case(char) '2':
			glBindTexture(GL_TEXTURE_2D, icon_texture_l2.id);
			break;
		case(char) '3':
			glBindTexture(GL_TEXTURE_2D, icon_texture_l3.id);
			break;
		case(char) '4':
			glBindTexture(GL_TEXTURE_2D, icon_texture_l4.id);
			break;
		case(char) '5':
			glBindTexture(GL_TEXTURE_2D, icon_texture_l5.id);
			break;
		case(char) 's':
			glBindTexture(GL_TEXTURE_2D, icon_texture_start.id);
			break;
		case(char) 'q':
			glBindTexture(GL_TEXTURE_2D, icon_texture_quit.id);
			break;
		case(char) 'b':
			glBindTexture(GL_TEXTURE_2D, icon_texture_back.id);
			break;
		case(char) 'p':
			glBindTexture(GL_TEXTURE_2D, icon_texture_pause.id);
			break;
		case(char) 'i':
			glBindTexture(GL_TEXTURE_2D, icon_texture_info.id);
			break;
		case(char) 'c':
			glBindTexture(GL_TEXTURE_2D, icon_texture_resume.id);
			break;
		case(char) 'm':
			glBindTexture(GL_TEXTURE_2D, icon_texture_menu.id);
			break;
		case(char) 'o':
			glBindTexture(GL_TEXTURE_2D, icon_texture_ok.id);
			break;
		case(char) 'r':
			glBindTexture(GL_TEXTURE_2D, icon_texture_restart.id);
			break;
		case(char) 'y':
			glBindTexture(GL_TEXTURE_2D, icon_texture_yes.id);
			break;
		case(char) 'n':
			glBindTexture(GL_TEXTURE_2D, icon_texture_no.id);
			break;
		case(char) 'v':
			glBindTexture(GL_TEXTURE_2D, icon_texture_save.id);
			break;
		case(char) 'k':
			glBindTexture(GL_TEXTURE_2D, icon_texture_key.id);
			break;
		case(char) 'g':
			glBindTexture(GL_TEXTURE_2D, icon_texture_goldcoin.id);
			break;
		case(char) 'A':
			glBindTexture(GL_TEXTURE_2D, icon_texture_s0.id);
			break;
		case(char) 'B':
			glBindTexture(GL_TEXTURE_2D, icon_texture_s1.id);
			break;
		case(char) 'C':
			glBindTexture(GL_TEXTURE_2D, icon_texture_s2.id);
			break;
		case(char) 'D':
			glBindTexture(GL_TEXTURE_2D, icon_texture_s3.id);
			break;
		case(char) 'E':
			glBindTexture(GL_TEXTURE_2D, icon_texture_s4.id);
			break;
		case(char) 'F':
			glBindTexture(GL_TEXTURE_2D, icon_texture_s5.id);
			break;
		case(char) 'G':
			glBindTexture(GL_TEXTURE_2D, icon_texture_s6.id);
			break;
		case(char) 'H':
			glBindTexture(GL_TEXTURE_2D, icon_texture_s7.id);
			break;
		case(char) 'I':
			glBindTexture(GL_TEXTURE_2D, icon_texture_s8.id);
			break;
		case(char) 'J':
			glBindTexture(GL_TEXTURE_2D, icon_texture_s9.id);
			break;
		case(char) 'W':
			glBindTexture(GL_TEXTURE_2D, icon_texture_h0.id);
			break;
		case(char) 'X':
			glBindTexture(GL_TEXTURE_2D, icon_texture_h1.id);
			break;
		case(char) 'Y':
			glBindTexture(GL_TEXTURE_2D, icon_texture_h2.id);
			break;
		case(char) 'Z':
			glBindTexture(GL_TEXTURE_2D, icon_texture_h3.id);
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

void Icon::mouse_hover_on(){
	if(!m_hover){
		m_hover = true;
		switch_to_hover();
	}
}

void Icon::mouse_hover_off(){
	if(m_hover){
		m_hover = false;
		switch_to_normal();
	}
}

void Icon::update_key_status(bool visibility){
	if(visibility){
		icon_texture_key.load_from_file(textures_path("key.png"));
	}else{
		icon_texture_key.load_from_file(textures_path("key_transparent.png"));
	}
}

void Icon::switch_to_hover(){
	switch(m_type){
		case(char) '0':
			icon_texture_l0.load_from_file(ui_path("1_hover.png"));
			break;
		case(char) '1':
			icon_texture_l1.load_from_file(ui_path("2_hover.png"));
			break;
		case(char) '2':
			icon_texture_l2.load_from_file(ui_path("3_hover.png"));
			break;
		case(char) '3':
			icon_texture_l3.load_from_file(ui_path("4_hover.png"));
			break;
		case(char) '4':
			icon_texture_l4.load_from_file(ui_path("5_hover.png"));
			break;
		case(char) '5':
			icon_texture_l5.load_from_file(ui_path("6_hover.png"));
			break;
		case(char) 's':
			icon_texture_start.load_from_file(ui_path("start_hover.png"));
			break;
		case(char) 'q':
			icon_texture_quit.load_from_file(ui_path("quit_hover.png"));
			break;
		case(char) 'b':
			icon_texture_back.load_from_file(ui_path("back_hover.png"));
			break;
		case(char) 'p':
			icon_texture_pause.load_from_file(ui_path("pause_hover.png"));
			break;
		case(char) 'i':
			icon_texture_info.load_from_file(ui_path("info_hover.png"));
			break;
		case(char) 'c':
			icon_texture_resume.load_from_file(ui_path("resume_hover.png"));
			break;
		case(char) 'm':
			icon_texture_menu.load_from_file(ui_path("menu_hover.png"));
			break;
		case(char) 'o':
			icon_texture_ok.load_from_file(ui_path("ok_hover.png"));
			break;
		case(char) 'r':
			icon_texture_restart.load_from_file(ui_path("restart_hover.png"));
			break;
		case(char) 'y':
			icon_texture_yes.load_from_file(ui_path("yes_hover.png"));
			break;
		case(char) 'n':
			icon_texture_no.load_from_file(ui_path("no_hover.png"));
			break;
		case(char) 'v':
			icon_texture_save.load_from_file(ui_path("save_hover.png"));
			break;
		}
}
void Icon::switch_to_normal(){
switch(m_type){
		case(char) '0':
			icon_texture_l0.load_from_file(ui_path("1.png"));
			break;
		case(char) '1':
			icon_texture_l1.load_from_file(ui_path("2.png"));
			break;
		case(char) '2':
			icon_texture_l2.load_from_file(ui_path("3.png"));
			break;
		case(char) '3':
			icon_texture_l3.load_from_file(ui_path("4.png"));
			break;
		case(char) '4':
			icon_texture_l4.load_from_file(ui_path("5.png"));
			break;
		case(char) '5':
			icon_texture_l5.load_from_file(ui_path("6.png"));
			break;
		case(char) 's':
			icon_texture_start.load_from_file(ui_path("start.png"));
			break;
		case(char) 'q':
			icon_texture_quit.load_from_file(ui_path("quit.png"));
			break;
		case(char) 'b':
			icon_texture_back.load_from_file(ui_path("back.png"));
			break;
		case(char) 'p':
			icon_texture_pause.load_from_file(ui_path("pause.png"));
			break;
		case(char) 'i':
			icon_texture_info.load_from_file(ui_path("info.png"));
			break;
		case(char) 'c':
			icon_texture_resume.load_from_file(ui_path("resume.png"));
			break;
		case(char) 'm':
			icon_texture_menu.load_from_file(ui_path("menu.png"));
			break;
		case(char) 'o':
			icon_texture_ok.load_from_file(ui_path("ok.png"));
			break;
		case(char) 'r':
			icon_texture_restart.load_from_file(ui_path("restart.png"));
			break;
		case(char) 'y':
			icon_texture_yes.load_from_file(ui_path("yes.png"));
			break;
		case(char) 'n':
			icon_texture_no.load_from_file(ui_path("no.png"));
			break;
		case(char) 'v':
			icon_texture_save.load_from_file(ui_path("save.png"));
			break;
		}

}

vec2 Icon::get_position()const
{
	return m_position;
}

char Icon::get_type()const
{
	return m_type;
}

void Icon::set_position(vec2 position) {
	m_position = position;
}

//hardcoded for setting digit
void Icon::set_digit_position(int digit){
	if(digit == 0)
		m_position = SCORE_DIGIT1_POS;
	else if(digit == 1)
		m_position = SCORE_DIGIT2_POS;
}

// Returns the local bounding coordinates scaled by the current size of the icon
vec2 Icon::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * m_width, std::fabs(m_scale.y) * m_height };
}