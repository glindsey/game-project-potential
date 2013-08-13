#include <SFML/Graphics.hpp>

#include "SimpleMatrixFont.h"

#include "ErrorMacros.h"

struct SimpleMatrixFont::Impl
{
  sf::Texture font_texture;
  glm::uvec2 glyph_size;
  glm::uvec2 glyph_count;
};

SimpleMatrixFont::SimpleMatrixFont(unsigned int x_glyph_size,
                                   unsigned int y_glyph_size,
                                   unsigned int x_glyph_count,
                                   unsigned int y_glyph_count)
  : TextureFont(), impl(new Impl())
{
  impl->glyph_size.x = x_glyph_size;
  impl->glyph_size.y = y_glyph_size;
  impl->glyph_count.x = x_glyph_count;
  impl->glyph_count.y = y_glyph_count;
}

SimpleMatrixFont::~SimpleMatrixFont()
{
  //dtor
}

bool SimpleMatrixFont::load(std::string name)
{
  sf::Image image;
  image.loadFromFile("fonts/" + name);
  // TODO: setting choosing whether "magic purple" should be treated as a mask
  image.createMaskFromColor(sf::Color(255, 0, 255));

  return impl->font_texture.loadFromImage(image);
}

void SimpleMatrixFont::bind()
{
  sf::Texture::bind(&impl->font_texture);
}

void SimpleMatrixFont::unbind()
{
  sf::Texture::bind(nullptr);
}

glm::vec4 SimpleMatrixFont::getTextureCoordinates(char32_t character)
{
  unsigned int xLocation = (unsigned int)character % impl->glyph_count.x;
  unsigned int yLocation = (unsigned int)character / impl->glyph_count.x;
  float xCoord = (float)xLocation / (float)impl->glyph_count.x;
  float yCoord = (float)yLocation / (float)impl->glyph_count.y;
  float xSize = 1 / (float)impl->glyph_count.x;
  float ySize = 1 / (float)impl->glyph_count.y;
  return glm::vec4(xCoord, yCoord, xSize, ySize);
}

glm::vec2 SimpleMatrixFont::getGlyphSize(char32_t character)
{
  return glm::vec2(impl->glyph_size.x, impl->glyph_size.y);
}

int32_t SimpleMatrixFont::getXAdvance(char32_t character)
{
  return impl->glyph_size.x;
}

int32_t SimpleMatrixFont::getKerning(char32_t char1, char32_t char2)
{
  return 0;
}

int32_t SimpleMatrixFont::getLineHeight()
{
  return impl->glyph_size.y;
}

int32_t SimpleMatrixFont::getBaseline()
{
  return impl->glyph_size.y;
}
