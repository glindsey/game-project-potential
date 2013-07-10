#include "GUI.h"

#include "Application.h"
#include "GUIRenderer.h"
#include "GUIRenderer3D.h"
#include "MenuArea.h"
#include "StatusArea.h"

namespace gsl
{
namespace rectopia
{

struct GUI::Impl
{
  char dummy_;
};

GUI::GUI()
  : GUIParentElement(nullptr, "GUI"), impl(new Impl())
{
  std::cout << "Debug: Creating GUI instance." << std::endl;

  sf::Window& app_window = App::instance().window();
  // Set up our own variables.
  glm::vec2 window_size = glm::vec2(app_window.getSize().x,
                                    app_window.getSize().y);

  this->setLocation(glm::vec2(0.0f));
  this->setSize(window_size);
  this->setVisible(true);
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
    printf("DEBUG: Visiting children of GUI node %p\n", (void*)this);

    boost::ptr_map<std::string, GUIElement>::iterator iter;
    for (iter = getChildren().begin();
         iter != getChildren().end();
         ++iter)
    {
      iter->second->accept(visitor);
    }
  }
}

EventResult GUI::handleWindowResize(int w, int h)
{
  glm::vec2 window_size = glm::vec2(w, h);
  this->setSize(window_size);
  return EventResult::Acknowledged;
}

} // namespace rectopia
} // namespace gsl
