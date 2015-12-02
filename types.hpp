#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <limits>
#include <vector>

/**
 * Common type definitions.
 */

/** Type of the individual component in an item. */
typedef double item_attribute;

/** Item in a multi-dimensional space. */
typedef std::vector<item_attribute> Item;

/** Dimension index for an item. */
typedef Item::size_type item_dimension;

/** A set of values for which the skyline should be computed. */
typedef std::vector<Item> Dataset;

/** Index for items in a dataset. */
typedef Dataset::size_type item_index;

/** A set of item indices, the subset of item indices in the dataset. */
typedef std::vector<item_index> ItemIndexSeq;

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

#endif // TYPES_HPP_
