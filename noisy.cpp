#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>

#include "common.hpp"

/* Ternary logic. */
enum class ternary {
    unknown = 0,
    false_ = 1,
    true_ = 2,
};

/** "Not found" skyline index. */
const size_type NULL_SKYLINE = std::numeric_limits<size_type>::max();

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
    size_type itemCount() const;

    /**
     * Dimension of every item in the underlying dataset.
     */
    size_type itemDimension() const;

    /**
     * Probability of returning the wrong result in compare().
     */
    double errorProbability() const;

    /**
     * Is item i is less than item j on a dimension k?
     * The result is erroneous with probability errorProbability()
     * (i.e. correct with probability 1 - errorProbability()).
     */
    bool less(size_type i, size_type j, size_type k);

    /**
     * The total number of comparisons made (that is, number of calls to compare()).
     */
    size_type comparisonCount() const;

private:
    const Dataset dataset_;
    const double errorProbability_;
    std::mt19937 rng_;
    size_type comparisonCount_;
};

/**
 * Is item i is less than item j on a dimension k?
 *
 * To check if (i_k < j_k), use less(i, j, k, tolerance).
 * To check if (i_k > j_k), use less(j, i, k, tolerance).
 * To check if (i_k <= j_k), use !less(j, i, k, tolerance).
 * To check if (i_k >= j_k), use !less(i, j, k, tolerance).
 */
bool less(Oracle& oracle, size_type i, size_type j, size_type k, double tolerance);

/**
 * Assuming lexicographical ordering of dimensions, is item i is less than item j?
 */
bool lessLex(Oracle& oracle, size_type i, size_type j, double tolerance);

/**
 * Is item i dominated by item j?
 */
bool dominatedBy(Oracle& oracle, size_type i, size_type j, double tolerance);

/**
 * Is item i dominated by any of the items j in c?
 */
// TODO: Implement a variant of dominance check when the ordering of c is known along each dimension.
bool dominatedByAny(Oracle& oracle, size_type i, const Skyline& c, double tolerance);

/**
 * Predicate for lexicographic non-dominance total order; used in maxLexNotDominated().
 *
 * @return ternary::true_ if either both items are not dominated and i < j, or i is domianted and j is not;
 *         ternary::false_ if either both items are not dominated and i > j, or i is not domianted and j is;
 *         ternary::unknown if both items are dominated.
 */
ternary lessLexNotDominated(Oracle& oracle, size_type i, size_type j, const Skyline& c, double tolerance);

/**
 * The index of the maximum item between item i and item j that is not dominated by any item in c.
 *
 * @return index of the maximal item between i and j, or NULL_SKYLINE if both i and j are dominated.
 */
size_type max2LexNotDominated(Oracle& oracle, size_type i, size_type j, const Skyline& c, double tolerance);

/**
 * The index of the maximum item among the n items whose indices are in s, from the specified offset,
 * that is not dominated by any item in c.
 *
 * @param n Number of items to consider (counting from the offset); must be in the range [1..4].
 * @param offset The offset in the specified index sequence; must be less than the length of s.
 *
 * @return index of the maximal item among s, or NULL_SKYLINE if all of them are domianted.
 */
size_type max4LexNotDominated(Oracle& oracle, const Skyline& s,
        size_type offset, size_type n, const Skyline& c, double tolerance);

/**
 * The index of the maximum item among the items whose indices are in s that is not dominated by any item in c.
 *
 * @return index of the maximal item among s, or NULL_SKYLINE if all of them are domianted.
 */
size_type maxLexNotDominated(Oracle& oracle, const Skyline& s, const Skyline& c, double tolerance);

/**
 * Sample the items in s for skyline items at most n times.
 */
void skySample(Oracle& oracle, const Skyline& s, size_type n, double tolerance, Skyline& result);

/**
 * Compute skyline from the entire dataset.
 */
void noisy(Oracle& oracle, double tolerance, Skyline& result);

