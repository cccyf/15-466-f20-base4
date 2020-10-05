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
		if (evt.key.keysym.sym == SDLK_a) {
			sceneScript.cur_chapter = sceneScript.stories[sceneScript.cur_chapter.index_a];
			return true;
		}
		if (evt.key.keysym.sym == SDLK_b)
		{
			sceneScript.cur_chapter = sceneScript.stories[sceneScript.cur_chapter.index_b];
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
	float cursor_x = -100.0f;
	float cursor_y = -100.0f;
	
	float sx = .8 / 1280;
	float sy = .8 / 720;

	// char txt[500] = "Here is the long long story. \nLong long ago, a princess was married to a king. \n\nPress 1 Press 2";
	textTexture.Update(sceneScript.cur_chapter.txt);
	textTexture.DrawText(cursor_x, cursor_y, sx, sy);
	
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

