#include "StageChunk.h"

#include <algorithm>

#include "ErrorMacros.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "StageComponentVisitor.h"

namespace gsl
{
namespace rectopia
{

const StageCoord StageChunk::ChunkSideLength;
constexpr StageCoord StageChunk::ChunkSize;

StageChunk::StageChunk(int idx, StageCoord x, StageCoord y, StageCoord z)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  coord_ = StageCoord3(x, y, z);
  index_ = idx;

  render_data_dirty_ = true;

  for (StageCoord y = 0; y < ChunkSideLength; ++y)
  {
    for (StageCoord x = 0; x < ChunkSideLength; ++x)
    {
      // Figure out where in the block pool the blocks will go.
      // (reinterpret_cast<> is normally SUPER EVIL but here it is a friend)
      StageBlock* block_location = reinterpret_cast<StageBlock*>(&block_pool_[0])
                                      + index(x, y);

      // "Allocate" memory out of the big block pool using placement new.
      // (Although in actuality we're not allocating anything!)
      blocks_[index(x, y)] = new (block_location) StageBlock(coord_.x + x, coord_.y + y, coord_.z);
    }
  }
}

StageChunk::~StageChunk()
{
  boost::mutex::scoped_lock lock(blocks_mutex_);
  std::cout << "Destroying StageChunk @ ("
            << coord_.x << ", "
            << coord_.y << ", "
            << coord_.z << ")..." << std::endl;

  // The destructors for the StageBlocks must be called EXPLICITLY
  // due to the placement new.
  for (StageCoord idx = 0; idx < ChunkSize; ++idx)
  {
    // Under other circumstances you'd be shot for doing this.  :)
    blocks_[idx]->~StageBlock();
  }
}

void StageChunk::accept(StageComponentVisitor& visitor)
{
  // Visit this chunk.
  bool visitChildren = visitor.visit(*this);

  if (visitChildren)
  {
    // Visit the blocks existing underneath this chunk.
    for (StageCoord idx = 0; idx < ChunkSize; ++idx)
    {
      blocks_[idx]->accept(visitor);
    }
  }
}

StageBlock& StageChunk::getBlock(StageCoord x, StageCoord y, StageCoord z)
{
  #ifndef NDEBUG
    if ((x - coord_.x < 0) || (x - coord_.x >= ChunkSideLength) ||
        (y - coord_.y < 0) || (y - coord_.y >= ChunkSideLength) ||
        (z != coord_.z))
    {
      MAJOR_ERROR("Attempt to get block (%d, %d, %d) from chunk @ (%d, %d, %d) which doesn't contain it",
                  x, y, z, coord_.x, coord_.y, coord_.z);
    }
  #endif // NDEBUG
  int idx = index(x - coord_.x, y - coord_.y);
  return *(blocks_[idx]);
}

StageBlock& StageChunk::getBlock(int idx)
{
  return *(blocks_[idx]);
}

StageCoord3 const& StageChunk::getCoordinates() const
{
  return coord_;
}

bool StageChunk::isOpaque(void)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  // Visit the blocks existing underneath this chunk.
  for (int idx = 0; idx < ChunkSize; ++idx)
  {
    StageBlock* block = blocks_[idx];
    if (!(block->isOpaque()))
    {
      return false;
    }
  }
  return true;
}

bool StageChunk::isSolid(void)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  // Visit the blocks existing underneath this chunk.
  for (int idx = 0; idx < ChunkSize; ++idx)
  {
    StageBlock* block = blocks_[idx];
    if (!(block->solid()))
    {
      return false;
    }
  }
  return true;
}

bool StageChunk::isTraversable(void)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  // Visit the blocks existing underneath this chunk.
  for (int idx = 0; idx < ChunkSize; ++idx)
  {
    StageBlock* block = blocks_[idx];
    if (block->traversable())
    {
      return true;
    }
  }
  return false;
}

bool StageChunk::isVisible(void)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  // Visit the blocks existing underneath this chunk.
  for (int idx = 0; idx < ChunkSize; ++idx)
  {
    StageBlock* block = blocks_[idx];
    if (block->visible())
    {
      return true;
    }
  }
  return false;
}

bool StageChunk::isKnown(void)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  // Visit the blocks existing underneath this chunk.
  for (int idx = 0; idx < ChunkSize; ++idx)
  {
    StageBlock* block = blocks_[idx];
    if (block->known())
    {
      return true;
    }
  }
  return false;
}

bool StageChunk::anyVisibleFaces()
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  // Visit the blocks existing underneath this chunk.
  for (int idx = 0; idx < ChunkSize; ++idx)
  {
    StageBlock* block = blocks_[idx];
    if (block->hasAnyVisibleFaces())
    {
      return true;
    }
  }
  return false;
}

bool StageChunk::isRenderDataDirty()
{
  return render_data_dirty_;
}

void StageChunk::setRenderDataDirty(bool dirty)
{
  render_data_dirty_ = dirty;
}

// === PRIVATE METHODS ========================================================
/** Get vector index for a block. */
int StageChunk::index(StageCoord x, StageCoord y)
{
  return ((int)y * (int)ChunkSideLength) + (int)x;
}

/** Get vector index for a block. */
int StageChunk::index(StageCoord2 coord)
{
  return index(coord.x, coord.y);
}

} // namespace rectopia
} // namespace gsl
