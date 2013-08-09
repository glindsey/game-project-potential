#include "StageChunkCollection.h"

#include "MathUtils.h"
#include "Stage.h"
#include "StageChunk.h"
#include "StageComponentVisitor.h"

namespace gsl
{
namespace rectopia
{

struct StageChunkCollection::Impl
{
  inline int calc_chunk_index(int chunk_x, int chunk_y, int chunk_z)
  {
    return (chunk_z * (int)num_of_chunks.x * (int)num_of_chunks.y) +
           (chunk_y * (int)num_of_chunks.x) + chunk_x;
  }

  inline int calc_block_index(int block_x, int block_y, int block_z)
  {
    return (block_z * (int)num_of_blocks.x * (int)num_of_blocks.y) +
           (block_y * (int)num_of_blocks.x) + block_x;
  }

  inline int calc_block_index_of_chunk(int chunk_x, int chunk_y, int chunk_z)
  {
    int block_x = (chunk_x * StageChunk::chunk_side_length);
    int block_y = (chunk_y * StageChunk::chunk_side_length);
    int block_z = chunk_z;

    return calc_block_index(block_x, block_y, block_z);
  }

  inline StageChunk* get_chunk_location(int chunk_index)
  {
    StageChunk* chunks = reinterpret_cast<StageChunk*>(chunk_pool);
    return &(chunks[chunk_index]);
  }

  inline StageChunk* get_chunk_location(int chunk_x, int chunk_y, int chunk_z)
  {
    int chunk_index = calc_chunk_index(chunk_x, chunk_y, chunk_z);
    return get_chunk_location(chunk_index);
  }

  inline StageBlock* get_block_location_of_chunk(int chunk_x,
                                                 int chunk_y,
                                                 int chunk_z)
  {
    int block_index = calc_block_index_of_chunk(chunk_x, chunk_y, chunk_z);

    return get_block_location(block_index);
  }

  inline StageBlock* get_block_location(int block_index)
  {
    StageBlock* blocks = reinterpret_cast<StageBlock*>(block_pool);
    return &(blocks[block_index]);
  }

  inline StageBlock* get_block_location(int block_x, int block_y, int block_z)
  {
    int block_index = calc_block_index(block_x, block_y, block_z);
    return get_block_location(block_index);
  }

  Impl(StageCoord3 total_block_size)
  {
    num_of_blocks = total_block_size;

    std::cout << "Allocating a " << num_of_blocks.x << "x" <<
                                    num_of_blocks.y << "x" <<
                                    num_of_blocks.z <<
                                    " memory pool for StageBlocks...";

    unsigned int block_pool_size = num_of_blocks.x *
                                   num_of_blocks.y *
                                   num_of_blocks.z *
                                   sizeof(StageBlock);

    std::cout << "  (" << (float)(block_pool_size / 1048576) << " MiB in size)" << std::endl;

    block_pool = malloc(block_pool_size);

    if (block_pool == nullptr)
    {
      throw new std::bad_alloc;
    }


    num_of_chunks.x = (num_of_blocks.x / StageChunk::chunk_side_length) +
        ((num_of_blocks.x % StageChunk::chunk_side_length == 0) ? 0 : 1);
    num_of_chunks.y = (num_of_blocks.y / StageChunk::chunk_side_length) +
        ((num_of_blocks.y % StageChunk::chunk_side_length == 0) ? 0 : 1);
    num_of_chunks.z = num_of_blocks.z;

    std::cout << "Allocating a " << num_of_chunks.x << "x" <<
                                    num_of_chunks.y << "x" <<
                                    num_of_chunks.z <<
                                    " memory pool for StageChunks...";

    unsigned int chunk_pool_size = num_of_chunks.x *
                                   num_of_chunks.y *
                                   num_of_chunks.z *
                                   sizeof(StageChunk);

    std::cout << "  (" << (float)(chunk_pool_size / 1024) << " kiB in size.)" << std::endl;

    chunk_pool = malloc(chunk_pool_size);

    if (chunk_pool == nullptr)
    {
      throw new std::bad_alloc;
    }
  }

  ~Impl()
  {
#ifdef DEBUG_DESTROY_STAGECHUNK_INSTANCES
    std::cout << "Destroying StageChunk instances..." << std::endl;
    for (int idx = 0; idx < num_of_chunks_.z *
                            num_of_chunks_.y *
                            num_of_chunks_.x; ++idx)
    {
      chunks_[idx]->~StageChunk();
    }
#endif

    std::cout << "Deleting the memory pool for StageChunks..." << std::endl;
    if (chunk_pool != nullptr)
    {
      free(chunk_pool);
    }

    std::cout << "Deleting the memory pool for StageBlocks..." << std::endl;
    if (block_pool != nullptr)
    {
      free(block_pool);
    }
  }

