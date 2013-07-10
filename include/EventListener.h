/** Event Listener framework.
 * @author Gregory Lindsey
 * @note Derived from code by metacipherstudios at http://sdltutorials.org.
 */

#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include <SFML/Graphics.hpp>

namespace gsl
{
namespace rectopia
{

/// Enum for results of processing an event.
enum class EventResult
{
  Pending, ///< The event has not been handled by any handler yet; only for variable initialization.
  Handled,        ///< The event was handled and should not be passed on
  Acknowledged,   ///< The event was handled, but should still be passed on
  Ignored,        ///< The event was ignored, and should be passed on
  Unknown ///< The event type is unknown; further action is at handler's discretion
};

/// Base class for any class that can process events.
class EventListener
{
public:
  EventListener();
  virtual ~EventListener();

  virtual EventResult handleEvent(const sf::Event& event);

  virtual EventResult handleFocusGained();
  virtual EventResult handleFocusLost();
  virtual EventResult handleKeyDown(sf::Event::KeyEvent key);
  virtual EventResult handleKeyUp(sf::Event::KeyEvent key);
  virtual EventResult handleMouseMove(sf::Event::MouseMoveEvent mouseMove);
  virtual EventResult handleMouseWheel(sf::Event::MouseWheelEvent mouseWheel);
  virtual EventResult handleMouseButtonDown(sf::Event::MouseButtonEvent mouseButton);
  virtual EventResult handleMouseButtonUp(sf::Event::MouseButtonEvent mouseButton);
  virtual EventResult handleJoyButtonDown(sf::Event::JoystickButtonEvent joystickButton);
  virtual EventResult handleJoyButtonUp(sf::Event::JoystickButtonEvent joystickButton);
  virtual EventResult handleJoyMove(sf::Event::JoystickMoveEvent joystickMove);
  virtual EventResult handleWindowExit();
  virtual EventResult handleWindowResize(int w, int h);
};

} // namespace rectopia
} // namespace gsl

#endif // EVENTLISTENER_H
