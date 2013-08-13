#ifndef APPLICATION_H
#define APPLICATION_H

#include <boost/random/mersenne_twister.hpp>
#include <memory>
#include <boost/thread/mutex.hpp>
#include <SFML/Window.hpp>

#include "common.h"

#include "EventListener.h"

#include "AppStateManager.h"
#include "FontCollection.h"

/// The primary application class.  This class is instantiated by main(), and
/// is the class wrapper for the entire program.  Since there's obviously one
/// instance of the application, it is a singleton class.
/// @author Gregory Lindsey
class App: public EventListener, public boost::noncopyable
{
public:
  /// Get the singleton instance of the application.
  /// @return Reference to the singleton instance of the app.
  static App& instance();

  /// Application destructor.
  /// The destructor tells the application state manager and the rendering
  /// thread to terminate.  It then joins the rendering thread.
  virtual ~App();

  /// Run the application.
  /// This function continues to run until haltProgram is set to true.
  void execute();

  /// Get the SFML window handle.
  /// @return Pointer to the SFML window.
  sf::Window& window() const;

  /// Get the random twister we use for generating random numbers.
  /// @return Reference to the twister.
  boost::random::mt19937& twister();

  /// Tell the application to halt.
  void halt();

  /// Return whether the application is running.
  bool isRunning();

  /// Get the StateManager instance.
  AppStateManager& getStateManager();

  /// Get the FontCollection instance.
  FontCollection& get_fonts();

protected:
private:
  /// Application constructor.  Private since Application is a singleton class.
  /// The constructor sets up SFML, OpenGL, and GLEW.  It creates the application
  /// screen, and ensures that the computer supports OpenGL 2.1 or greater, and
  /// has support for Vertex Buffer Objects (VBOs).  It also starts a separate
  /// thread to handle rendering.
  App();

  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;

  /// Processing function.
  /// The processing function is very straightforward:
  /// it polls the SFML window for events, and processes them when they come
  /// in.  It also runs the state manager process function.
  void process();

  /// Rendering function.
  /// Also very straightforward, the rendering function is responsible for
  /// rendering the GUI and stage to the screen regularly.
  /// @todo Restrict the maximum rendering speed to 60fps.
  void render();

  /// Application event handler.
  /// For events that come in, the Application tries to handle the event itself
  /// first; if it doesn't handle that event, it passes it along to the state
  /// manager.
  /// @param event Event to handle.
  /// @return Result of handling the event.
  EventResult handle_event(const sf::Event& event);

  /// Window closing handler.
  /// If the application sees that the window has been closed, it sets
  /// isRunning to false, which tells the processing and rendering loops to
  /// halt.
  EventResult handle_window_exit();

  /// Key down handler.
  /// This handler looks for application-wide keypresses, such as "Escape" to
  /// close the window.
  /// @todo Eventually Escape should be handled by the app state manager, since
  ///       we don't want the app to simply bail when Escape is pressed!
  EventResult handle_key_down(sf::Event::KeyEvent key);

};

#endif // CAPP_H
