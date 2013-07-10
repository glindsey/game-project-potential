/*
 * StageChunk.h
 *
 *  Created on: Nov 16, 2012
 *      Author: 310037506
 */

#ifndef STAGECHUNK_H_
#define STAGECHUNK_H_

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

#include "common.h"

#include "StageBlock.h"
#include "StageComponent.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class FaceBools;
class Stage;
class StageComponentVisitor;
class Substance;

/** A StageChunk is a manageable collection of blocks.  Splitting the stage into chunks
 *  allows for faster rendering and calculation when the stage contents change.
 *  StageChunks are always 1 Z-level in height; this is so the renderer can
 *  "strip away" layers of StageChunks in order to get to the cursor level,
 *  without having to re-render any vertex data.
 */
class StageChunk:
  virtual public StageComponent,
  public boost::noncopyable
{
public:
  StageChunk(int idx, StageCoord x, StageCoord y, StageCoord z);
  ~StageChunk();

  void accept(StageComponentVisitor& visitor);

  /** Gets the block with the coordinates requested. */
  StageBlock& getBlock(StageCoord x, StageCoord y, StageCoord z);

  /** Gets a block using raw index. */
  StageBlock& getBlock(int idx);

  /** Gets this StageChunk's coordinates.
   *  Coordinates correspond to the upper-back-left corner of the Chunk. */
  StageCoord3 const& getCoordinates() const;

  /// Returns bool indicating whether this chunk is totally opaque.
  bool isOpaque(void);

  /// Returns bool indicating whether this chunk is totally solid.
  bool isSolid(void);

  /// Returns bool indicating whether this chunk is totally traversable.
  bool isTraversable(void);

  /// Returns bool indicating whether this chunk is visible in any way.
  bool isVisible(void);

  /// Returns bool indicating whether this chunk has any known blocks.
  bool isKnown(void);

  /// Returns true if any faces are visible, false otherwise.
  /// Includes both solid AND fluid layers.
  bool anyVisibleFaces();

  /// Returns true if the chunk's render data needs to be recalculated AND if
  /// the stage indicates it is okay to render the map.
  bool isRenderDataDirty();

  /// Sets whether render data needs recalculating.
  void setRenderDataDirty(bool dirty);

  /// The constant, hard-coded chunk side length.
  static const StageCoord ChunkSideLength = 64;

  /// The constant, hard-coded chunk size.
  static constexpr StageCoord ChunkSize = ChunkSideLength * ChunkSideLength;

private:
  /// @note Normally this class would use a PIMPL idiom like the other classes
  ///       I've implemented.  However, due to the sheer number of StageChunks
  ///       we deal with, I'm trying to keep complexity down to a minimum.

  int index(StageCoord x, StageCoord y);
  int index(StageCoord2 coord);

  /** Absolute coordinates of this chunk.  Constant after initialization. */
  StageCoord3 coord_;

  /** Index of this chunk.  Constant after initialization. */
  int index_;

  /** Boolean indicating whether rendering data needs to be regenerated. */
  bool render_data_dirty_;

  /** Memory pool for blocks in the chunk. */
  char block_pool_[sizeof(StageBlock) * StageChunk::ChunkSize];

  /** Array of pointers to blocks in the chunk.
   *  The blocks themselves will be stored in the block_pool_.
   */
  StageBlock* blocks_[StageChunk::ChunkSize];

  /** Mutex for accessing blocks array. */
  boost::mutex blocks_mutex_;
};

} // namespace rectopia
} // namespace gsl

#endif /* STAGECHUNK_H_ */
