#include <iostream>

#include "GUIElement.h"

#include "ErrorMacros.h"
#include "GUIElementVisitor.h"
#include "Settings.h"

namespace gsl
{
namespace rectopia
{

struct GUIElement::Impl
{
  /// Pointer to parent of this element.
  /// If it is null, it means this element has no parent.
  /// The <i>only</i> element that should be parentless in a project
  /// is the main window!
  GUIElement* parent_;

  /// Name of this element.
  std::string name_;

  /// Location of the element in relation to its parent.
  glm::vec2 location_;

  /// Size of the element in relation to its parent.
  /// @todo Not sure if this will be in absolute pixels, or relative to parent's size.
  ///       For GUI elements, the former may be a better choice.
  glm::vec2 size_;

  /// Boolean indicating whether the element is visible.
  bool visible_;

  /// The background color of the element.
  glm::vec4 back_color_;

  /// The foreground color of the element.
  glm::vec4 fore_color_;

  /// Boolean indicating whether render data needs to be updated.
  bool dirty_;
};

GUIElement::GUIElement(GUIElement* parent,
                       std::string name,
                       glm::vec2 element_location,
                       glm::vec2 element_size,
                       bool element_visible)
  : impl(new Impl())
{
  if (parent == this)
  {
    FATAL_ERROR("GUIElement trying to commit time-travelling incest -- an object cannot be its own parent");
  }

  impl->parent_ = parent;
  impl->name_ = name;
  impl->location_ = element_location;
  impl->size_ = element_size;
  impl->visible_ = element_visible;
  impl->back_color_ = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  impl->fore_color_ = glm::vec4(1.0f);
  impl->dirty_ = true;
}

GUIElement::~GUIElement()
{

}

void GUIElement::accept(GUIElementVisitor& visitor)
{
  visitor.visit(*this);
}

std::string const GUIElement::getElementType()
{
  return "GUIElement";
}

std::string GUIElement::getElementName()
{
  return impl->name_;
}

glm::vec2 GUIElement::getAbsoluteLocation()
{
  glm::vec2 absSize = getAbsoluteSize();
  glm::vec2 absLocation;
  glm::vec2 parentSize;
  glm::vec2 parentLocation;

  if (impl->parent_ != nullptr)
  {
    parentLocation = impl->parent_->getAbsoluteLocation();
    parentSize = impl->parent_->getAbsoluteSize();

    if (impl->location_.x > 0)
    {
      absLocation.x = (parentLocation.x + impl->location_.x);
    }
    else if (impl->location_.x == 0)
    {
      float parent_x_center = (parentSize.x - 1) / 2;
      float x_center = (absSize.x - 1) / 2;
      absLocation.x = parentLocation.x + (parent_x_center - x_center);
    }
    else //if (location_.x < 0)
    {
      absLocation.x = parentLocation.x + impl->location_.x +
                      parentSize.x - absSize.x;
    }

    if (impl->location_.y > 0)
    {
      absLocation.y = (parentLocation.y + impl->location_.y);
    }
    else if (impl->location_.y == 0)
    {
      float parent_y_center = (parentSize.y - 1) / 2;
      float y_center = (absSize.y - 1) / 2;
      absLocation.y = parentLocation.y + (parent_y_center - y_center);
    }
    else //if (location_.y < 0)
    {
      absLocation.y = parentLocation.y + impl->location_.y +
                      parentSize.y - absSize.y;
    }
    return absLocation;
  }
  else
  {
    return impl->location_;
  }
}

glm::vec2 GUIElement::getAbsoluteSize()
{
  glm::vec2 parentSize;
  glm::vec2 absSize;

  if (impl->parent_ != nullptr)
  {
    parentSize = impl->parent_->getAbsoluteSize();

    // If size is positive, use it directly.
    // If it is zero or negative, use it as an offset from parent's size.
    absSize.x = (impl->size_.x > 0) ?
                  impl->size_.x : parentSize.x + impl->size_.x;
    absSize.y = (impl->size_.y > 0) ?
                  impl->size_.y : parentSize.y + impl->size_.y;

    // Assert that the resulting size is > 0.  If not, set it equal to parent's size.
    absSize.x = (absSize.x > 0) ? absSize.x : parentSize.x;
    absSize.y = (absSize.y > 0) ? absSize.y : parentSize.y;

    return absSize;
  }
  else
  {
    return impl->size_;
  }
}

bool GUIElement::getAbsoluteVisibility()
{
  if (impl->parent_ != nullptr)
  {
    return (impl->visible_ && impl->parent_->getAbsoluteVisibility());
  }
  else
  {
    return impl->visible_;
  }
}

void GUIElement::clearDirty()
{
  clearDirtyFlag();
}

void GUIElement::setDirty()
{
  setDirtyFlag();
  if ((impl->parent_ != nullptr) && (impl->parent_->isDirty() != true))
  {
    impl->parent_->setDirty();
  }
}

bool GUIElement::isDirty()
{
  return isDirtyFlagSet();
}

glm::vec2 const& GUIElement::getLocation()
{
  return impl->location_;
}

void GUIElement::setLocation(glm::vec2 element_location)
{
  impl->location_ = element_location;
  setDirtyFlag();
}

glm::vec2 const& GUIElement::getSize()
{
  return impl->size_;
}

void GUIElement::setSize(glm::vec2 element_size)
{
  impl->size_ = element_size;
  setDirtyFlag();
}

bool GUIElement::getVisible()
{
  return impl->visible_;
}

void GUIElement::setVisible(bool visible)
{
  impl->visible_ = visible;
  setDirtyFlag();
}

glm::vec4 const& GUIElement::getBackColor()
{
  return impl->back_color_;
}

void GUIElement::setBackColor(glm::vec4 color)
{
  impl->back_color_ = color;
  setDirtyFlag();
}

glm::vec4 const& GUIElement::getForeColor()
{
  return impl->fore_color_;
}

void GUIElement::setForeColor(glm::vec4 color)
{
  impl->fore_color_ = color;
  setDirtyFlag();
}

void GUIElement::clearDirtyFlag()
{
  impl->dirty_ = false;
}

void GUIElement::setDirtyFlag()
{
  impl->dirty_ = true;

  if (impl->parent_ != nullptr)
  {
    impl->parent_->setDirty();
  }
}

bool GUIElement::isDirtyFlagSet()
{
  return impl->dirty_;
}

} // namespace rectopia
} // namespace gsl
