#ifndef GUIVERTEXRENDERDATA_H
#define GUIVERTEXRENDERDATA_H

/** Struct representing a GUI vertex to send to openGL. */
struct GUIVertexRenderData
{
  GUIVertexRenderData()
  {
    x = 0;
    y = 0;
    r = 0;
    g = 0;
    b = 0;
    a = 0;
    s = 0;
    t = 0;
    textured = 0;
  }

  GUIVertexRenderData(float _x,
                      float _y,
                      float _r = 0.5,
                      float _g = 0.5,
                      float _b = 0.5,
                      float _a = 1.0,
                      float _s = 0,
                      float _t = 0,
                      float _textured = 0)
  {
    x = _x;
    y = _y;
    r = _r;
    g = _g;
    b = _b;
    a = _a;
    s = _s;
    t = _t;
    textured = _textured;
  }

  float x, y;               ///< Screen coordinates
  float r, g, b, a;         ///< Color
  float s, t;               ///< Texture coordinates
  float textured;           ///< >=0.5 means vertex is textured
};
#endif // GUIVERTEXRENDERDATA_H
