#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <limits>
#include <string>
#include <vector>

/**
 * Common type definitions.
 */

/** Type of the individual component in an item. */
using item_attribute = double;

/** Item in a multi-dimensional space. */
using Item = std::vector<item_attribute>;

/** Dimension index for an item. */
using item_dimension = Item::size_type;

/** A set of values for which the skyline should be computed. */
using Dataset = std::vector<Item>;

/** Index for items in a dataset. */
using item_index = Dataset::size_type;

/** A set of item indices, the subset of item indices in the dataset. */
using ItemIndexSeq = std::vector<item_index>;

enum {
    /** Representation of non-existent item index. */
    NULL_ITEM_INDEX = std::numeric_limits<item_index>::max(),
};

/** Value in a ternary (strong Kleene) logic. */
enum class ternary {
    unknown = 0,
    false_ = 1,
    true_ = 2,
};

#define string_to_item_attribute(s) (std::stod((s)))

#endif // TYPES_HPP_
