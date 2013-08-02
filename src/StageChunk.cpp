#include "StageChunk.h"

#include <algorithm>

#include "ErrorMacros.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "StageChunkCollection.h"
#include "StageComponentVisitor.h"

namespace gsl
{
namespace rectopia
{

const StageCoord StageChunk::chunk_side_length;

StageChunk::StageChunk(StageChunkCollection* parent,
                       int chunk_index,
                       StageCoord block_x,
                       StageCoord block_y,
                       StageCoord block_z)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  parent_ = parent;
  index_ = chunk_index;
  coord_ = StageCoord3(block_x, block_y, block_z);

  render_data_dirty_ = true;
}

StageChunk::~StageChunk()
{
  /* No destructor calls needed here.  If they ARE required later they will
     go into StageChunkCollection::~StageChunkCollection(). */
}

void StageChunk::accept(StageComponentVisitor& visitor)
{
  // Visit this chunk.
  bool visitChildren = visitor.visit(*this);

  if (visitChildren)
  {
    // Visit the blocks existing underneath this chunk.

    for (StageCoord add_y = 0; add_y < chunk_side_length; ++add_y)
    {
      for (StageCoord add_x = 0; add_x < chunk_side_length; ++add_x)
      {
        // Figure out where in the block pool this block is.
        StageBlock* block_location = parent_->getBlockPointer(coord_.x + add_x,
                                                              coord_.y + add_y,
                                                              coord_.z);

        // Visit this block.
        block_location->accept(visitor);
      }
    }
  }
}

StageChunkCollection* StageChunk::get_parent() const
{
  return parent_;
}

StageCoord3 const& StageChunk::get_coords() const
{
  return coord_;
}

bool StageChunk::is_opaque(void)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  // Visit the blocks existing underneath this chunk.

  for (StageCoord add_y = 0; add_y < chunk_side_length; ++add_y)
  {
    for (StageCoord add_x = 0; add_x < chunk_side_length; ++add_x)
    {
      // Figure out where in the block pool this block is.
      StageBlock* block_location = parent_->getBlockPointer(coord_.x + add_x,
                                                            coord_.y + add_y,
                                                            coord_.z);

      if (!(block_location->is_opaque()))
      {
        return false;
      }
    }
  }
  return true;
}

bool StageChunk::is_solid(void)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  for (StageCoord add_y = 0; add_y < chunk_side_length; ++add_y)
  {
    for (StageCoord add_x = 0; add_x < chunk_side_length; ++add_x)
    {
      // Figure out where in the block pool this block is.
      StageBlock* block_location = parent_->getBlockPointer(coord_.x + add_x,
                                                            coord_.y + add_y,
                                                            coord_.z);

      if (!(block_location->is_solid()))
      {
        return false;
      }
    }
  }
  return true;
}

bool StageChunk::is_traversable(void)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  for (StageCoord add_y = 0; add_y < chunk_side_length; ++add_y)
  {
    for (StageCoord add_x = 0; add_x < chunk_side_length; ++add_x)
    {
      // Figure out where in the block pool this block is.
      StageBlock* block_location = parent_->getBlockPointer(coord_.x + add_x,
                                                            coord_.y + add_y,
                                                            coord_.z);

      if (block_location->is_traversable())
      {
        return true;
      }
    }
  }
  return false;
}

bool StageChunk::is_visible(void)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  for (StageCoord add_y = 0; add_y < chunk_side_length; ++add_y)
  {
    for (StageCoord add_x = 0; add_x < chunk_side_length; ++add_x)
    {
      // Figure out where in the block pool this block is.
      StageBlock* block_location = parent_->getBlockPointer(coord_.x + add_x,
                                                            coord_.y + add_y,
                                                            coord_.z);

      if (block_location->is_visible())
      {
        return true;
      }
    }
  }

  return false;
}

bool StageChunk::is_known(void)
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  for (StageCoord add_y = 0; add_y < chunk_side_length; ++add_y)
  {
    for (StageCoord add_x = 0; add_x < chunk_side_length; ++add_x)
    {
      // Figure out where in the block pool this block is.
      StageBlock* block_location = parent_->getBlockPointer(coord_.x + add_x,
                                                            coord_.y + add_y,
                                                            coord_.z);

      if (block_location->is_known())
      {
        return true;
      }
    }
  }
  return false;
}

bool StageChunk::has_any_visible_faces()
{
  boost::mutex::scoped_lock lock(blocks_mutex_);

  for (StageCoord add_y = 0; add_y < chunk_side_length; ++add_y)
  {
    for (StageCoord add_x = 0; add_x < chunk_side_length; ++add_x)
    {
      // Figure out where in the block pool this block is.
      StageBlock* block_location = parent_->getBlockPointer(coord_.x + add_x,
                                                            coord_.y + add_y,
                                                            coord_.z);

      if (block_location->has_any_visible_faces())
      {
        return true;
      }
    }
  }
  return false;
}

bool StageChunk::is_render_data_dirty()
{
  return render_data_dirty_;
}

void StageChunk::set_render_data_dirty(bool dirty)
{
  render_data_dirty_ = dirty;
}

} // namespace rectopia
} // namespace gsl
