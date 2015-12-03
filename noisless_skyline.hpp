#ifndef NOISLESS_SKYLINE_HPP_
#define NOISLESS_SKYLINE_HPP_

#include <unordered_set>

#include "types.hpp"

/** An alternative to ItemIndexSeq if efficient removals are required. */
using ItemIndexSet = std::unordered_set<item_index>;

/**
 * Compute noisless skyline with output-sensitive algorithm.
 */
void noislessSkyline(const Dataset& dataset, ItemIndexSeq& result);

/**
 * Find maximal lexicographical element.
 */
item_index maxLex(const Dataset& dataset, const ItemIndexSet& items);

/**
 * Remove items that are dominated by maximum.
 */
void removeDominated(item_index maximum, const Dataset& dataset, ItemIndexSet& items);

#endif // NOISLESS_SKYLINE_HPP_
