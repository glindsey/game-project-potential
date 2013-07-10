#include "GUIParentElement.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "ErrorMacros.h"
#include "GUIElementVisitor.h"

namespace gsl
{
namespace rectopia
{

struct GUIParentElement::Impl
{
  /// Vector of GUI Elements that this element contains.
  boost::ptr_map<std::string, GUIElement> children_;
};

GUIParentElement::GUIParentElement(GUIElement* parent,
                                   std::string name,
                                   glm::vec2 element_location,
                                   glm::vec2 element_size,
                                   bool element_visible)
  : GUIElement(parent, name, element_location,
               element_size, element_visible),
    impl(new Impl())
{
  //ctor
}

GUIParentElement::~GUIParentElement()
{
  //dtor
}

void GUIParentElement::accept(GUIElementVisitor& visitor)
{
  bool visit_children = visitor.visit(*this);
  if (visit_children)
  {
    boost::ptr_map<std::string, GUIElement>::iterator iter;

    printf("DEBUG: Visiting children of GUIParentElement node %p\n", (void*)this);
    for (iter = impl->children_.begin();
         iter != impl->children_.end();
         ++iter)
    {
      iter->second->accept(visitor);
    }
  }
}

EventResult GUIParentElement::handleEvent(sf::Event const& event)
{
  EventResult result = EventResult::Pending;

  // First try to act upon it ourselves.
  result = GUIElement::handleEvent(event);
  if (result == EventResult::Handled)
  {
    return result;
  }

  boost::ptr_map<std::string, GUIElement>::iterator iter;

  // Then pass it along to each child in order.
  for (iter = impl->children_.begin();
       iter != impl->children_.end();
       ++iter)
  {
    result = iter->second->handleEvent(event);
    if (result == EventResult::Handled)
    {
      return result;
    }
  }

  // Finally, if no child handled the event, return.
  return result;
}

bool GUIParentElement::addChild(GUIElement* _child)
{
  if (_child == nullptr)
  {
    FATAL_ERROR("Attempted to add null GUIElement to GUIParentElement \"%s\"",
                this->getElementName().c_str());
  }

  std::string name = _child->getElementName();

  if (impl->children_.count(name) == 0)
  {
    printf("DEBUG: Adding child element \"%s\" to parent element \"%s\"\n",
           name.c_str(), this->getElementName().c_str());
    impl->children_.insert(name, _child);
    setDirty();
    return true;
  }
  else
  {
    return false;
  }
}

bool GUIParentElement::delChild(std::string _name)
{
  if (impl->children_.count(_name) != 0)
  {
    impl->children_.erase(_name);
    setDirty();
    return true;
  }
  else
  {
    return false;
  }
}

void GUIParentElement::clearChildren()
{
  impl->children_.clear();
  setDirty();
}

GUIElement* GUIParentElement::getChild(std::string _name)
{
  if (impl->children_.count(_name) != 0)
  {
    return &(impl->children_.at(_name));
  }
  else
  {
    return nullptr;
  }
}

unsigned int GUIParentElement::getChildCount()
{
  return impl->children_.size();
}

void GUIParentElement::clearDirty()
{
  clearDirtyFlag();

  boost::ptr_map<std::string, GUIElement>::iterator iter;

  // Then pass it along to each child in order.
  for (iter = impl->children_.begin();
       iter != impl->children_.end();
       ++iter)
  {
    iter->second->clearDirty();
  }
}

boost::ptr_map<std::string, GUIElement>& GUIParentElement::getChildren()
{
  return impl->children_;
}

} // namespace rectopia
} // namespace gsl