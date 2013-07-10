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
  int index(int x, int y, int z)
  {
    return (z * (int)collection_size_.x * (int)collection_size_.y) +
           (y * (int)collection_size_.x) + x;
  }

  Impl(StageCoord3 collection_size)
    : collection_size_(collection_size)
  {
    std::cout << "Allocating the memory pool for StageChunks..." << std::endl;
    unsigned int chunk_pool_size = collection_size_.x *
                                   collection_size_.y *
                                   collection_size_.z *
                                   sizeof(StageChunk);
    std::cout << "  (...which, by the way, is " << (float)(chunk_pool_size / 1048576) << " Mib in size.)" << std::endl;

    chunk_pool_ = malloc(chunk_pool_size);

    if (chunk_pool_ == nullptr)
    {
      throw new std::bad_alloc;
    }

    std::cout << "Creating and initializing the StageChunk instances..." << std::endl;
    chunks_.reset(new StageChunk*[collection_size_.x *
                                  collection_size_.y *
                                  collection_size_.z]);

    for (int z = 0; z < collection_size_.z; ++z)
    {
      std::cout << "-- Chunk layer " << z << "..." << std::endl;
      for (int y = 0; y < collection_size_.y; ++y)
      {
        for (int x = 0; x < collection_size_.x; ++x)
        {
          StageChunk* chunk_location = reinterpret_cast<StageChunk*>(chunk_pool_)
                                          + index(x, y, z);

          StageCoord xc = x * StageChunk::ChunkSideLength;
          StageCoord yc = y * StageChunk::ChunkSideLength;
          StageCoord zc = z;
          chunks_[index(x, y, z)] = new (chunk_location) StageChunk(index(x, y, z), xc, yc, zc);
        }
      }
    }
  }

  ~Impl()
  {
#ifdef DEBUG_DESTROY_STAGECHUNK_INSTANCES
    std::cout << "Destroying StageChunk instances..." << std::endl;
    for (int idx = 0; idx < collection_size_.z *
                            collection_size_.y *
                            collection_size_.x; ++idx)
    {
      chunks_[idx]->~StageChunk();
    }
#endif

    std::cout << "Deleting the memory pool for StageChunks..." << std::endl;
    if (chunk_pool_ != nullptr)
    {
      free(chunk_pool_);
    }
  }

  /// Pointer to a big ol' memory pool for StageChunks.
  void* chunk_pool_;

  /// Pointer to an array of StageChunk pointers.
  boost::scoped_array<StageChunk*> chunks_;

  /// Size of the collection.
  StageCoord3 collection_size_;
};

StageChunkCollection::StageChunkCollection(StageCoord3 collection_size)
  : impl(new Impl(collection_size))
{
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
    for (int z = 0; z < impl->collection_size_.z; ++z)
    {
      for (int y = 0; y < impl->collection_size_.y; ++y)
      {
        for (int x = 0; x < impl->collection_size_.x; ++x)
        {
          int idx = impl->index(x, y, z);
          impl->chunks_[idx]->accept(visitor);
        }
      }
    }
  }
}

StageChunk& StageChunkCollection::getChunk(int idx)
{
  return *(impl->chunks_[idx]);
}

StageChunk& StageChunkCollection::getChunkContaining(StageCoord x, StageCoord y, StageCoord z)
{
  int xc = x / StageChunk::ChunkSideLength;
  int yc = y / StageChunk::ChunkSideLength;
  int zc = z;
  int idx = impl->index(xc, yc, zc);
  return *(impl->chunks_[idx]);
}

StageBlock& StageChunkCollection::getBlock(StageCoord x, StageCoord y, StageCoord z)
{
  return getChunkContaining(x, y, z).getBlock(x, y, z);
}

} // namespace rectopia
} // namespace gsl
