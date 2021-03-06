// *** ADDED BY HEADER FIXUP ***
#include <ctime>
#include <iostream>
// *** END ***
/** Application State: Splash Screen
 * @author Gregory Lindsey
 * @note Derived from code by metacipherstudios at http://sdltutorials.org.
 */

#include "AppStateSplash.h"
#include "AppStateManager.h"

#include "Application.h"

AppStateSplash::AppStateSplash(AppStateManager* manager)
  : AppState(manager)
{
}

AppStateSplash::~AppStateSplash()
{
}

void AppStateSplash::enter_state()
{
  // TODO: Load Simple Logo

  clock.restart();
}

void AppStateSplash::leave_state()
{
}

void AppStateSplash::process()
{
  if (clock.getElapsedTime().asSeconds() >= 1)
  {
    manager_->SetActiveAppState(AppStateID::Game);
  }
}

void AppStateSplash::render()
{
  // TODO: rewrite to use 2D shader program instead.

  //sf::RenderWindow* window = App::instance().window();

  // TODO: center it.
  //window->pushGLStates();
  //window->draw(spriteLogo);
  //window->popGLStates();
}
