#ifndef STAGECHUNKCOLLECTION_H_
#define STAGECHUNKCOLLECTION_H_

#include "common_includes.h"
#include "common_typedefs.h"

#include "StageComponent.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class Stage;
class StageBlock;
class StageChunk;
class StageComponentVisitor;

class StageChunkCollection: StageComponent
{
public:
  StageChunkCollection(StageCoord3 collection_size);
  ~StageChunkCollection();

  void accept(StageComponentVisitor& visitor);

  /// Get the chunk containing the requested block.
  StageChunk& getChunkContaining(StageCoord block_x,
                                 StageCoord block_y,
                                 StageCoord block_z);

  /// Get an individual chunk by index.
  StageChunk& getChunk(int chunk_index);

  /// Get an individual block in the collection.
  StageBlock& get_block(StageCoord block_x,
                       StageCoord block_y,
                       StageCoord block_z);

  /// Get a pointer to an individual block in the collection.
  StageBlock* getBlockPointer(StageCoord block_x,
                              StageCoord block_y,
                              StageCoord block_z);


private:
  struct Impl;
  /// Private implementation
  std::unique_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif /* STAGECHUNKCOLLECTION_H_ */
