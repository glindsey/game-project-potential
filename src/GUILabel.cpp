#include "GUILabel.h"

#include "GUIElementVisitor.h"

struct GUILabel::Impl
{
  std::u32string   text_;
};

GUILabel::GUILabel(GUIElement* parent,
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

GUILabel::~GUILabel()
{
  //dtor
}

void GUILabel::accept(GUIElementVisitor& visitor)
{
  visitor.visit(*this);
}

std::string const GUILabel::getElementType()
{
  return "GUILabel";
}

std::u32string GUILabel::getText()
{
  return impl->text_;
}

void GUILabel::setText(std::u32string text)
{
  impl->text_ = text;
}
