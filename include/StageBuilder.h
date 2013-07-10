/*
 * StageBuilder.h
 *
 *  Created on: Nov 29, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDER_H_
#define STAGEBUILDER_H_

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

namespace gsl
{
namespace rectopia
{

/// Forward declarations
class Stage;

/// StageBuilder: Pure virtual interface for objects that help construct the stage.
class StageBuilder: public boost::noncopyable
{
public:
  /// Executes the build process on the stage.
  /// This function must be called repeatedly until it returns true to
  /// indicate that it has completed its process.  This allows the main
  /// thread to continue polling for events while the stage is being
  /// built.
  /// @return False if the process is not completed, True if it is.
  virtual bool Build() = 0;

  /// Resets the build process.
  /// This function resets the build process to the beginning.
  /// @warning At the moment this function is not required to revert
  ///          a partially-finished build process; thus, calling this
  ///          function on a partly-completed build process will lead
  ///          to unpredictable results!
  virtual void Reset() = 0;
};

} // namespace rectopia
} // namespace gsl

#endif /* STAGEBUILDER_H_ */
