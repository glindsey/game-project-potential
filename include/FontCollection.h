#ifndef FONTCOLLECTION_H
#define FONTCOLLECTION_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>
#include <string>

namespace gsl
{
namespace rectopia
{

// Forward declarations
class TextureFont;

class FontCollection : public boost::noncopyable
{
  public:
    FontCollection();
    virtual ~FontCollection();

    // Add the requested font to the collection.
    bool add(std::string fontName, boost::shared_ptr<TextureFont> textureFont);

    // Return whether the requested font exists.
    bool exists(std::string fontName);

    // Get the requested font.  If the font doesn't exist, return the fallback
    // font.
    TextureFont& get(std::string fontName);

    // Get the default font.  Use for speed when you just want the default.
    TextureFont& getDefault();

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif // FONTCOLLECTION_H
