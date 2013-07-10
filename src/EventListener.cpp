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

EventResult EventListener::handleEvent(const sf::Event& event)
{
  switch (event.type)
  {
  case sf::Event::GainedFocus:
    return this->handleFocusGained();

  case sf::Event::LostFocus:
    return this->handleFocusLost();

  case sf::Event::KeyPressed:
    return this->handleKeyDown(event.key);

  case sf::Event::KeyReleased:
    return this->handleKeyUp(event.key);

  case sf::Event::MouseMoved:
    return this->handleMouseMove(event.mouseMove);

  case sf::Event::MouseWheelMoved:
    return this->handleMouseWheel(event.mouseWheel);

  case sf::Event::MouseButtonPressed:
    return this->handleMouseButtonDown(event.mouseButton);

  case sf::Event::MouseButtonReleased:
    return this->handleMouseButtonUp(event.mouseButton);

  case sf::Event::JoystickButtonPressed:
    return this->handleJoyButtonDown(event.joystickButton);

  case sf::Event::JoystickButtonReleased:
    return this->handleJoyButtonUp(event.joystickButton);

  case sf::Event::JoystickMoved:
    return this->handleJoyMove(event.joystickMove);

  case sf::Event::Closed:
    return this->handleWindowExit();

  case sf::Event::Resized:
    return this->handleWindowResize(event.size.width, event.size.height);

  default:
    return EventResult::Unknown;
  }
}

EventResult EventListener::handleFocusGained()
{
  return EventResult::Ignored;
}
EventResult EventListener::handleFocusLost()
{
  return EventResult::Ignored;
}
EventResult EventListener::handleKeyDown(sf::Event::KeyEvent key)
{
  return EventResult::Ignored;
}
EventResult EventListener::handleKeyUp(sf::Event::KeyEvent key)
{
  return EventResult::Ignored;
}
EventResult EventListener::handleMouseMove(sf::Event::MouseMoveEvent mouseMove)
{
  return EventResult::Ignored;
}
EventResult EventListener::handleMouseWheel(sf::Event::MouseWheelEvent mouseWheel)
{
  return EventResult::Ignored;
}
EventResult EventListener::handleMouseButtonDown(sf::Event::MouseButtonEvent mouseButton)
{
  return EventResult::Ignored;
}
EventResult EventListener::handleMouseButtonUp(sf::Event::MouseButtonEvent mouseButton)
{
  return EventResult::Ignored;
}
EventResult EventListener::handleJoyButtonDown(sf::Event::JoystickButtonEvent joystickButton)
{
  return EventResult::Ignored;
}
EventResult EventListener::handleJoyButtonUp(sf::Event::JoystickButtonEvent joystickButton)
{
  return EventResult::Ignored;
}
EventResult EventListener::handleJoyMove(sf::Event::JoystickMoveEvent joystickMove)
{
  return EventResult::Ignored;
}
EventResult EventListener::handleWindowExit()
{
  return EventResult::Ignored;
}
EventResult EventListener::handleWindowResize(int w, int h)
{
  return EventResult::Ignored;
}

} // namespace rectopia
} // namespace gsl
