/*
 * StageComponentVisitor.h
 *
 *  Created on: Nov 8, 2012
 *      Author: 310037506
 */

#ifndef STAGECOMPONENTVISITOR_H_
#define STAGECOMPONENTVISITOR_H_

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class Stage;
class Prop;
class StageBlock;
class StageChunk;
class StageChunkCollection;

/// Superclass for any visitors to stage components.
class StageComponentVisitor
{
public:
  virtual bool visit(Stage& stage) = 0;
  virtual bool visit(Prop& prop) = 0;
  virtual bool visit(StageBlock& getBlock) = 0;
  virtual bool visit(StageChunk& chunk) = 0;
  virtual bool visit(StageChunkCollection& collection) = 0;

protected:
private:
};

} // namespace rectopia
} // namespace gsl

#endif /* STAGECOMPONENTVISITOR_H_ */