/** Main entry point. */
int main(int argc, char** argv) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " input output size dimensions tolerance error_probability" << std::endl;
        return EXIT_FAILURE;
    }

    auto input = argv[1];
    auto output = argv[2];
    auto size = datasetSizeParse(argv[3]);
    auto dimensions = datasetSizeParse(argv[4]);
    auto tolerance = std::stod(argv[5]);
    auto errorProbability = std::stod(argv[6]);

    Dataset dataset(size, dimensions);
    datasetRead(dataset, input);

    Oracle oracle(dataset, errorProbability);
    Skyline skyline;
    auto beforeTime = std::chrono::steady_clock::now();
    noisy(oracle, tolerance, skyline);
    auto afterTime = std::chrono::steady_clock::now();

    std::sort(skyline.begin(), skyline.end());
    skylineWrite(skyline, output);

    auto runningTime = std::chrono::duration_cast<std::chrono::milliseconds>(afterTime - beforeTime).count();
    std::cout << runningTime << " " << oracle.comparisonCount() << std::endl;

    return EXIT_SUCCESS;
}

Oracle::Oracle(const Dataset& dataset, double errorProbability)
        : dataset_(dataset), errorProbability_(errorProbability), rng_(std::random_device()()), comparisonCount_(0) {
}

size_type Oracle::itemCount() const {
    return dataset_.size();
}

size_type Oracle::itemDimension() const {
    return dataset_.ndims();
}

double Oracle::errorProbability() const {
    return errorProbability_;
}

bool Oracle::less(size_type i, size_type j, size_type k) {
    bool correctResult = dataset_(i,k) < dataset_(j,k);
    comparisonCount_++;
    double sample = std::generate_canonical<double, std::numeric_limits<double>::digits>(rng_);
    return (sample < errorProbability_) ? !correctResult : correctResult;
}

size_type Oracle::comparisonCount() const {
    return comparisonCount_;
}

bool less(Oracle& oracle, size_type i, size_type j, size_type k, double tolerance) {
    // If the oracle is good enough, use it's result directly.
    // Otherwise, take the majority of 3 comparisons, while allowing 2*tolerance error probability.
    if (oracle.errorProbability() <= tolerance) {
        return oracle.less(i, j, k);
    } else {
        bool result1 = less(oracle, i, j, k, 2*tolerance);
        bool result2 = less(oracle, i, j, k, 2*tolerance);
        // Save 1 call to the underlying oracle if the first 2 results are the same.
        return (result1 ^ result2) ? less(oracle, i, j, k, 2*tolerance) : result1;
    }
}

bool lessLex(Oracle& oracle, size_type i, size_type j, double tolerance) {
    // TODO: Save calls to the underlying oracle by computing gt first and returning early?
    size_type lt = 0;
    for (; lt < oracle.itemDimension(); lt++) {
        if (less(oracle, i, j, lt, tolerance/2)) {
            break;
        }
    }
    size_type gt = 0;
    for (; gt < oracle.itemDimension(); gt++) {
        // i_gt > j_gt?
        if (less(oracle, j, i, gt, tolerance/2)) {
            break;
        }
    }
    return (gt == oracle.itemDimension()) || (lt <= gt);
}

bool dominatedBy(Oracle& oracle, size_type i, size_type j, double tolerance) {
    for (size_type k = 0; k < oracle.itemDimension(); k++) {
        // If item i is greater than item j on some dimension, then i is not dominated by j.
        if (less(oracle, j, i, k, tolerance)) {
            return false;
        }
    }
    return true;
}

bool dominatedByAny(Oracle& oracle, size_type i, const Skyline& c, double tolerance) {
    for (auto j : c) {
        if (dominatedBy(oracle, i, j, tolerance)) {
            return true;
        }
    }
    return false;
}

