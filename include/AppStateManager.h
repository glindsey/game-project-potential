#ifndef APPSTATEMANAGER_H
#define APPSTATEMANAGER_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "AppState.h"

enum class AppStateID
{
  // Add your Other App States Here
  None = 0,
  SplashScreen,
  Game
};

/// Manager of all the application states.
class AppStateManager: public EventListener
{
public:
  /// Initializes the state manager.
  /// This function instantiates all of the possible app states, and assigns
  /// them to AppStateID tags.
  AppStateManager();

  AppStateManager(AppStateManager const&) = delete;
  AppStateManager& operator=(AppStateManager const&) = delete;


  ~AppStateManager();

  EventResult handle_event(const sf::Event& event);

  /// Call the process function of the active app state.
  /// @note Called in the processing thread ONLY!
  void process();

  /// Call the render function of the active app state.
  /// @note Called in the rendering thread ONLY!
  void render();

public:
  /// Set the active app state.
  /// When called, this function calls the leave_state function of the
  /// current state, switches states, and then calls the enter_state function
  /// of the new state.
  /// @param state ID of the application state to switch to.
  void SetActiveAppState(AppStateID state);

  /// Get the currently active app state.
  /// @return ID of the currently active application state.
  AppStateID GetActiveAppState() const;

private:
  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};

#endif // APPSTATEMANAGER_H
