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

// Forward declarations
class FaceBools;
class Stage;
class StageChunkCollection;
class StageComponentVisitor;
class Substance;

/** A StageChunk is a manageable collection of blocks.  Splitting the stage into chunks
 *  allows for faster rendering and calculation when the stage contents change.
 *  StageChunks are always 1 Z-level in height; this is so the renderer can
 *  "strip away" layers of StageChunks in order to get to the cursor level,
 *  without having to re-render any vertex data.
 */
class StageChunk:
  public StageComponent,
  public boost::noncopyable
{
public:
  StageChunk(StageChunkCollection* parent,
             int chunk_index,
             StageCoord block_x,
             StageCoord block_y,
             StageCoord block_z);

  ~StageChunk();

  void accept(StageComponentVisitor& visitor);

  /** Gets this StageChunk's parent. */
  StageChunkCollection* get_parent() const;

  /** Gets this StageChunk's coordinates.
   *  Coordinates correspond to the upper-back-left corner of the Chunk. */
  StageCoord3 const& get_coords() const;

  /// Returns bool indicating whether this chunk is totally opaque.
  bool is_opaque(void);

  /// Returns bool indicating whether this chunk is totally solid.
  bool is_solid(void);

  /// Returns bool indicating whether this chunk is totally traversable.
  bool is_traversable(void);

  /// Returns bool indicating whether this chunk is visible in any way.
  bool is_visible(void);

  /// Returns bool indicating whether this chunk has any known blocks.
  bool is_known(void);

  /// Returns true if any faces are visible, false otherwise.
  /// Includes both solid AND fluid layers.
  bool has_any_visible_faces();

  /// Returns true if the chunk's render data needs to be recalculated AND if
  /// the stage indicates it is okay to render the map.
  bool is_render_data_dirty();

  /// Sets whether render data needs recalculating.
  void set_render_data_dirty(bool dirty);

  /// The constant, hard-coded chunk side length.
  static const StageCoord chunk_side_length = 32;

private:
  /// @note Normally this class would use a PIMPL idiom like the other classes
  ///       I've implemented.  However, due to the sheer number of StageChunks
  ///       we deal with, I'm trying to keep complexity down to a minimum.

  /** Pointer to the StageChunkCollection that owns this chunk. */
  StageChunkCollection* parent_;

  /** Absolute coordinates of this chunk.  Constant after initialization. */
  StageCoord3 coord_;

  /** Index of this chunk.  Constant after initialization. */
  int index_;

  /** Boolean indicating whether rendering data needs to be regenerated. */
  bool render_data_dirty_;

  /** Mutex for accessing blocks array. */
  boost::mutex blocks_mutex_;
};
#endif /* STAGECHUNK_H_ */
