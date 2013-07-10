/*
 * StageBuilderColumnShifter.h
 *
 *  Created on: Nov 29, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDERCOLUMNSHIFTER_H_
#define STAGEBUILDERCOLUMNSHIFTER_H_

#include "StageBuilder.h"

namespace gsl
{
namespace rectopia
{

/// This class takes the column heights stored in the stage column data
/// structure and shifts the stage blocks up or down as appropriate.
class StageBuilderColumnShifter: public gsl::rectopia::StageBuilder
{
public:
  /// Create the column shifter.
  /// @param stage Reference to stage to build on.
  /// @param seed Random seed to use.
  /// @param bias Bias value of the height map (average value).
  StageBuilderColumnShifter(Stage& stage, int seed, int bias);
  virtual ~StageBuilderColumnShifter();

  /// Shift the columns.
  /// @return False until finished; true when done.
  virtual bool Build();

  /// Reset the builder.
  virtual void Reset();

private:
  struct Impl;
  /// Private implementation pointer
  boost::scoped_ptr<Impl> impl;
};

} /* namespace rectopia */
} /* namespace gsl */
#endif /* STAGEBUILDERCOLUMNSHIFTER_H_ */
