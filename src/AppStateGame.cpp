// *** ADDED BY HEADER FIXUP ***
#include <iostream>
// *** END ***
#include "AppStateGame.h"

#include "AppStateManager.h"
#include "GUI.h"
#include "GUIRenderer3D.h"
#include "MenuArea.h"
#include "Settings.h"
#include "Stage.h"
#include "StageRenderer3D.h"
#include "StatusArea.h"

namespace gsl
{
namespace rectopia
{

struct AppStateGame::Impl
{
  boost::scoped_ptr<StageRenderer> stage_renderer_;
  boost::scoped_ptr<GUIRenderer> gui_renderer_;

  /// Game GUI instance.
  boost::scoped_ptr<GUI> gui_;

  static const unsigned int status_bar_height_;
  static const unsigned int menu_bar_width_;
};

const unsigned int AppStateGame::Impl::status_bar_height_ = 150;
const unsigned int AppStateGame::Impl::menu_bar_width_ = 250;

AppStateGame::AppStateGame(AppStateManager* manager)
  : AppState(manager), impl(new Impl())
{
  std::cout << "DEBUG: Creating AppStateGame." << std::endl;

  impl->gui_.reset(new GUI());

  // Create the status area.
  StatusArea* status_area = new StatusArea(impl->gui_.get());
  status_area->setSize(glm::vec2(-10, impl->status_bar_height_));
  status_area->setLocation(glm::vec2(5, -5));
  status_area->setVisible(true);
  impl->gui_->addChild(status_area);  //<-- Takes over ownership of status_area

  // Create the menu area.
  MenuArea* menu_area = new MenuArea(impl->gui_.get());
  menu_area->setSize(glm::vec2(impl->menu_bar_width_,
                                  -(10 + impl->status_bar_height_)));
  menu_area->setLocation(glm::vec2(5, 5));
  menu_area->setVisible(false);  // DEBUG: shut off for now
  impl->gui_->addChild(menu_area);  //<-- Takes over ownership of menu_area
}

AppStateGame::~AppStateGame()
{
  std::cout << "DEBUG: Destroying AppStateGame." << std::endl;
}

void AppStateGame::enterState()
{
  std::cout << "DEBUG: Entering state AppStateGame." << std::endl;

  // Build the terrain.
  // TODO: choose a seed
  Stage::getInstance().build(Settings::terrainSize, 2);
}

void AppStateGame::leaveState()
{
  std::cout << "DEBUG: Leaving state AppStateGame." << std::endl;
}

EventResult AppStateGame::handleEvent(const sf::Event& event)
{
  EventResult result = EventResult::Pending;

  // 1. Try to handle the event ourselves.
  result = EventListener::handleEvent(event);
  if (result == EventResult::Handled)
  {
    return result;
  }

  // 2. Pass the event to the GUI.
  result = impl->gui_->handleEvent(event);
  if (result == EventResult::Handled)
  {
    return result;
  }

  // 3. Pass the event to the stage renderer, if it exists.
  if (impl->stage_renderer_ != nullptr)
  {
    result = impl->stage_renderer_->handleEvent(event);
    if (result == EventResult::Handled)
    {
      return result;
    }
  }

  // 4. Pass the event to the game stage.
  result = Stage::getInstance().handleEvent(event);
  if (result == EventResult::Handled)
  {
    return result;
  }

  return result;
}

void AppStateGame::process()
{
  // Run the stage's processing state machine.
  Stage::getInstance().process();
}

void AppStateGame::render()
{
  if (impl->stage_renderer_.get() == nullptr)
  {
    impl->stage_renderer_.reset(new StageRenderer3D());
  }
  if (impl->gui_renderer_.get() == nullptr)
  {
    impl->gui_renderer_.reset(new GUIRenderer3D());
  }

  if (Stage::getInstance().okay_to_render_map())
  {
    // Tell renderer to visit the stage to create the vertex array.
    Stage::getInstance().accept(*(impl->stage_renderer_));

    // Get the renderer ready.
    impl->stage_renderer_->prepare();

    // Render the vertices to the screen.
    impl->stage_renderer_->draw();

    // Finish the drawing procedure.
    impl->stage_renderer_->finish();
  }

  if (1)
  {
    // Tell renderer to visit the GUI to create the vertex array.
    impl->gui_->accept(*(impl->gui_renderer_));

    // Get the renderer ready.
    impl->gui_renderer_->prepare();

    // Render the vertices to the screen.
    impl->gui_renderer_->draw();

    // Finish the drawing procedure.
    impl->gui_renderer_->finish();
  }
}

} // namespace rectopia
} // namespace gsl
