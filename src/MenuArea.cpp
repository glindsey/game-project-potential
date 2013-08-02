#include "MenuArea.h"

#include "Application.h"
#include "GUIFrame.h"

namespace gsl
{
namespace rectopia
{

struct MenuArea::Impl
{
  // Nothing here yet!
  int dummy_;
};

MenuArea::MenuArea(GUIElement* parent)
  : GUIParentElement(parent, "menu_area"), impl(new Impl())
{
  GUIFrame* frame = new GUIFrame(this, "frame");
  frame->set_bg_color(glm::vec4(0.0f, 0.0f, 0.1f, 1.0f));
  frame->set_visible(true);
  addChild(frame);
}

void MenuArea::render()
{
  // TODO: rewrite to use 2D shader program instead.

  /*
  //sf::RenderWindow* window = App::instance().window();

  // Draw children of the menu area.
  renderChildren();

  // TODO: stuff
  //window->draw(whatever);
  */
}

} // namespace rectopia
} // namespace gsl
