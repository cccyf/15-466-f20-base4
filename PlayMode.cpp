#include "PlayMode.hpp"

// #include "LitColorTextureProgram.hpp"

#include "ColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode(){
	//----- allocate OpenGL resources -----
	{ //vertex buffer:
		glGenBuffers(1, &vertex_buffer); // a name, like a pointer
		//for now, buffer will be un-filled.

		GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
	}

	{ //vertex array mapping buffer for color_texture_program:
		//ask OpenGL to fill vertex_buffer_for_color_texture_program with the name of an unused vertex array object:
		glGenVertexArrays(1, &vertex_buffer_for_color_texture_program);

		//set vertex_buffer_for_color_texture_program as the current vertex array object:
		glBindVertexArray(vertex_buffer_for_color_texture_program);

		//set vertex_buffer as the source of glVertexAttribPointer() commands:
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		// type, name
		// data of this type -> go to the name

		//set up the vertex array object to describe arrays of PongMode::Vertex:
		glVertexAttribPointer(//todo
			color_texture_program.Position_vec4, //attribute
			3, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(Vertex), //stride
			(GLbyte *)0 + 0 //offset
		);
		// open this buffer object
		glEnableVertexAttribArray(color_texture_program.Position_vec4);
		//[Note that it is okay to bind a vec3 input to a vec4 attribute -- the w component will be filled with 1.0 automatically]

		glVertexAttribPointer(
			color_texture_program.Color_vec4, //attribute
			4, //size
			GL_UNSIGNED_BYTE, //type
			GL_TRUE, //normalized
			sizeof(Vertex), //stride
			(GLbyte *)0 + 4*3 //offset
		);
		glEnableVertexAttribArray(color_texture_program.Color_vec4);

		glVertexAttribPointer(
			color_texture_program.TexCoord_vec2, //attribute
			2, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(Vertex), //stride
			(GLbyte *)0 + 4*3 + 4*1 //offset
		);
		glEnableVertexAttribArray(color_texture_program.TexCoord_vec2);

		// TODO
		//done referring to vertex_buffer, so unbind it:
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//done setting up vertex array object, so unbind it:
		glBindVertexArray(0);

		GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
	}

	{ //solid white texture:
		//ask OpenGL to fill white_tex with the name of an unused texture object:
		glGenTextures(1, &white_tex);

		//bind that texture object as a GL_TEXTURE_2D-type texture:
		glBindTexture(GL_TEXTURE_2D, white_tex);

		//upload a 1x1 image of solid white to the texture:
		glm::uvec2 size = glm::uvec2(1,1);
		// glm u8vec4 color
		std::vector< glm::u8vec4 > data(size.x*size.y, glm::u8vec4(0xff, 0xff, 0xff, 0xff));
		// generate a 2d texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

		//set filtering and wrapping parameters:
		//(it's a bit silly to mipmap a 1x1 texture, but I'm doing it because you may want to use this code to load different sizes of texture)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// s-x, t-y
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//since texture uses a mipmap and we haven't uploaded one, instruct opengl to make one for us:
		glGenerateMipmap(GL_TEXTURE_2D);

		//Okay, texture uploaded, can unbind it:
		glBindTexture(GL_TEXTURE_2D, 0);

		GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
	}
}

PlayMode::~PlayMode() {

		//----- free OpenGL resources -----
	glDeleteBuffers(1, &vertex_buffer);
	vertex_buffer = 0;

	glDeleteVertexArrays(1, &vertex_buffer_for_color_texture_program);
	vertex_buffer_for_color_texture_program = 0;

	glDeleteTextures(1, &white_tex);
	white_tex = 0;
	
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		
	}
	return false;
}

void PlayMode::update(float elapsed) {

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	// #define HEX_TO_U8VEC4( HX ) (glm::u8vec4( (HX >> 24) & 0xff, (HX >> 16) & 0xff, (HX >> 8) & 0xff, (HX) & 0xff ))
	
	hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, "abcdefg hijklmn opqrst uvwxyz", -1, 0, -1);
	hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf, hb_language_from_string("en", -1));

	FT_Library  ft_library;
	FT_Face face;
	FT_Init_FreeType(&ft_library);

	size_t index = 0;
	FT_New_Face(ft_library, "/System/Library/Fonts/Menlo.ttc", index, &face);
	FT_Set_Char_Size(face, 0, 1200, 0, 0);
	hb_font_t *font = hb_ft_font_create(face, NULL);

	hb_shape(font, buf, NULL, 0);

	unsigned int glyph_count;
	hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
	hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

	float cursor_x = -1.0f;
	float cursor_y = 0.0f;
	
	float sx = 2.0 / 1280;
	float sy = 2.0 / 720;
	GLuint texture{0}, sampler{0};
    GLuint vbo{0}, vao{0};
    GLuint vs{0}, fs{0}, program{0};
	const char *VERTEX_SHADER = ""
        "#version 410 core\n"
		"in vec4 in_Position;\n"
        "out vec2 texCoords;\n"
        "void main(void) {\n"
        "    gl_Position = vec4(in_Position.xy, 0, 1);\n"
        "    texCoords = in_Position.zw;\n"
        "}\n";


	const char *FRAGMENT_SHADER = ""
        "#version 410 core\n"
        "precision highp float;\n"
        "uniform sampler2D tex;\n"
        "uniform vec4 color;\n"
        "in vec2 texCoords;\n"
        "out vec4 fragColor;\n"
        "void main(void) {\n"
        "    fragColor = vec4(1, 1, 1, texture(tex, texCoords).r) * color;\n"
        "}\n";

	// Initialize our texture and VBOs
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

	glGenTextures(1, &texture);
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Initialize shader
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &VERTEX_SHADER, 0);
    glCompileShader(vs);

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &FRAGMENT_SHADER, 0);
    glCompileShader(fs);

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Set some initialize GL state
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1, 0.2, 0.4, 0);