ternary lessLexNotDominated(Oracle& oracle, size_type i, size_type j, const Skyline& c, double tolerance) {
    if (dominatedByAny(oracle, i, c, tolerance)) {
        if (dominatedByAny(oracle, j, c, tolerance)) {
            // Both items are dominated, there is no ordering for them.
            return ternary::unknown;
        } else {
            // Item i is dominated, item j is NOT dominated: i < j
            return ternary::true_;
        }
    } else {
        if (dominatedByAny(oracle, j, c, tolerance)) {
            // Item i is NOT dominated, item j is dominated: i > j
            return ternary::false_;
        } else {
            // Both items i and j are not dominated: the result is determined by lexicographic ordering.
            return lessLex(oracle, i, j, tolerance) ? ternary::true_ : ternary::false_;
        }
    }
}

size_type max2LexNotDominated(Oracle& oracle, size_type i, size_type j, const Skyline& c, double tolerance) {
    if (i == NULL_SKYLINE) {
        if (j == NULL_SKYLINE) {
            return NULL_SKYLINE;
        } else {
            return j;
        }
    } else {
        if (j == NULL_SKYLINE) {
            return i;
        } else {
            switch (lessLexNotDominated(oracle, i, j, c, tolerance)) {
                case ternary::true_: {
                    return j;
                }
                case ternary::false_: {
                    return i;
                }
                case ternary::unknown: {
                    return NULL_SKYLINE;
                }
            }
        }
    }
}

size_type max4LexNotDominated(Oracle& oracle, const Skyline& s,
        Skyline::size_type offset, Skyline::size_type n, const Skyline& c, double tolerance) {
    switch (n) {
        case 1: {
            return s[offset];
        }
        case 2: {
            return max2LexNotDominated(oracle, s[offset], s[offset+1], c, tolerance);
        }
        case 3: {
            size_type max01 = max2LexNotDominated(oracle, s[offset], s[offset+1], c, tolerance/2);
            return max2LexNotDominated(oracle, max01, s[offset+2], c, tolerance/2);
        }
        case 4: {
            size_type max01 = max2LexNotDominated(oracle, s[offset], s[offset+1], c, tolerance/2);
            size_type max23 = max2LexNotDominated(oracle, s[offset+2], s[offset+3], c, tolerance/2);
            return max2LexNotDominated(oracle, max01, max23, c, tolerance/2);
        }
        default: {
            throw std::invalid_argument("n is not in range [1..4]");
        }
    }
}

size_type maxLexNotDominated(Oracle& oracle, const Skyline& s, const Skyline& c, double tolerance) {
    if (s.size() <= 4) {
        return max4LexNotDominated(oracle, s, 0, s.size(), c, tolerance);
    }
    Skyline smax((s.size() - 1) / 4 + 1);
    size_type i = 0;
    for (; i < smax.size() - 1; i++) {
        smax[i] = max4LexNotDominated(oracle, s, 4*i, 4, c, tolerance);
    }
    // The last group may be smaller.
    smax[i] = max4LexNotDominated(oracle, s, 4*i, s.size() - 4*i, c, tolerance);
    return maxLexNotDominated(oracle, smax, c, tolerance);
}

void skySample(Oracle& oracle, const Skyline& s, size_type n, double tolerance, Skyline& skyline) {
    skyline.clear();
    for (size_type i = 0; i < n; i++) {
        size_type z = maxLexNotDominated(oracle, s, skyline, tolerance/n);
        if (z == NULL_SKYLINE) {
            return;
        }
        skyline.push_back(z);
    }
}

void noisy(Oracle& oracle, double tolerance, Skyline& skyline) {
    Skyline s(oracle.itemCount());
    std::iota(s.begin(), s.end(), 0);
    // int i = 1;
    int pow2i = 2; // 2^i
    size_type ni = 4; // 2^(2^i)
    while (true) {
        skyline.clear();
        skySample(oracle, s, ni, tolerance/pow2i, skyline);
        if (skyline.size() < ni) {
            return;
        }
        // i += 1;
        pow2i *= 2;
        ni *= ni;
    }
}
