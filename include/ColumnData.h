#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#ifndef COLUMNDATA_H_
#define COLUMNDATA_H_

/// Data for a single column in the stage.
struct ColumnData
{
  int initial_height;
  int solid_height;
  int render_height;
  int outdoor_height;
  bool rampTop;
  bool dirty;
};

#endif /* COLUMNDATA_H_ */
