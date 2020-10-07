#include <hb.h>
#include <hb-ft.h>
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <ft2build.h>
#include "GL.hpp"
#include "ColorTextureProgram.hpp"

#include FT_FREETYPE_H

struct TextTexture {
    TextTexture();
    ~TextTexture();
    void DrawText(float cursor_x, float cursor_y, float scale_x, float scale_y);
    void Update(char* txt);

    int align = 0; // 0->mid
    float min_line_cursor = -.5;
    float max_line_cursor = 0.5;
    int line_limit = 30;

    int line_no = 0;

    char txt [500] = "morning";

    unsigned int VAO, VBO;
    GLuint vs{0}, fs{0}, program{0};

    //draw functions will work on vectors of vertices, defined as follows:
	struct Vertex {
		Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_, glm::vec2 const &TexCoord_) :
			Position(Position_), Color(Color_), TexCoord(TexCoord_) { }
		glm::vec3 Position;
		glm::u8vec4 Color;
		glm::vec2 TexCoord;
	};
	static_assert(sizeof(Vertex) == 4*3 + 1*4 + 4*2, "PongMode::Vertex should be packed");

    FT_Library  library;
    FT_Face     face;

    // FT_GlyphSlot  slot;
    FT_Matrix     matrix;                 /* transformation matrix */
    FT_Vector     pen;                    /* untransformed origin  */
    FT_Error      error;

    const char*         filename;
    char*         text;

    double        angle;
    int           target_height;
    int           n, num_chars;

    hb_buffer_t *buf;
    hb_font_t *font;

    struct Gly{
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    struct Coordi{
        float x, y, s, t;
    };

    std::map<char, Gly> text_mappings;

    private:
        TextTexture::Gly LoadTexture(char cp);

	// ColorTextureProgram color_texture_program;

};