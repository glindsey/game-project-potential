#ifndef APPSTATE_H
#define APPSTATE_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "EventListener.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class AppStateManager;

/// Application state framework.  All application states are derived from this
/// class.
class AppState: public EventListener, public boost::noncopyable
{
public:
  /// Constructor.
  /// @param manager State manager that this state belongs to.
  AppState(AppStateManager* manager);

  virtual ~AppState();

  virtual void enterState()
  {
  }

  virtual void leaveState()
  {
  }

  virtual void process()
  {
  }

  virtual void render()
  {
  }

protected:
  /// Reference to state manager that this state belongs to.
  AppStateManager* manager_;
};

} // namespace rectopia
} // namespace gsl

#endif // CAPPSTATE_H