#include "AppState.h"

namespace gsl
{
namespace rectopia
{

AppState::AppState(AppStateManager* manager)
{
  manager_ = manager;
}

AppState::~AppState()
{
}

} // namespace rectopia
} // namespace gsl
