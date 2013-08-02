#include "Application.h"

#include <iostream>
#include <cstdio>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "ErrorMacros.h"
#include "FPSControl.h"
#include "Settings.h"

namespace gsl
{
namespace rectopia
{

struct App::Impl
{
  /// Pointer to the application instance.
  static std::unique_ptr<App> instance_;

  /// Random twister we use for generating all sorts of nifty crap.
  boost::random::mt19937 twister_;

  /// Pointer to the main window.
  std::unique_ptr<sf::Window> window_;

  // Mutex for the main window.
  boost::mutex window_mutex_;

  /// Boolean requesting complete shutdown of the program.
  bool halt_program_;

  /// State manager
  std::unique_ptr<AppStateManager> state_manager_;

  /// Font collection
  std::unique_ptr<FontCollection> font_collection_;

  /// Frame timer for rendering thread.
  FrequencyControl frame_timer_;

  /// Timer for processing thread.
  FrequencyControl process_timer_;
};

std::unique_ptr<App> App::Impl::instance_;

App::App()
  : impl(new Impl())
{
  // Create a new window.
  sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
  impl->window_.reset(new sf::Window());
  impl->window_->create(sf::VideoMode(1024, 576, desktop.bitsPerPixel),
                        "Potential", sf::Style::Default,
                        sf::ContextSettings(32, 32, 4, 3, 3));
  impl->window_->setActive(true);

  // Read the settings.xml file.
  Settings::Initialize();

  // Create the font collection.
  impl->font_collection_.reset(new FontCollection());

}

App::~App()
{
}

void App::execute()
{
  sf::Event event;

  // Create the state manager.
  impl->state_manager_.reset(new AppStateManager());

  // Set the app state manager to the splash screen.
  impl->state_manager_->SetActiveAppState(AppStateID::SplashScreen);

  // Set the "halt program" flag to false.
  impl->halt_program_ = false;

  // DEACTIVATE the window's rendering context.
  impl->window_->setActive(false);

  // Start our two threads.
  std::cout << "Starting processing thread..." << std::endl;
  boost::thread processing_thread(&App::process, this);

  std::cout << "Starting rendering thread..." << std::endl;
  boost::thread rendering_thread(&App::render, this);

  while (!(impl->halt_program_))
  {
    while (impl->window_->pollEvent(event))
    {
      handleEvent(event);
    }
    boost::thread::yield();
  }

  // Join the rendering thread until it quits.
  rendering_thread.join();

  // Join the processing thread until it quits.
  processing_thread.join();

  // Set the app state manager to none (which will clean up the last state).
  impl->state_manager_->SetActiveAppState(AppStateID::None);
}

void App::process()
{
  std::cout << "Processing thread started." << std::endl;

  while (!(impl->halt_program_))
  {
    impl->process_timer_.loop();
    impl->state_manager_->process();
    boost::thread::yield();
  }
  std::cout << "Processing thread terminating." << std::endl;
}

void App::render()
{
  char buffer[255];
  sf::Window& window = *(impl->window_.get());

  std::cout << "Rendering thread started." << std::endl;

  window.setActive(true);

  // Initialize GLEW.
  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    FATAL_ERROR("Error initializing GLEW: %s", glewGetErrorString(err));
  }
  if (!GLEW_VERSION_3_3 )  // check that the machine supports OpenGL 3.2.
  {
    FATAL_ERROR("OpenGL 3.3 support is required");
  }

  // Set up the OpenGL depth buffer.
  glDepthFunc(GL_LEQUAL);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_DEPTH_TEST);

  // Set up alpha blending.
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  // Some stuff to make rendering a bit nicer.
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  while (!(impl->halt_program_))
  {
    impl->frame_timer_.loop();
    std::snprintf(buffer, 255,
                  "Rectopia -- %3.2f FPS (%3.2f) / %3.2f PPS (%3.2f)",
                  impl->frame_timer_.getFrequency(),
                  impl->frame_timer_.getFilteredFrequency(),
                  impl->process_timer_.getFrequency(),
                  impl->process_timer_.getFilteredFrequency());

    impl->state_manager_->render();
    impl->window_->setTitle(buffer);
    impl->window_->display();

    boost::thread::yield();
  }

  std::cout << "Rendering thread terminating." << std::endl;
}

EventResult App::handleEvent(const sf::Event& event)
{
  EventResult result = EventResult::Pending;
  // Send the event to the proper listeners.
  result = EventListener::handleEvent(event);
  if (result != EventResult::Handled)
  {
    result = impl->state_manager_->handleEvent(event);
  }
  return result;
}

EventResult App::handleWindowExit()
{
  impl->halt_program_ = true;
  return EventResult::Handled;
}

sf::Window& App::window() const
{
  return *(impl->window_.get());
}

boost::random::mt19937& App::twister()
{
  return impl->twister_;
}

void App::halt()
{
  impl->halt_program_ = true;
}

bool App::isRunning()
{
  return (impl->halt_program_);
}

App& App::instance()
{
  // Instantiate application if it hasn't been yet.
  if (Impl::instance_.get() == nullptr)
  {
    Impl::instance_.reset(new App());
  }

  return *(Impl::instance_.get());
}

AppStateManager& App::getStateManager()
{
  return *(impl->state_manager_.get());
}

FontCollection& App::getFonts()
{
  return *(impl->font_collection_.get());
}

EventResult App::handleKeyDown(sf::Event::KeyEvent key)
{
  if (key.code == sf::Keyboard::Escape)
  {
    impl->halt_program_ = true;
    return EventResult::Handled;
  }
  return EventResult::Ignored;
}

} // namespace rectopia
} // namespace gsl
