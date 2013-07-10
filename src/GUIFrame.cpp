#include "GUIFrame.h"

#include "GUIElementVisitor.h"

namespace gsl
{
namespace rectopia
{

struct GUIFrame::Impl
{
  // Actually nothing to put here for now!
  char dummy_;
};

GUIFrame::GUIFrame(GUIElement* parent,
                   std::string name,
                   glm::vec2 element_location,
                   glm::vec2 element_size,
                   bool element_visible)
  : GUIParentElement(parent, name, element_location,
                     element_size, element_visible),
    impl(new Impl())
{
  //ctor
}

GUIFrame::~GUIFrame()
{
  //dtor
}

void GUIFrame::accept(GUIElementVisitor& visitor)
{
  bool visit_children = visitor.visit(*this);
  if (visit_children)
  {
    printf("DEBUG: Visiting children of GUIFrame node %p\n", (void*)this);
    boost::ptr_map<std::string, GUIElement>::iterator iter;

    for (iter = getChildren().begin();
         iter != getChildren().end();
         ++iter)
    {
      iter->second->accept(visitor);
    }
  }
}

std::string const GUIFrame::getElementType()
{
  return "GUIFrame";
}

} // namespace rectopia
} // namespace gsl
