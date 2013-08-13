// *** ADDED BY HEADER FIXUP ***
#include <iostream>
// *** END ***
#include "AppStateManager.h"

// Refer to your Other App States Here
#include "AppStateSplash.h"
#include "AppStateGame.h"

struct AppStateManager::Impl
{
  /// Pointer to the currently active app state.
  AppStateID activeAppState;

  /// Collection of app states.
  boost::ptr_map<AppStateID, AppState> appStates;
};

AppStateManager::AppStateManager()
  : impl(new Impl())
{
  AppStateID appStateSplash = AppStateID::SplashScreen;
  AppStateID appStateGame = AppStateID::Game;

  // Instantiate and push all of the requisite AppStates onto our structure.
  impl->appStates.insert(appStateSplash, new AppStateSplash(this));
  impl->appStates.insert(appStateGame, new AppStateGame(this));
}

AppStateManager::~AppStateManager()
{
}

/** Event handler.  Passes events on to the active app state. */
EventResult AppStateManager::handle_event(const sf::Event& event)
{
  if (impl->activeAppState != AppStateID::None)
  {
    AppState& state = impl->appStates.at(impl->activeAppState);
    EventResult result = state.handle_event(event);
    return result;
  }
  else
  {
    return EventResult::Ignored;
  }
}

void AppStateManager::process()
{
  if (impl->activeAppState != AppStateID::None)
  {
    impl->appStates.at(impl->activeAppState).process();
  }
}

void AppStateManager::render()
{
  if (impl->activeAppState != AppStateID::None)
  {
    impl->appStates.at(impl->activeAppState).render();
  }
}

void AppStateManager::SetActiveAppState(AppStateID state)
{
  if (impl->activeAppState != AppStateID::None)
  {
    impl->appStates.at(impl->activeAppState).leave_state();
  }

  impl->activeAppState = state;

  if (impl->activeAppState != AppStateID::None)
  {
    impl->appStates.at(impl->activeAppState).enter_state();
  }
}

AppStateID AppStateManager::GetActiveAppState() const
{
  return impl->activeAppState;
}
