#include <SFML/Graphics.hpp>

#include "SimpleMatrixFont.h"

namespace gsl
{
namespace rectopia
{

struct SimpleMatrixFont::Impl
{
  sf::Texture fontTexture;
  glm::uvec2 glyphSize;
  glm::uvec2 glyphCount;
};

SimpleMatrixFont::SimpleMatrixFont(unsigned int xGlyphSize,
                                   unsigned int yGlyphSize,
                                   unsigned int xGlyphCount,
                                   unsigned int yGlyphCount)
  : TextureFont(), impl(new Impl())
{
  impl->glyphSize.x = xGlyphSize;
  impl->glyphSize.y = yGlyphSize;
  impl->glyphCount.x = xGlyphCount;
  impl->glyphCount.y = yGlyphCount;
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

  return impl->fontTexture.loadFromImage(image);
}

void SimpleMatrixFont::bind()
{
  impl->fontTexture.bind(&impl->fontTexture);
}

glm::vec4 SimpleMatrixFont::getTextureCoordinates(char32_t character)
{
  unsigned int xLocation = (unsigned int)character % impl->glyphCount.x;
  unsigned int yLocation = (unsigned int)character / impl->glyphCount.x;
  float xCoord = (float)xLocation / (float)impl->glyphCount.x;
  float yCoord = (float)yLocation / (float)impl->glyphCount.y;
  float xSize = 1 / (float)impl->glyphCount.x;
  float ySize = 1 / (float)impl->glyphCount.y;
  return glm::vec4(xCoord, yCoord, xSize, ySize);
}

glm::vec2 SimpleMatrixFont::getGlyphSize(char32_t character)
{
  return glm::vec2(impl->glyphSize.x, impl->glyphSize.y);
}

int32_t SimpleMatrixFont::getXAdvance(char32_t character)
{
  return impl->glyphSize.x;
}

int32_t SimpleMatrixFont::getKerning(char32_t char1, char32_t char2)
{
  return 0;
}

int32_t SimpleMatrixFont::getLineHeight()
{
  return impl->glyphSize.y;
}

int32_t SimpleMatrixFont::getBaseline()
{
  return impl->glyphSize.y;
}

} // end namespace rectopia
} // end namespace gsl
