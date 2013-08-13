// *** ADDED BY HEADER FIXUP ***
#include <ctime>
// *** END ***
#ifndef APPSTATESPLASH_H
#define APPSTATESPLASH_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "AppState.h"

/// Class representing the splash screen state of the application.
class AppStateSplash: public AppState
{
public:
  AppStateSplash(AppStateManager* manager);
  virtual ~AppStateSplash();

  void enter_state();
  void leave_state();
  void process();
  void render();

private:
  sf::Texture texLogo;
  sf::Sprite spriteLogo;
  sf::Clock clock;
};

#endif // CAPPSTATESPLASH_H
