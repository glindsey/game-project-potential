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

    DEEP_TRACE("Visiting children of GUIParentElement node %p", (void*)this);
    for (iter = impl->children_.begin();
         iter != impl->children_.end();
         ++iter)
    {
      iter->second->accept(visitor);
    }
  }
}

EventResult GUIParentElement::handle_event(sf::Event const& event)
{
  EventResult result = EventResult::Pending;

  // First try to act upon it ourselves.
  result = GUIElement::handle_event(event);
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
    result = iter->second->handle_event(event);
    if (result == EventResult::Handled)
    {
      return result;
    }
  }

  // Finally, if no child handled the event, return.
  return result;
}

bool GUIParentElement::add_child(GUIElement* _child)
{
  if (_child == nullptr)
  {
    FATAL_ERROR("Attempted to add null GUIElement to GUIParentElement \"%s\"",
                this->getElementName().c_str());
  }

  std::string name = _child->getElementName();

  if (impl->children_.count(name) == 0)
  {
    DEEP_TRACE("Adding child element \"%s\" to parent element \"%s\"",
           name.c_str(), this->getElementName().c_str());
    impl->children_.insert(name, _child);
    set_dirty();
    return true;
  }
  else
  {
    return false;
  }
}

bool GUIParentElement::del_child(std::string _name)
{
  if (impl->children_.count(_name) != 0)
  {
    impl->children_.erase(_name);
    set_dirty();
    return true;
  }
  else
  {
    return false;
  }
}

void GUIParentElement::clear_children()
{
  impl->children_.clear();
  set_dirty();
}

GUIElement* GUIParentElement::get_child(std::string _name)
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

unsigned int GUIParentElement::get_child_count()
{
  return impl->children_.size();
}

void GUIParentElement::clear_dirty()
{
  clear_dirty_flag();

  boost::ptr_map<std::string, GUIElement>::iterator iter;

  // Then pass it along to each child in order.
  for (iter = impl->children_.begin();
       iter != impl->children_.end();
       ++iter)
  {
    iter->second->clear_dirty();
  }
}

boost::ptr_map<std::string, GUIElement>& GUIParentElement::get_children()
{
  return impl->children_;
}

} // namespace rectopia
} // namespace gsl
