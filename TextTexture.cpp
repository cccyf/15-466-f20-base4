
#include "TextTexture.hpp"
#include <iostream>
#include "gl_errors.hpp"
#include <glm/gtc/type_ptr.hpp>

#include FT_FREETYPE_H

TextTexture::TextTexture(){
      char fn [50] = "/System/Library/Fonts/Menlo.ttc";
      unsigned int index = 0;

      buf = hb_buffer_create();
      
      hb_buffer_add_utf8(buf, txt, -1, 0, -1);

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
      std::cout<<"before\n";

      // FT_Face face;
	FT_Set_Char_Size(face, 0, 1000, 0, 0);
	font = hb_ft_font_create(face, NULL);
	
      hb_shape(font, buf, NULL, 0);
    
        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 'a'; c < 'z'; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            std::cout << face->glyph->glyph_index << " glyph index \n";
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Gly character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            std::cout << c << " " << face->glyph->bitmap.rows << " " << face->glyph->bitmap.width << "\n";
            draw_bitmap(&face->glyph->bitmap,
                  face->glyph->bitmap_left,
                  target_height - face->glyph->bitmap_top );
                  show_image();
            text_mappings.insert(std::pair<char, Gly>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished


    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


TextTexture::Gly TextTexture::LoadTexture(char cp){
      auto tid = text_mappings.find(cp);
      if (tid != text_mappings.end())
      {
            return tid->second;
      }
      // // upload texture
      // unsigned int texture;
      // glGenTextures(1, &texture);
      // glBindTexture(GL_TEXTURE_2D, texture);

      // glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // TODO

      // glTexImage2D(
      //       GL_TEXTURE_2D,
      //       0,
      //       GL_RED,
      //       face->glyph->bitmap.width,
      //       face->glyph->bitmap.rows,
      //       0,
      //       GL_RED,
      //       GL_UNSIGNED_BYTE,
      //       face->glyph->bitmap.buffer
      // );
      // // set texture options
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   

      // Gly gly = 
      // {     
      //       texture,
      //       glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      //       glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      //       static_cast<unsigned int>(face->glyph->advance.x)
      // };

      // text_mappings.insert(std::pair<char, Gly>(cp, gly));

      exit(0);
}

// void drawGly(TextTexture::Gly gly){

// }

void render_text(const std::string &str, FT_Face face, float x, float y, float sx, float sy) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    std::cout<<str<<"\n";
    const FT_GlyphSlot glyph = face->glyph;

    for(auto c : str) {
          std::cout << c << "\n";
        if(FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            continue;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                     glyph->bitmap.width, glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

        const float vx = x + glyph->bitmap_left * sx;
        const float vy = y + glyph->bitmap_top * sy;
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

        x += (glyph->advance.x >> 6) * sx;
        y += (glyph->advance.y >> 6) * sy;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}
// const char *VERTEX_SHADER = ""
//         "#version 410 core\n"
//         "in vec4 in_Position;\n"
//         "out vec2 texCoords;\n"
//         "void main(void) {\n"
//         "    gl_Position = vec4(in_Position.xy, 0, 1);\n"
//         "    texCoords = in_Position.zw;\n"
//         "}\n";

TextTexture::~TextTexture(){
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}
// const char *FRAGMENT_SHADER = ""
//         "#version 410 core\n"
//         "precision highp float;\n"
//         "uniform sampler2D tex;\n"
//         "uniform vec4 color;\n"
//         "in vec2 texCoords;\n"
//         "out vec4 fragColor;\n"
//         "void main(void) {\n"
//         "    fragColor = vec4(1, 1, 1, texture(tex, texCoords).r) * color;\n"
//         "}\n";
void TextTexture::DrawText(ColorTextureProgram* color_texture_program, float cursor_x, float cursor_y, float scale){
      // unsigned int glyph_count;
      // hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
      // hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
      // // FT_Error      error;
      FT_Vector     pen;                    /* untransformed origin  */
      pen.x = 300 * 4;
      pen.y = ( target_height - 200 ) * 4;

      cursor_x = 0.0;
       cursor_y= 0.0;
      // FT_GlyphSlot slot;
      
      // glm::vec4 color =  glm::vec4(0.5, 0.8f, 0.2f, 1.0f);

	glUseProgram(color_texture_program->program);
      GL_ERRORS();
	// // glUseProgram(color_texture_program.program);
      // glUniform4f(glGetUniformLocation(color_texture_program->program, "Color_vec4"), color.x, color.y, color.z, color.w);
            // GL_ERRORS();
      // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
	// glUniformMatrix4fv(color_texture_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(projection));
    std::cout << 1 << "\n";
    GLuint texture{0}, sampler{0};
    GLuint vbo{0}, vao{0};
//     GLuint vs{0}, fs{0}, program{0};
    std::cout << 2 << "\n";

      glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenTextures(1, &texture);
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    std::cout << 3 << "\n";

    // Initialize shader
//     vs = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vs, 1, &VERTEX_SHADER, 0);
//     glCompileShader(vs);

//     fs = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fs, 1, &FRAGMENT_SHADER, 0);
//     glCompileShader(fs);

//     program = glCreateProgram();
//     glAttachShader(program, vs);
//     glAttachShader(program, fs);
//     glLinkProgram(program);

    // Set some initialize GL state
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1, 0.2, 0.4, 0);
    std::cout << 3 << "\n";

    // Get shader uniforms
    glBindAttribLocation(color_texture_program->program, 0, "Position_vec4");
    GLuint texUniform = glGetUniformLocation(color_texture_program->program, "TexCoord_vec2");
    GLuint colorUniform = glGetUniformLocation(color_texture_program->program, "Color_vec4");

    std::cout << 3 << "\n";

        glClear(GL_COLOR_BUFFER_BIT);

    std::cout << 4 << "\n";

        // Bind stuff
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindSampler(0, sampler);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glUseProgram(color_texture_program->program);
        glUniform4f(colorUniform, 1, 1, 1, 1);
        glUniform1i(texUniform, 0);
    std::cout << 4 << "\n";
      //   FT_Set_Pixel_Sizes(face, 0, 50);
        GL_ERRORS();
    std::cout << 4 << "\n";

      render_text(txt, face, 0.5, 0.f, 2., 2.);
    std::cout << 3 << "\n";

      // for (size_t i = 0; i < glyph_count; i++)
      // for(size_t i=0; i<num_chars; i++)
      // {
      //       // hb_codepoint_t glyphid = glyph_info[i].codepoint;
            
      //       // // hb_position_t x_offset = glyph_pos[i].x_offset / 64.0;
      //       // // hb_position_t y_offset = glyph_pos[i].y_offset / 64.0;
      //       // hb_position_t x_advance = glyph_pos[i].x_advance / 64.0;
      //       // hb_position_t y_advance = glyph_pos[i].y_advance / 64.0;

      //       // if (FT_Load_Char(face, txt[i], FT_LOAD_RENDER))
      //       // {
      //       //       std::cout << "ERROR::FREETYTPE: Failed to load Glyph " << txt[i] << std::endl;
      //       //       continue;
      //       // }
      //       // std::cout<<2<<"\n";
            
      //       // // /* load glyph image into the slot (erase previous one) */
      //       // // auto error = FT_Load_Glyph( face, glyphid, FT_LOAD_DEFAULT );
      //       // // if ( error )
      //       // //       continue;  /* ignore errors */

      //       // // /* convert to an anti-aliased bitmap */
      //       // // error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
      //       // // if ( error ){
      //       // //       std::cout<<error<<"\n";
      //       // //       continue;
      //       // // }
      //       // // std::cout<<1<<"\n";
      //       // // FT_Bitmap* bm = &face->glyph->bitmap;
      //       // slot = face->glyph;
            
      //       Gly gly = LoadTexture(txt[i]);
      //       // Gly gly = text_mappings.find(txt[i]);
      //       // GL_ERRORS();
            
      //       float xpos = cursor_x + gly.Bearing.x * scale;
      //       float ypos = cursor_y - (gly.Size.y - gly.Bearing.y) * scale;

      //       float w = gly.Size.x * scale;
      //       float h = gly.Size.y * scale;
      //       // update VBO for each character
      //       // float vertices[6][4] = {
      //       //       {{xpos     ypos + h,  0.0f, 0.0f },            
      //       //       { xpos,     ypos,       0.0f, 1.0f },
      //       //       { xpos + w, ypos,       1.0f, 1.0f },

      //       //       { xpos,     ypos + h,   0.0f, 0.0f },
      //       //       { xpos + w, ypos,       1.0f, 1.0f },
      //       //       { xpos + w, ypos + h,   1.0f, 0.0f }           
      //       // };
      //       ColorTextureProgram::Vertex vertices[] {
      //              {{ xpos,     ypos + h},color,   {0.0f, 0.0f }},            
      //             { {xpos,     ypos}, color,        {0.0f, 1.0f }},
      //             { {xpos + w, ypos}, color,      {1.0f, 1.0f} },

      //             { {xpos,     ypos + h}, color,  {0.0f, 0.0f }},
      //             { {xpos + w, ypos}, color,     {1.0f, 1.0f }},
      //             { {xpos + w, ypos + h}, color,  {1.0f, 0.0f }}
      //       };
      //       // render glyph texture over quad
      //       // update content of VBO memory

      //       GLuint vertext_buffer, vertex_array;

      //       glGenBuffers(1, &vertext_buffer);
      //       vertex_array = color_texture_program->GetVao(vertext_buffer);

      //       glBindBuffer(GL_ARRAY_BUFFER, vertext_buffer);
      //       glBufferData(GL_ARRAY_BUFFER, 4*sizeof(ColorTextureProgram::Vertex), static_cast<const void*>(vertices), GL_STATIC_DRAW);

      //       glBindVertexArray(vertex_array);

      //       // glActiveTexture(GL_TEXTURE0);
      //       // GL_ERRORS();
      //       // glBindVertexArray(VAO);
      //       // GL_ERRORS();

      //       // glBindBuffer(GL_ARRAY_BUFFER, VBO);
      //       // GL_ERRORS();
      //       // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
      //       // GL_ERRORS();
      //       // glBindBuffer(GL_ARRAY_BUFFER, 0);
      //       // GL_ERRORS();
      //       // // render quad
      //       // std::cout<<gly.TextureID << txt[i] << "\n";

      //       // std::cout<<w << " " << h << "\n";


      //       glActiveTexture(GL_TEXTURE0);

      //       glBindTexture(GL_TEXTURE_2D, gly.TextureID);
      //       GL_ERRORS();

      //       glDrawArrays(GL_TRIANGLES, 0, 6);
      //       GL_ERRORS();
      //       // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
      //       cursor_x += (gly.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    
      //       //     draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
            

      //       // cursor_x += x_advance;
      //       // cursor_y += y_advance;
      // }
      // glBindVertexArray(0);
      // glBindTexture(GL_TEXTURE_2D, 0);
      //       GL_ERRORS();

      
      // glUseProgram(0);
      //       GL_ERRORS();
            // show_image();
      // FT_Done_Face    ( face );
      // FT_Done_FreeType( library );
      // exit(0);
}