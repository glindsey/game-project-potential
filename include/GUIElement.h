#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include <memory>
#include <glm/glm.hpp>
#include <string>

#include "common.h"

#include "EventListener.h"

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
  void set_location(glm::vec2 element_location);

  glm::vec2 const& getSize();

  /// Set the element's size.
  /// The size may be specified as a negative or positive number.
  /// If it is positive or zero, it is the absolute size of the element.
  /// If it is negative, it is subtracted from the size of the element's parent.
  /// @param size Size of the element.
  void set_size(glm::vec2 element_size);

  bool getVisible();
  void set_visible(bool element_visible);

  glm::vec4 const& get_bg_color();
  void set_bg_color(glm::vec4 color);

  glm::vec4 const& get_fg_color();
  void set_fg_color(glm::vec4 color);

  glm::vec2 getAbsoluteLocation();
  glm::vec2 getAbsoluteSize();
  bool get_absolute_visibility();

  /// Set the dirty flag on this element and on this element's parent (if any).
  virtual void set_dirty();

  /// Clear the dirty flag on this element.
  virtual void clear_dirty();

  /// Return whether this element is dirty.
  virtual bool is_dirty();

protected:
  void set_dirty_flag();
  void clear_dirty_flag();
  bool is_dirty_flag_set();

private:
  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};
#endif // GUIELEMENT_H
