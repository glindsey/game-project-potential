#include "GUI.h"

#include "Application.h"
#include "ErrorMacros.h"
#include "GUIRenderer.h"
#include "GUIRenderer3D.h"
#include "MenuArea.h"
#include "StatusArea.h"

struct GUI::Impl
{
  char dummy_;
};

GUI::GUI()
  : GUIParentElement(nullptr, "GUI"), impl(new Impl())
{
  sf::Window& app_window = App::instance().window();
  // Set up our own variables.
  glm::vec2 window_size = glm::vec2(app_window.getSize().x,
                                    app_window.getSize().y);

  this->set_location(glm::vec2(0.0f));
  this->set_size(window_size);
  this->set_visible(true);
}

GUI::~GUI()
{
  //dtor
}

void GUI::accept(GUIElementVisitor& visitor)
{
  bool visit_children = visitor.visit(*this);
  if (visit_children)
  {
    DEEP_TRACE("Visiting children of GUI node %p\n", (void*)this);

    boost::ptr_map<std::string, GUIElement>::iterator iter;
    for (iter = get_children().begin();
         iter != get_children().end();
         ++iter)
    {
      iter->second->accept(visitor);
    }
  }
}

EventResult GUI::handle_window_resize(int w, int h)
{
  glm::vec2 window_size = glm::vec2(w, h);
  this->set_size(window_size);
  return EventResult::Acknowledged;
}
