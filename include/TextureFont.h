#ifndef TEXTUREFONT_H
#define TEXTUREFONT_H

#include <boost/noncopyable.hpp>
#include <glm/glm.hpp>
#include <string>

#include "common.h"

namespace gsl
{
namespace rectopia
{

/// A font with an OpenGL bindable texture.  Characters are returned as
/// texture coordinates stored in a glm::vec4 (s,t,u,v).
/// This is a virtual class used as an interface.  Implementations should
/// derive from this class.

class TextureFont : public boost::noncopyable
{
  public:
    TextureFont();
    virtual ~TextureFont();

    /// Loads a particular font.  The string passed in is implementation-defined
    /// but is usually a filename to load.
    /// @param name Name of the font to load.
    /// @return True if loading was successful; false if an error occurred.
    virtual bool load(std::string name) = 0;

    /// Binds the font texture for OpenGL rendering.
    virtual void bind() = 0;

    /// Unbinds the font texture for OpenGL rendering.
    virtual void unbind() = 0;

    /// Gets the texture coordinates of a particular character.
    /// @param character Character to find.
    /// @return A vec4 containing the s/t/p/q coordinates of the texture.
    virtual glm::vec4 getTextureCoordinates(char32_t character) = 0;

    /// Gets the size of a character glyph, in pixels.
    /// @param character Character to find.
    /// @return A vec2 containing the x/y size of the glyph, in pixels.
    virtual glm::vec2 getGlyphSize(char32_t character) = 0;

    /// Gets the X-advance for a particular character -- often, but not always,
    /// the same as the character's width.
    /// @param character Character to find.
    /// @return The number of pixels to advance for the character.
    virtual int32_t getXAdvance(char32_t character) = 0;

    /// Gets the kerning for a pair of characters.
    /// @param char1 The first character in the pair.
    /// @param char2 The second character in the pair.
    /// @return The number of pixels to kern.
    virtual int32_t getKerning(char32_t char1, char32_t char2) = 0;

    /// Get the line height for the font.
    /// @return The number of pixels the line height is.
    virtual int32_t getLineHeight() = 0;

    /// Gets the font's baseline.
    /// @return The pixel corresponding to the font's baseline.
    virtual int32_t getBaseline() = 0;

  protected:
  private:
};

} // end namespace rectopia
} // end namespace gsl

#endif // TEXTUREFONT_H
