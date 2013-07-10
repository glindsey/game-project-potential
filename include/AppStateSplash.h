// *** ADDED BY HEADER FIXUP ***
#include <ctime>
// *** END ***
#ifndef APPSTATESPLASH_H
#define APPSTATESPLASH_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "AppState.h"

namespace gsl
{
namespace rectopia
{

/// Class representing the splash screen state of the application.
class AppStateSplash: public AppState
{
public:
  AppStateSplash(AppStateManager* manager);
  virtual ~AppStateSplash();

  void enterState();
  void leaveState();
  void process();
  void render();

private:
  sf::Texture texLogo;
  sf::Sprite spriteLogo;
  sf::Clock clock;
};

} // namespace rectopia
} // namespace gsl

#endif // CAPPSTATESPLASH_H
