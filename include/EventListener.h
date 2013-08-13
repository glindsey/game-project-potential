/** Event Listener framework.
 * @author Gregory Lindsey
 * @note Derived from code by metacipherstudios at http://sdltutorials.org.
 */

#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include <SFML/Graphics.hpp>

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

  virtual EventResult handle_event(const sf::Event& event);

  virtual EventResult handle_focus_gained();
  virtual EventResult handle_focus_lost();
  virtual EventResult handle_key_down(sf::Event::KeyEvent key);
  virtual EventResult handle_key_up(sf::Event::KeyEvent key);
  virtual EventResult handle_mouse_move(sf::Event::MouseMoveEvent mouseMove);
  virtual EventResult handle_mouse_wheel(sf::Event::MouseWheelEvent mouseWheel);
  virtual EventResult handle_mouse_button_down(sf::Event::MouseButtonEvent mouseButton);
  virtual EventResult handle_mouse_button_up(sf::Event::MouseButtonEvent mouseButton);
  virtual EventResult handle_joy_button_down(sf::Event::JoystickButtonEvent joystickButton);
  virtual EventResult handle_joy_button_up(sf::Event::JoystickButtonEvent joystickButton);
  virtual EventResult handle_joy_move(sf::Event::JoystickMoveEvent joystickMove);
  virtual EventResult handle_window_exit();
  virtual EventResult handle_window_resize(int w, int h);
};

#endif // EVENTLISTENER_H
