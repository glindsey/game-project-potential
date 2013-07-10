#ifndef GUIPARENTELEMENT_H
#define GUIPARENTELEMENT_H

#include <boost/ptr_container/ptr_map.hpp>

#include "common.h"

#include "GUIElement.h"

namespace gsl
{
namespace rectopia
{

/// Base class for a GUI element that can have child elements underneath it.
/// It inherits from GUIElement as its parent class.
class GUIParentElement: public GUIElement
{
public:
  GUIParentElement(GUIElement* parent,
                   std::string name,
                   glm::vec2 element_location = glm::vec2(0.0f),
                   glm::vec2 element_size = glm::vec2(0.0f),
                   bool element_visible = false);

  virtual ~GUIParentElement();

  /// Accept a visitor to this element.
  virtual void accept(GUIElementVisitor& visitor);

  /// Handle an event.
  /// First we attempt to act upon the event ourselves; if the event is not
  /// handled, we pass it along to each child element in turn.
  EventResult handleEvent(sf::Event const& event);

  /// Add a child to this element.
  bool addChild(GUIElement* child);

  /// Delete a child from this element.
  bool delChild(std::string _name);

  /// Clear all children of this element.
  void clearChildren();

  /// Get a child of this element.
  GUIElement* getChild(std::string _name);

  /// Get the total number of children of this element.
  unsigned int getChildCount();

  /// Clear dirty flag, and dirty flag on all child elements.
  virtual void clearDirty();

protected:
  /// Return container of children.
  boost::ptr_map<std::string, GUIElement>& getChildren();

private:
  struct Impl;
  /// Private implementation pointer
  boost::scoped_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif // GUIPARENTELEMENT_H
