/** Event Listener framework.
 * @author Gregory Lindsey
 * @note Derived from code by metacipherstudios at http://sdltutorials.org.
 */

#include "EventListener.h"

namespace gsl
{
namespace rectopia
{

EventListener::EventListener()
{
}

EventListener::~EventListener()
{
  //Do nothing
}

EventResult EventListener::handle_event(const sf::Event& event)
{
  switch (event.type)
  {
  case sf::Event::GainedFocus:
    return this->handle_focus_gained();

  case sf::Event::LostFocus:
    return this->handle_focus_lost();

  case sf::Event::KeyPressed:
    return this->handle_key_down(event.key);

  case sf::Event::KeyReleased:
    return this->handle_key_up(event.key);

  case sf::Event::MouseMoved:
    return this->handle_mouse_move(event.mouseMove);

  case sf::Event::MouseWheelMoved:
    return this->handle_mouse_wheel(event.mouseWheel);

  case sf::Event::MouseButtonPressed:
    return this->handle_mouse_button_down(event.mouseButton);

  case sf::Event::MouseButtonReleased:
    return this->handle_mouse_button_up(event.mouseButton);

  case sf::Event::JoystickButtonPressed:
    return this->handle_joy_button_down(event.joystickButton);

  case sf::Event::JoystickButtonReleased:
    return this->handle_joy_button_up(event.joystickButton);

  case sf::Event::JoystickMoved:
    return this->handle_joy_move(event.joystickMove);

  case sf::Event::Closed:
    return this->handle_window_exit();

  case sf::Event::Resized:
    return this->handle_window_resize(event.size.width, event.size.height);

  default:
    return EventResult::Unknown;
  }
}

EventResult EventListener::handle_focus_gained()
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_focus_lost()
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_key_down(sf::Event::KeyEvent key)
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_key_up(sf::Event::KeyEvent key)
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_mouse_move(sf::Event::MouseMoveEvent mouseMove)
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_mouse_wheel(sf::Event::MouseWheelEvent mouseWheel)
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_mouse_button_down(sf::Event::MouseButtonEvent mouseButton)
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_mouse_button_up(sf::Event::MouseButtonEvent mouseButton)
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_joy_button_down(sf::Event::JoystickButtonEvent joystickButton)
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_joy_button_up(sf::Event::JoystickButtonEvent joystickButton)
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_joy_move(sf::Event::JoystickMoveEvent joystickMove)
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_window_exit()
{
  return EventResult::Ignored;
}
EventResult EventListener::handle_window_resize(int w, int h)
{
  return EventResult::Ignored;
}

} // namespace rectopia
} // namespace gsl
