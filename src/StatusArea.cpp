#include <sstream>
#include <string>

#include "StatusArea.h"

#include "Application.h"
#include "FaceBools.h"
#include "GUIFrame.h"
#include "GUILabel.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"

struct StatusArea::Impl
{
  char dummy_;
};

StatusArea::StatusArea(GUIElement* parent)
  : GUIParentElement(parent, "status_area"), impl(new Impl())
{
  GUIFrame* frame = new GUIFrame(this, "frame");
  frame->set_bg_color(glm::vec4(0.0f, 0.0f, 0.1f, 0.5f));
  frame->set_visible(true);
  add_child(frame);

  GUILabel* test_label = new GUILabel(frame, "test_label");
  test_label->set_bg_color(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
  test_label->set_fg_color(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
  test_label->set_location(glm::vec2(-5, -5));
  test_label->set_size(glm::vec2(300, 20));
  test_label->set_visible(true);
  test_label->setText(U"Testing GUILabel");
  frame->add_child(test_label);
}

StatusArea::~StatusArea()
{
}

void StatusArea::render()
{
  // TODO: rewrite to use 2D shader program instead.
  /*
  sf::RenderWindow* window = App::instance().window();

  sf::Vector2f areaLocation = getAbsoluteLocation();

  sf::Vector3i const& cursorLoc = impl->stage_->cursor();

  StageBlock& block = impl->stage_->block(cursorLoc);
  const Substance& solid_substance = block.get_substance(BlockLayer::Solid);
  const Substance& fluid_substance = block.get_substance(BlockLayer::Fluid);

  sf::Text text;
  std::ostringstream stream;

  // TODO: Any updates to the children that are required.

  // Draw children of the status area.
  renderChildren();

  // DEBUG: Create the text.
  stream << "Block (" << cursorLoc.x << ", " << cursorLoc.y << ", "
         << cursorLoc.z << ") -- ";

  if (block.known())
  {
    stream << solid_substance.getProperties().get<std::string>("name") << " / ";
    stream << fluid_substance.getProperties().get<std::string>("name");
  }
  else
  {
    stream << "???";

    if (Settings::debugMapRevealAll)
    {
      stream << "(" << solid_substance.getProperties().get<std::string>("name")
             << " / ";
      stream << fluid_substance.getProperties().get<std::string>("name") << ")";
    }
  }

  text.setString(stream.str());
  text.setFont(GUIElement::get_defaultFont());
  text.setCharacterSize(16);
  text.setPosition(areaLocation.x + 2, areaLocation.y + 2);
  window->draw(text);

  stream.str("");
  stream << "Hidden: solid = ";
  stream << ((block.get_hidden_faces(BlockLayer::Solid).bottom()) ? "Bm " : "-- ");
  stream << ((block.get_hidden_faces(BlockLayer::Solid).top()) ? "Tp " : "-- ");
  stream << ((block.get_hidden_faces(BlockLayer::Solid).left()) ? "Lt " : "-- ");
  stream << ((block.get_hidden_faces(BlockLayer::Solid).right()) ? "Rt " : "-- ");
  stream << ((block.get_hidden_faces(BlockLayer::Solid).back()) ? "Bk " : "-- ");
  stream << ((block.get_hidden_faces(BlockLayer::Solid).front()) ? "Ft " : "-- ");
  stream << "/ fluid = ";
  stream << ((block.get_hidden_faces(BlockLayer::Fluid).bottom()) ? "Bm " : "--") ;
  stream << ((block.get_hidden_faces(BlockLayer::Fluid).top()) ? "Tp " : "-- ");
  stream << ((block.get_hidden_faces(BlockLayer::Fluid).left()) ? "Lt " : "-- ");
  stream << ((block.get_hidden_faces(BlockLayer::Fluid).right()) ? "Rt " : "-- ");
  stream << ((block.get_hidden_faces(BlockLayer::Fluid).back()) ? "Bk " : "-- ");
  stream << ((block.get_hidden_faces(BlockLayer::Fluid).front()) ? "Ft " : "-- ");
  text.setString(stream.str());
  text.setPosition(areaLocation.x + 2, areaLocation.y + 22);
  window->draw(text);

  stream.str("");
  stream << ((block.visible()) ? "VISBL" : "hiddn") << " ";
  stream << ((block.solid()) ? "SOLID" : "nonsd") << " ";
  stream << ((block.traversable()) ? "TRAVER" : "impass") << ", ";
  stream << ((block.opaque()) ? "OPAQUE" : "transp");
  text.setString(stream.str());
  text.setPosition(areaLocation.x + 2, areaLocation.y + 62);
  window->draw(text);

  stream.str("");
  stream << "Corners: bl/br/fl/fr = " << block.top_corners().back_left() << "/"
         << block.top_corners().back_right() << "/"
         << block.top_corners().front_left() << "/"
         << block.top_corners().front_right();
  text.setString(stream.str());
  text.setPosition(areaLocation.x + 2, areaLocation.y + 82);
  window->draw(text);
  */
}
