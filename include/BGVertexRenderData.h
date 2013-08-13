#ifndef GUIVERTEXRENDERDATA_H
#define GUIVERTEXRENDERDATA_H

/** Struct representing a background vertex to send to openGL. */
struct BGVertexRenderData
{
  BGVertexRenderData()
  {
    x = 0;
    y = 0;
    r = 0;
    g = 0;
    b = 0;
    a = 0;
  }

  BGVertexRenderData(float _x,
                      float _y,
                      float _r = 0.5,
                      float _g = 0.5,
                      float _b = 0.5,
                      float _a = 1.0)
  {
    x = _x;
    y = _y;
    r = _r;
    g = _g;
    b = _b;
    a = _a;
  }

  float x, y;               ///< Screen coordinates
  float r, g, b, a;         ///< Color
};

#endif // GUIVERTEXRENDERDATA_H
