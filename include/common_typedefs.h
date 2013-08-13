#ifndef COMMON_TYPEDEFS_H_INCLUDED
#define COMMON_TYPEDEFS_H_INCLUDED

/// Typedef to use for coordinates.
/// I'm using short ints; although they limit the total stage
/// size to 32767x32767x32767, that's already a ludicrously huge stage that
/// would take a modern computer several minutes per frame to update.
typedef short int StageCoord;

/// Struct for X/Y dimensions.
struct StageCoord2
{
  StageCoord2()
  {
    this->x = 0;
    this->y = 0;
  }
  StageCoord2(StageCoord n)
  {
    this->x = n;
    this->y = n;
  }
  StageCoord2(StageCoord x, StageCoord y)
  {
    this->x = x;
    this->y = y;
  }
  StageCoord2& operator=(const StageCoord2 &rhs)
  {
    if (this != &rhs)
    {
      this->x = rhs.x;
      this->y = rhs.y;
    }
    return *this;  // Return a reference to myself.
  }
  bool operator==(const StageCoord2 &other) const
  {
    return ((this->x == other.x) &&
            (this->y == other.y));
  }
  bool operator!=(const StageCoord2 &other) const
  {
    return !(*this == other);
  }

  StageCoord x;
  StageCoord y;
};

/// Struct for X/Y/Z dimensions.
struct StageCoord3
{
  StageCoord3()
  {
    this->x = 0;
    this->y = 0;
    this->z = 0;
  }
  StageCoord3(StageCoord n)
  {
    this->x = n;
    this->y = n;
    this->z = n;
  }
  StageCoord3(StageCoord x, StageCoord y, StageCoord z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  StageCoord3& operator=(const StageCoord3 &rhs)
  {
    if (this != &rhs)
    {
      this->x = rhs.x;
      this->y = rhs.y;
      this->z = rhs.z;
    }
    return *this;  // Return a reference to myself.
  }
  bool operator==(const StageCoord3 &other) const
  {
    return ((this->x == other.x) &&
            (this->y == other.y) &&
            (this->z == other.z));
  }
  bool operator!=(const StageCoord3 &other) const
  {
    return !(*this == other);
  }

  StageCoord x;
  StageCoord y;
  StageCoord z;
};

/// Typedef to indicate that a value should be interpreted as a percentage.
typedef double Percentage;

/// "SerialNumber" typedef used for enumerating tangible objects.
/// An int will allow for a total of 4,294,967,295 Props before
/// we run out of numbers.  I HOPE this is sufficient.  If not, we'll
/// move to a long long int, which gives us 18,446,744,073,709,551,615 Props.
/// I'm just trying to avoid that as 64-bit numbers can be a pain to print out.
/// Other options: GUIDs, or identifiers incorporating the creation date/time.
typedef unsigned int SerialNumber;
const SerialNumber SERIALNUMBER_NULL = (SerialNumber) (-1);

#endif // COMMON_TYPEDEFS_H_INCLUDED
