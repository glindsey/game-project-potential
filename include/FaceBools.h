/*
 * FaceBools.h
 *
 *  Created on: Nov 9, 2012
 *      Author: Gregory Lindsey
 */

#ifndef FACEBOOLS_H_
#define FACEBOOLS_H_

#include "common_includes.h"
#include "common_typedefs.h"

namespace gsl
{
namespace rectopia
{

/// A set of booleans representing the six sides of a cube, or other six-sided polytope.
class FaceBools
{
public:
  FaceBools();
  FaceBools(bool b);
  FaceBools(const FaceBools& original);
  ~FaceBools();

  void setAll(bool b);
  bool anyTrue() const;
  bool allTrue() const;

  bool top() const;
  bool bottom() const;
  bool left() const;
  bool right() const;
  bool back() const;
  bool front() const;

  void set_top(bool value);
  void set_bottom(bool value);
  void set_left(bool value);
  void set_right(bool value);
  void set_back(bool value);
  void set_front(bool value);

  FaceBools& operator=(const FaceBools& rhs);
  FaceBools& operator=(const bool& rhs);
  FaceBools& operator|=(const FaceBools& rhs);
  FaceBools& operator&=(const FaceBools& rhs);
  FaceBools& operator^=(const FaceBools& rhs);
  const FaceBools operator|(const FaceBools& other) const;
  const FaceBools operator&(const FaceBools& other) const;
  const FaceBools operator^(const FaceBools& other) const;
  bool operator==(const FaceBools& other) const;
  bool operator!=(const FaceBools& other) const;
  FaceBools operator!();
  FaceBools operator~();

private:
  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif /* FACEBOOLS_H_ */