  /// Pointer to a big ol' memory pool for StageChunks.
  void* chunk_pool;

  /// Pointer to a big ol' memory pool for StageBlocks.
  void* block_pool;

  /// Size of the stage, in StageBlocks.
  StageCoord3 num_of_blocks;

  /// Size of the stage, in StageChunks.
  StageCoord3 num_of_chunks;

};

StageChunkCollection::StageChunkCollection(StageCoord3 total_block_size)
  : impl(new Impl(total_block_size))
{
  std::cout << "Creating and initializing the StageBlock instances..." << std::endl;
  for (int block_z = 0; block_z < impl->num_of_blocks.z; ++block_z)
  {
    for (int block_y = 0; block_y < impl->num_of_blocks.y; ++block_y)
    {
      for (int block_x = 0; block_x < impl->num_of_blocks.x; ++block_x)
      {
        int block_index = impl->calc_block_index(block_x, block_y, block_z);


        // Figure out where in the block pool this block will go.
        StageBlock* block_ptr = impl->get_block_location(block_index);

        // Use placement new to construct the StageBlock.
        (void) new (block_ptr) StageBlock(block_x, block_y, block_z);
      }
    }
  }

  std::cout << "Creating and initializing the StageChunk instances..." << std::endl;

  for (int chunk_z = 0; chunk_z < impl->num_of_chunks.z; ++chunk_z)
  {
    for (int chunk_y = 0; chunk_y < impl->num_of_chunks.y; ++chunk_y)
    {
      for (int chunk_x = 0; chunk_x < impl->num_of_chunks.x; ++chunk_x)
      {
        int chunk_index = impl->calc_chunk_index(chunk_x, chunk_y, chunk_z);
        StageChunk* chunk_ptr = impl->get_chunk_location(chunk_index);

        StageCoord block_x = chunk_x * StageChunk::chunk_side_length;
        StageCoord block_y = chunk_y * StageChunk::chunk_side_length;
        StageCoord block_z = chunk_z;

        // Placement new means we don't have to save the return value, which
        // seems *really* weird, but that's how it is!
        (void) new (chunk_ptr) StageChunk(this, chunk_index,
                                          block_x, block_y, block_z);
      }
    }
  }
}

StageChunkCollection::~StageChunkCollection()
{
  std::cout << "Destroying StageChunkCollection..." << std::endl;
}

void StageChunkCollection::accept(StageComponentVisitor& visitor)
{
  // Visit the collection.  Note that this may not be strictly necessary.
  bool visitChildren = visitor.visit(*this);

  if (visitChildren)
  {
    // Visit the chunks existing in the collection.
    for (int chunk_z = 0; chunk_z < impl->num_of_chunks.z; ++chunk_z)
    {
      for (int chunk_y = 0; chunk_y < impl->num_of_chunks.y; ++chunk_y)
      {
        for (int chunk_x = 0; chunk_x < impl->num_of_chunks.x; ++chunk_x)
        {
          int chunk_index = impl->calc_chunk_index(chunk_x, chunk_y, chunk_z);
          StageChunk* chunk = impl->get_chunk_location(chunk_index);
          chunk->accept(visitor);
        }
      }
    }
  }
}

StageChunk& StageChunkCollection::getChunk(int idx)
{
  return *(impl->get_chunk_location(idx));
}

StageChunk& StageChunkCollection::get_chunk_containing(StageCoord block_x,
                                                     StageCoord block_y,
                                                     StageCoord block_z)
{
  int chunk_x = block_x / StageChunk::chunk_side_length;
  int chunk_y = block_y / StageChunk::chunk_side_length;
  int chunk_z = block_z;
  int chunk_index = impl->calc_chunk_index(chunk_x, chunk_y, chunk_z);
  return *(impl->get_chunk_location(chunk_index));
}

StageBlock& StageChunkCollection::get_block(StageCoord block_x,
                                           StageCoord block_y,
                                           StageCoord block_z)
{
  return *(getBlockPointer(block_x, block_y, block_z));
}

StageBlock* StageChunkCollection::getBlockPointer(StageCoord block_x,
                                                  StageCoord block_y,
                                                  StageCoord block_z)
{
  if ((block_x < 0) || (block_y < 0) || (block_z < 0) ||
      (block_x >= impl->num_of_blocks.x) ||
      (block_y >= impl->num_of_blocks.y) ||
      (block_z >= impl->num_of_blocks.z))
  {
    std::cout << "Attempt to get block (" <<
                  block_x << ", " <<
                  block_y << ", " <<
                  block_z << ")" << std::endl;
  }

  int block_index = impl->calc_block_index(block_x, block_y, block_z);
  return impl->get_block_location(block_index);
}

} // namespace rectopia
} // namespace gsl