// Get shader uniforms
    glUseProgram(program);
    glBindAttribLocation(program, 0, "in_Position");
    GLuint texUniform = glGetUniformLocation(program, "tex");
    GLuint colorUniform = glGetUniformLocation(program, "color");
	
	// Bind Stuff
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, sampler);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glUseProgram(program);
	glUniform4f(colorUniform, 1, 1, 1, 1);
	glUniform1i(texUniform, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	const FT_GlyphSlot glyph = face->glyph;

	for (size_t i = 0; i < glyph_count; ++i) {
		hb_codepoint_t glyphid = glyph_info[i].codepoint;
		float x_offset = glyph_pos[i].x_offset / 64.0;
		float y_offset = glyph_pos[i].y_offset / 64.0;
		float x_advance = glyph_pos[i].x_advance / 64.0;
		float y_advance = glyph_pos[i].y_advance / 64.0;

		if(FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT) != 0)
            continue;

		if (FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL) != 0) {
			continue;
		}

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                     glyph->bitmap.width, glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

        const float vx = cursor_x + x_offset + glyph->bitmap_left * sx;
        const float vy = cursor_y + y_offset + glyph->bitmap_top * sy;
        const float w = glyph->bitmap.width * sx;
        const float h = glyph->bitmap.rows * sy;

        struct {
            float x, y, s, t;
        } data[6] = {
            {vx    , vy    , 0, 0},
            {vx    , vy - h, 0, 1},
            {vx + w, vy    , 1, 0},
            {vx + w, vy    , 1, 0},
            {vx    , vy - h, 0, 1},
			 {vx + w, vy - h, 1, 1}
        };

        glBufferData(GL_ARRAY_BUFFER, 24*sizeof(float), data, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

		cursor_x += x_advance * sx;
		cursor_y += y_advance * sy;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	hb_buffer_destroy(buf);
    hb_font_destroy(font);
	//---- actual drawing ----

	//clear the color buffer:
	// const glm::u8vec4 bg_color = HEX_TO_U8VEC4(0xf7f2ebff);
	// glClearColor(bg_color.r / 255.0f, bg_color.g / 255.0f, bg_color.b / 255.0f, bg_color.a / 255.0f);
	// glClear(GL_COLOR_BUFFER_BIT);
	// GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.

	// //use alpha blending:
	// glEnable(GL_BLEND);
	// GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// //don't use the depth test:
	// GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.
	// glDisable(GL_DEPTH_TEST);
	// GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.

	//upload vertices to vertex_buffer:
	// glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //set vertex_buffer as current
	// // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STREAM_DRAW); //upload vertices array
	// glBindBuffer(GL_ARRAY_BUFFER, 0);

	//set color_texture_program as current program:
	// glUseProgram(color_texture_program.program);
	// std::cout << "pid: " << color_texture_program.program << "\n";
	// GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.
	
    // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(720.f), 0.0f, static_cast<float>(600.f));
	// //upload OBJECT_TO_CLIP to the proper uniform location:
	// glUniformMatrix4fv(color_texture_program.OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(projection));
	// GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.

	// //use the mapping vertex_buffer_for_color_texture_program to fetch vertex data:
	// // glBindVertexArray(vertex_buffer_for_color_texture_program);

	// // //bind the solid white texture to location zero so things will be drawn just with their colors:
	// // glActiveTexture(GL_TEXTURE0);
	// // glBindTexture(GL_TEXTURE_2D, white_tex);

	// textTexture.DrawText(&color_texture_program, 25.0f, 25.0f, 1.0f);

	// //run the OpenGL pipeline:
	// // glDrawArrays(GL_TRIANGLES, 0, GLsizei(vertices.size()));
	// GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.

	// //unbind the solid white texture:
	// // glBindTexture(GL_TEXTURE_2D, 0);

	// // //reset vertex array to none:
	// // glBindVertexArray(0);

	// // //reset current program to none:
	// glUseProgram(0);

	// GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.

	{ //use DrawLines to overlay some text:
		
		// float aspect = float(drawable_size.x) / float(drawable_size.y);
		// DrawLines lines(glm::mat4(
		// 	1.0f / aspect, 0.0f, 0.0f, 0.0f,
		// 	0.0f, 1.0f, 0.0f, 0.0f,
		// 	0.0f, 0.0f, 1.0f, 0.0f,
		// 	0.0f, 0.0f, 0.0f, 1.0f
		// ));

		// constexpr float H = 0.09f;
		// lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
		// 	glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
		// 	glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
		// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		// float ofs = 2.0f / drawable_size.y;
		// lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
		// 	glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
		// 	glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
		// 	glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
	GL_ERRORS();
}

