#ifndef NL_SKYLINE_HPP_
#define NL_SKYLINE_HPP_

#include "types.hpp"

/**
 * Compute noisless skyline with naive nested loops algorithm.
 */
void nlSkyline(const Dataset& dataset, ItemIndexSeq& result);

#endif // NL_SKYLINE_HPP_
