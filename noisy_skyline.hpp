#ifndef NOISY_SKYLINE_HPP_
#define NOISY_SKYLINE_HPP_

#include <random>

#include "types.hpp"

/**
 * This class emulates queries to independent noisy oracles.
 * It holds the real data and answers questions
 * "Is i-th item is less than j-th item on dimension k?"
 * with some predefined error probability.
 * Also, it keeps track of number of such queries.
 *
 * Derived oracles should either this class
 * or other derived oracles as a basis.
 *
 * After construction, there is no way to retreive
 * the dataset for which the skyline should be computed.
 * This ensures that derived oracles won't use
 * the numerical values in the dataset directly.
 */
class Oracle {
public:
    /**
     * Construct the oracle.
     *
     * @param dataset The dataset to use; must not be empty,
     *     all items in the dataset must have the same size.
     * @param errorProbability Ratio of correctness for queries to this oracle;
     *     must be in the range [0.0; 0.5).
     */
    Oracle(const Dataset& dataset, double errorProbability);

    /**
     * Total number of items in the dataset.
     */
    item_index itemCount() const;

    /**
     * Dimension of every item in the underlying dataset.
     */
    item_dimension itemDimension() const;

    /**
     * Probability of returning the wrong result in compare().
     */
    double errorProbability() const;

    /**
     * Is item i is less than item j on a dimension k?
     * The result is erroneous with probability errorProbability()
     * (i.e. correct with probability 1 - errorProbability()).
     */
    bool less(item_index i, item_index j, item_dimension k);

    /**
     * The total number of comparisons made (that is, number of calls to compare()).
     */
    int comparisonCount() const;

private:
    const Dataset dataset_;
    const double errorProbability_;
    std::mt19937 rng_;
    int comparisonCount_;
};

/**
 * Is item i is less than item j on a dimension k?
 *
 * To check if (i_k < j_k), use less(i, j, k, tolerance).
 * To check if (i_k > j_k), use less(j, i, k, tolerance).
 * To check if (i_k <= j_k), use !less(j, i, k, tolerance).
 * To check if (i_k >= j_k), use !less(i, j, k, tolerance).
 */
bool less(Oracle& oracle, item_index i, item_index j, item_dimension k, double tolerance);

/**
 * Assuming lexicographical ordering of dimensions, is item i is less than item j?
 */
bool lessLex(Oracle& oracle, item_index i, item_index j, double tolerance);

/**
 * Is item i dominated by item j?
 */
bool dominatedBy(Oracle& oracle, item_index i, item_index j, double tolerance);

/**
 * Is item i dominated by any of the items j in c?
 */
// TODO: Implement a variant of dominance check when the ordering of c is known along each dimension.
bool dominatedByAny(Oracle& oracle, item_index i, const ItemIndexSeq& c, double tolerance);

/**
 * Predicate for lexicographic non-dominance total order; used in maxLexNotDominated().
 *
 * @return ternary::true_ if either both items are not dominated and i < j, or i is domianted and j is not;
 *         ternary::false_ if either both items are not dominated and i > j, or i is not domianted and j is;
 *         ternary::unknown if both items are dominated.
 */
ternary lessLexNotDominated(Oracle& oracle, item_index i, item_index j, const ItemIndexSeq& c, double tolerance);

/**
 * The index of the maximum item between item i and item j that is not dominated by any item in c.
 *
 * @return index of the maximal item between i and j, or NULL_ITEM_INDEX if both i and j are dominated.
 */
item_index max2LexNotDominated(Oracle& oracle, item_index i, item_index j, const ItemIndexSeq& c, double tolerance);

/**
 * The index of the maximum item among the n items whose indices are in s, from the specified offset,
 * that is not dominated by any item in c.
 *
 * @param n Number of items to consider (counting from the offset); must be in the range [1..4].
 * @param offset The offset in the specified index sequence; must be less than the length of s.
 *
 * @return index of the maximal item among s, or NULL_ITEM_INDEX if all of them are domianted.
 */
item_index max4LexNotDominated(Oracle& oracle, const ItemIndexSeq& s,
        ItemIndexSeq::size_type offset, ItemIndexSeq::size_type n, const ItemIndexSeq& c, double tolerance);

/**
 * The index of the maximum item among the items whose indices are in s that is not dominated by any item in c.
 *
 * @return index of the maximal item among s, or NULL_ITEM_INDEX if all of them are domianted.
 */
item_index maxLexNotDominated(Oracle& oracle, const ItemIndexSeq& s, const ItemIndexSeq& c, double tolerance);

/**
 * Sample the items in s for skyline items at most n times.
 */
void skySample(Oracle& oracle, const ItemIndexSeq& s, unsigned long n, double tolerance, ItemIndexSeq& result);

/**
 * Compute skyline from items in s.
 */
void skyline(Oracle& oracle, const ItemIndexSeq& s, double tolerance, ItemIndexSeq& result);

/**
 * Compute skyline from the entire dataset.
 */
void fullSkyline(Oracle& oracle, double tolerance, ItemIndexSeq& result);

#endif // NOISY_SKYLINE_HPP_
