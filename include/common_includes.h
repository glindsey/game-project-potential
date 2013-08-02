#ifndef COMMON_INCLUDES_H_INCLUDED
#define COMMON_INCLUDES_H_INCLUDED

// NOTE: I'm trying to get away from this all-in-one file, and use specific
//       includes instead to speed up compilation and rebuild time.
//       Thus, things will slowly disappear from this file as I optimize.

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <iostream>
#include <limits>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/set.hpp>
#include <boost/container/vector.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <memory>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <SFML/Graphics.hpp>

#endif // COMMON_INCLUDES_H_INCLUDED
