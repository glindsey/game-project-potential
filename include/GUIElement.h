#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include <boost/scoped_ptr.hpp>
#include <glm/glm.hpp>
#include <string>

#include "common.h"

#include "EventListener.h"

namespace gsl
{
namespace rectopia
{

/// Forward declarations
class GUIElementVisitor;

/// Base class for an element in the game engine's GUI.
class GUIElement: public EventListener
{
public:
  GUIElement(GUIElement* parent,
             std::string name,
             glm::vec2 element_location = glm::vec2(0.0f),
             glm::vec2 element_size = glm::vec2(0.0f),
             bool element_visible = false);

  virtual ~GUIElement();

  /// Accept a visitor to this element.
  virtual void accept(GUIElementVisitor& visitor);

  /// Returns the name of this particular element type.
  virtual std::string const getElementType();

  /// Returns this element's name.
  std::string getElementName();

  glm::vec2 const& getLocation();

  /// Set the element's location.
  /// The location may be specified as a negative or positive number.
  /// If it is positive or zero, it is the offset of the LEFT or TOP edge
  ///                            from the LEFT or TOP edge of its parent.
  /// If it is negative, it is the offset of the RIGHT or BOTTOM edge
  ///                    from the RIGHT or BOTTOM edge of its parent.
  /// Thus, the sign of the coordinate represents how the element is anchored to its parent.
  /// @todo Make zero indicate anchoring to the parent's center.
  /// @param location Location of the element.
  void setLocation(glm::vec2 element_location);

  glm::vec2 const& getSize();

  /// Set the element's size.
  /// The size may be specified as a negative or positive number.
  /// If it is positive or zero, it is the absolute size of the element.
  /// If it is negative, it is subtracted from the size of the element's parent.
  /// @param size Size of the element.
  void setSize(glm::vec2 element_size);

  bool getVisible();
  void setVisible(bool element_visible);

  glm::vec4 const& getBackColor();
  void setBackColor(glm::vec4 color);

  glm::vec4 const& getForeColor();
  void setForeColor(glm::vec4 color);

  glm::vec2 getAbsoluteLocation();
  glm::vec2 getAbsoluteSize();
  bool getAbsoluteVisibility();

  /// Set the dirty flag on this element and on this element's parent (if any).
  virtual void setDirty();

  /// Clear the dirty flag on this element.
  virtual void clearDirty();

  /// Return whether this element is dirty.
  virtual bool isDirty();

protected:
  void setDirtyFlag();
  void clearDirtyFlag();
  bool isDirtyFlagSet();

private:
  struct Impl;
  /// Private implementation pointer
  boost::scoped_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif // GUIELEMENT_H
