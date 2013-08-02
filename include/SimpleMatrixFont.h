#ifndef SIMPLEMATRIXFONT_H
#define SIMPLEMATRIXFONT_H

#include <memory>
#include <glm/glm.hpp>

#include "TextureFont.h"

namespace gsl
{
namespace rectopia
{

class SimpleMatrixFont : public TextureFont
{
  public:
    SimpleMatrixFont(unsigned int xGlyphSize = 16,
                     unsigned int yGlyphSize = 16,
                     unsigned int xGlyphCount = 16,
                     unsigned int yGlyphCount = 16);

    virtual ~SimpleMatrixFont();

    bool load(std::string name);
    void bind();
    glm::vec4 getTextureCoordinates(char32_t character);
    glm::vec2 getGlyphSize(char32_t character);
    int32_t getXAdvance(char32_t character);
    int32_t getKerning(char32_t char1, char32_t char2);
    int32_t getLineHeight();
    int32_t getBaseline();

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // end namespace rectopia
} // end namespace gsl

#endif // SIMPLEMATRIXFONT_H
