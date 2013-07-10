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
#include "Substance.h"
#include "PropPrototype.h"
#include "Verb.h"

namespace gsl
{
namespace rectopia
{

AppStateSplash::AppStateSplash(AppStateManager* manager)
  : AppState(manager)
{
  std::cout << "DEBUG: Creating AppStateSplash." << std::endl;
  // Create the required stores.
  Verb::initialize();                 // verb dictionary
  Substance::initialize();             // materials
  PropPrototype::initialize();        // prop prototypes
  std::cout << "DEBUG: Created AppStateSplash." << std::endl;
}

AppStateSplash::~AppStateSplash()
{
  std::cout << "DEBUG: Destroying AppStateSplash." << std::endl;
}

void AppStateSplash::enterState()
{
  std::cout << "DEBUG: Entering state AppStateSplash." << std::endl;
  // TODO: Load Simple Logo

  clock.restart();
  std::cout << "DEBUG: Entered state AppStateSplash." << std::endl;
}

void AppStateSplash::leaveState()
{
  std::cout << "DEBUG: Leaving state AppStateSplash." << std::endl;
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

} // namespace rectopia
} // namespace gsl
