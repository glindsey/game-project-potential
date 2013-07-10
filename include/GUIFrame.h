#ifndef GUIFRAME_H
#define GUIFRAME_H

#include <glm/glm.hpp>
#include <string>

#include "common.h"

#include "GUIParentElement.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class GUIElementVisitor;

class GUIFrame: public GUIParentElement
{
public:
  GUIFrame(GUIElement* parent,
           std::string name,
           glm::vec2 element_location = glm::vec2(0.0f),
           glm::vec2 element_size = glm::vec2(0.0f),
           bool element_visible = false);

  virtual ~GUIFrame();

  virtual void accept(GUIElementVisitor& visitor) override;

  virtual std::string const getElementType() override;

protected:
  struct Impl;
  /// Private implementation pointer
  boost::scoped_ptr<Impl> impl;

private:
};

} // namespace rectopia
} // namespace gsl

#endif // GUIFRAME_H
