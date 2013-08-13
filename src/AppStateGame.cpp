// *** ADDED BY HEADER FIXUP ***
#include <iostream>
// *** END ***
#include "AppStateGame.h"

#include "AppStateManager.h"
#include "BGRenderer3D.h"
#include "GUI.h"
#include "GUIRenderer3D.h"
#include "MenuArea.h"
#include "Settings.h"
#include "Stage.h"
#include "StageRenderer3D.h"
#include "StatusArea.h"

struct AppStateGame::Impl
{
  std::unique_ptr<BGRenderer> bg_renderer;
  std::unique_ptr<StageRenderer> stage_renderer;
  std::unique_ptr<GUIRenderer> gui_renderer;

  /// Game GUI instance.
  std::unique_ptr<GUI> gui;

  static const unsigned int status_bar_height;
  static const unsigned int menu_bar_width;
};

const unsigned int AppStateGame::Impl::status_bar_height = 150;
const unsigned int AppStateGame::Impl::menu_bar_width = 250;

AppStateGame::AppStateGame(AppStateManager* manager)
  : AppState(manager), impl(new Impl())
{
  impl->gui.reset(new GUI());

  // Create the status area.
  StatusArea* status_area = new StatusArea(impl->gui.get());
  status_area->set_size(glm::vec2(-10, impl->status_bar_height));
  status_area->set_location(glm::vec2(5, -5));
  status_area->set_visible(true);
  impl->gui->add_child(status_area);  //<-- Takes over ownership of status_area

  // Create the menu area.
  MenuArea* menu_area = new MenuArea(impl->gui.get());
  menu_area->set_size(glm::vec2(impl->menu_bar_width,
                                  -(10 + impl->status_bar_height)));
  menu_area->set_location(glm::vec2(5, 5));
  menu_area->set_visible(false);  // DEBUG: shut off for now
  impl->gui->add_child(menu_area);  //<-- Takes over ownership of menu_area
}

AppStateGame::~AppStateGame()
{
}

void AppStateGame::enter_state()
{
  // Build the terrain.
  // TODO: choose a seed
  Stage::getInstance().build(Settings::terrainSize, 2);
}

void AppStateGame::leave_state()
{
}

EventResult AppStateGame::handle_event(const sf::Event& event)
{
  EventResult result = EventResult::Pending;

  // 1. Try to handle the event ourselves.
  result = EventListener::handle_event(event);
  if (result == EventResult::Handled)
  {
    return result;
  }

  // 2. Pass the event to the GUI.
  result = impl->gui->handle_event(event);
  if (result == EventResult::Handled)
  {
    return result;
  }

  // 3. Pass the event to the stage renderer, if it exists.
  if (impl->stage_renderer != nullptr)
  {
    result = impl->stage_renderer->handle_event(event);
    if (result == EventResult::Handled)
    {
      return result;
    }
  }

  // 4. Pass the event to the game stage.
  result = Stage::getInstance().handle_event(event);
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
  if (impl->bg_renderer.get() == nullptr)
  {
    impl->bg_renderer.reset(new BGRenderer3D());
  }
  if (impl->stage_renderer.get() == nullptr)
  {
    impl->stage_renderer.reset(new StageRenderer3D());
  }
  if (impl->gui_renderer.get() == nullptr)
  {
    impl->gui_renderer.reset(new GUIRenderer3D());
  }

  // Render background.
  impl->bg_renderer->draw();

  if (Stage::getInstance().okay_to_render_map())
  {
    // Tell renderer to visit the stage to create the vertex array.
    Stage::getInstance().accept(*(impl->stage_renderer));

    // Render stage.
    impl->stage_renderer->draw();
  }

  if (0)
  {
    // Tell renderer to visit the GUI to create the vertex array.
    impl->gui->accept(*(impl->gui_renderer));

    // Render GUI.
    impl->gui_renderer->draw();
  }
}
