/*
 * StageRenderer3D.h
 *
 *  Created on: Nov 13, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGERENDERER3D_H_
#define STAGERENDERER3D_H_

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "StageRenderer.h"

class StageRenderer3D: public StageRenderer
{
public:
  StageRenderer3D();
  virtual ~StageRenderer3D();

  bool visit(Stage& stage);
  bool visit(Prop& prop);
  bool visit(StageBlock& get_block);
  bool visit(StageChunk& chunk);
  bool visit(StageChunkCollection& collection);

  void draw();

  EventResult handle_window_resize(int w, int h);
  EventResult handle_key_down(sf::Event::KeyEvent key);

protected:
private:
  struct Impl;

  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};
#endif /* STAGERENDERER3D_H_ */
