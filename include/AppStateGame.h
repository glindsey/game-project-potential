#ifndef APPSTATEGAME_H
#define APPSTATEGAME_H

#include <boost/scoped_ptr.hpp>

#include "common.h"

#include "AppState.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class StageRenderer;

/// Class representing the game state of the application.
class AppStateGame: public AppState
{
public:
  /// Initializes the game state.
  /// @param manager State manager that the state belongs to.
  AppStateGame(AppStateManager* manager);
  virtual ~AppStateGame();

  /// Handle an incoming SFML event.
  /// This function first attempts to handle an event itself.  If it has no
  /// handler, it then tries to pass the event on in the following order:
  /// 1. GUI
  /// 2. Stage renderer
  /// 3. Game stage
  /// @param event The event to be handled.
  /// @return EventResult indicating if/how the event was handled.
  EventResult handleEvent(const sf::Event& event);

  void enterState();
  void leaveState();
  void process();
  void render();

private:
  struct Impl;

  /// Private implementation pointer
  boost::scoped_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif // APPSTATEGAME_H
