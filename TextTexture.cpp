
#include "TextTexture.hpp"
#include <iostream>
#include "gl_errors.hpp"
#include <glm/gtc/type_ptr.hpp>

#include FT_FREETYPE_H

TextTexture::TextTexture(){
      char fn [50] = "/System/Library/Fonts/AquaKana.ttc";
      unsigned int index = 0;

      buf = hb_buffer_create();

      hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
      hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
      hb_buffer_set_language(buf, hb_language_from_string("en", -1));
      
      filename      = fn;                           /* first argument     */
      text          = txt;                           /* second argument    */
      num_chars     = strlen( text );
      
      if (FT_Init_FreeType(&library))
      {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return;
      }

      if (FT_New_Face(library, filename, index, &face))
      {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
            return;
      }
      
      /* use 50pt at 100dpi */
      // error = FT_Set_Char_Size( face, 50 * 64, 0,
      //                         100, 0 );                /* set character size */
      // /* error handling omitted */

      // font = hb_ft_font_create(face, nullptr);

      // hb_shape(font, buf, NULL, 0);

      // FT_Face face;
	FT_Set_Char_Size(face, 0, 16*64, 360, 400);
	font = hb_ft_font_create(face, NULL);
	
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
}

TextTexture::~TextTexture(){
      FT_Done_Face(face);
      FT_Done_FreeType(library);

      hb_buffer_destroy(buf);
      hb_font_destroy(font);
}

void TextTexture::Update(char* update_txt){
      strcpy(txt, update_txt);
      txt[strlen(update_txt)] = 0;
}

void TextTexture::DrawText(float cursor_x, float cursor_y, float scale_x, float scale_y){
      cursor_x = cursor_x < -1.0 ? -0.5 : cursor_x;
      cursor_y = cursor_y < -1.0 ? 0.5 : cursor_y;
      
      hb_buffer_clear_contents(buf);
      // buf = hb_buffer_create();
      // hb_buffer_clear_contents(buf); 
      hb_buffer_add_utf8(buf, txt, -1, 0, -1);
      hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf, hb_language_from_string("en", -1));

	hb_shape(font, buf, NULL, 0);

      unsigned int glyph_count;
	hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
	hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

      GLuint texture{0}, sampler{0};
      GLuint vbo{0}, vao{0};

      // Initialize our texture and VBOs
      glGenBuffers(1, &vbo);
      glGenVertexArrays(1, &vao);

      glGenTextures(1, &texture);
      glGenSamplers(1, &sampler);
      glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      // Set some initialize GL state
      glEnable(GL_BLEND);
      glDisable(GL_CULL_FACE);
      glDisable(GL_DEPTH_TEST);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glClearColor(0, 0.2, 0.2, 0);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

      unsigned int cur_start = 0;

      while (cur_start < glyph_count)
      {
            unsigned int cur_end;
            float cur_offset = 0.f;
            int seen_blankspace = 0;

            for (cur_end = cur_start; cur_end < glyph_count && (cur_offset <= 1.f || glyph_info[cur_end].codepoint != 3); cur_end += 1)
            {
                 cur_offset += glyph_pos[cur_end].x_advance / 64.0 * scale_x;
                 if (glyph_info[cur_end].codepoint == 0)
                 {
                       seen_blankspace = 1;
                       break;
                 }
                 
            }
            
            cursor_x = -cur_offset/2;
            float max_h = 0.05;
            for (size_t i = cur_start; i < cur_end; i++)
            {
                  hb_codepoint_t glyphid = glyph_info[i].codepoint;
                  float x_offset = glyph_pos[i].x_offset / 64.0;
                  float y_offset = glyph_pos[i].y_offset / 64.0;
                  float x_advance = glyph_pos[i].x_advance / 64.0;
                  float y_advance = glyph_pos[i].y_advance / 64.0;

                  if(FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT) != 0){
                        continue;
                  }

                  if (FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL) != 0) {
                        continue;
                  }

                  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                        glyph->bitmap.width, glyph->bitmap.rows,
                        0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

                  const float vx = cursor_x + x_offset + glyph->bitmap_left * scale_x;
                  const float vy = cursor_y + y_offset + glyph->bitmap_top * scale_y;
                  const float w = glyph->bitmap.width * scale_x;
                  const float h = glyph->bitmap.rows * scale_y;

                  max_h = h>max_h? h: max_h;

                  Coordi data[6] = {
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

                  cursor_x += x_advance * scale_x;
                  cursor_y += y_advance * scale_y;
                  // if (cursor_x > max_line_cursor)
                  // {
                        
                  // }
            }
            
            cursor_y -= 1.5 * max_h;

            cur_start = cur_end;
            if (seen_blankspace)
            {
                  cur_start += 1;
            }
            
      }
      

	// for (size_t i = 0; i < glyph_count; ++i) {
	// 	hb_codepoint_t glyphid = glyph_info[i].codepoint;
	// 	float x_offset = glyph_pos[i].x_offset / 64.0;
	// 	float y_offset = glyph_pos[i].y_offset / 64.0;
	// 	float x_advance = glyph_pos[i].x_advance / 64.0;
	// 	float y_advance = glyph_pos[i].y_advance / 64.0;

	// 	if(FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT) != 0){
      //             continue;
      //       }

	// 	if (FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL) != 0) {
	// 		continue;
	// 	}

      //       glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
      //                glyph->bitmap.width, glyph->bitmap.rows,
      //                0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

      //       const float vx = cursor_x + x_offset + glyph->bitmap_left * scale_x;
      //       const float vy = cursor_y + y_offset + glyph->bitmap_top * scale_y;
      //       const float w = glyph->bitmap.width * scale_x;
      //       const float h = glyph->bitmap.rows * scale_y;

      //       Coordi data[6] = {
      //             {vx    , vy    , 0, 0},
      //             {vx    , vy - h, 0, 1},
      //             {vx + w, vy    , 1, 0},
      //             {vx + w, vy    , 1, 0},
      //             {vx    , vy - h, 0, 1},
      //                   {vx + w, vy - h, 1, 1}
      //       };

      //       glBufferData(GL_ARRAY_BUFFER, 24*sizeof(float), data, GL_DYNAMIC_DRAW);
      //       glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
      //       glDrawArrays(GL_TRIANGLES, 0, 6);

      //       cursor_x += x_advance * scale_x;
      //       cursor_y += y_advance * scale_y;
      //       if (cursor_x > max_line_cursor)
      //       {
      //             cursor_x = min_line_cursor;
      //             cursor_y -= 1.5 * h;
      //       }
            
	// }

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

}