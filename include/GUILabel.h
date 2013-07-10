#ifndef GUILABEL_H
#define GUILABEL_H

#include <glm/glm.hpp>
#include <string>

#include "common.h"

#include "GUIElement.h"

namespace gsl
{
namespace rectopia
{

/// Forward declarations
class GUIElementVisitor;

/// Class representing a text label in the GUI.
class GUILabel: public GUIElement
{
public:
  GUILabel(GUIElement* parent,
           std::string name,
           glm::vec2 element_location = glm::vec2(0.0f),
           glm::vec2 element_size = glm::vec2(0.0f),
           bool element_visible = false);

  virtual ~GUILabel();

  virtual void accept(GUIElementVisitor& visitor) override;
  virtual std::string const getElementType() override;

  virtual std::u32string getText();
  virtual void setText(std::u32string text);

private:
  struct Impl;
  /// Private implementation pointer
  boost::scoped_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif // GUIFRAME_H