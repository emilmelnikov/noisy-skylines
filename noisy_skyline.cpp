#include "noisy_skyline.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>

#include "types.hpp"
#include "io.hpp"

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " input output tolerance error_probability" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1]);
    std::ofstream output(argv[2]);
    double tolerance;
    double errorProbability;
    try {
        tolerance = std::stod(argv[3]);
        errorProbability = std::stod(argv[4]);
        if (!(0.0 <= tolerance && tolerance < 0.5) || !(0.0 <= errorProbability && errorProbability < 0.5)) {
            throw std::invalid_argument("out of range");
        }
    } catch (std::invalid_argument e) {
        std::cerr << "tolerance and error_probability must be in range [0.0; 0.5)" << std::endl;
        return EXIT_FAILURE;
    }

    Dataset dataset;
    readDataset(input, dataset);

    Oracle oracle(dataset, errorProbability);
    ItemIndexSeq result;
    auto beforeTime = std::chrono::steady_clock::now();
    fullSkyline(oracle, tolerance, result);
    auto afterTime = std::chrono::steady_clock::now();

    std::sort(result.begin(), result.end());
    writeSkyline(output, result);

    auto runningTime = std::chrono::duration_cast<std::chrono::milliseconds>(afterTime - beforeTime).count();
    std::cout << runningTime << " " << oracle.comparisonCount() << std::endl;

    return EXIT_SUCCESS;
}

Oracle::Oracle(const Dataset& dataset, double errorProbability)
        : dataset_(dataset), errorProbability_(errorProbability), rng_(std::random_device()()), comparisonCount_(0) {
}

item_index Oracle::itemCount() const {
    return dataset_.size();
}

item_dimension Oracle::itemDimension() const {
    return dataset_[0].size();
}

double Oracle::errorProbability() const {
    return errorProbability_;
}

bool Oracle::less(item_index i, item_index j, item_dimension k) {
    bool correctResult = dataset_[i][k] < dataset_[j][k];
    comparisonCount_++;
    double sample = std::generate_canonical<double, std::numeric_limits<double>::digits>(rng_);
    return (sample < errorProbability_) ? !correctResult : correctResult;
}

int Oracle::comparisonCount() const {
    return comparisonCount_;
}

bool less(Oracle& oracle, item_index i, item_index j, item_dimension k, double tolerance) {
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

bool lessLex(Oracle& oracle, item_index i, item_index j, double tolerance) {
    // TODO: Save calls to the underlying oracle by computing gt first and returning early?
    item_dimension lt = 0;
    for (; lt < oracle.itemDimension(); lt++) {
        if (less(oracle, i, j, lt, tolerance/2)) {
            break;
        }
    }
    item_dimension gt = 0;
    for (; gt < oracle.itemDimension(); gt++) {
        // i_gt > j_gt?
        if (less(oracle, j, i, gt, tolerance/2)) {
            break;
        }
    }
    return (gt == oracle.itemDimension()) || (lt <= gt);
}

bool dominatedBy(Oracle& oracle, item_index i, item_index j, double tolerance) {
    for (item_dimension k = 0; k < oracle.itemDimension(); k++) {
        // If item i is greater than item j on some dimension, then i is not dominated by j.
        if (less(oracle, j, i, k, tolerance)) {
            return false;
        }
    }
    return true;
}

bool dominatedByAny(Oracle& oracle, item_index i, const ItemIndexSeq& c, double tolerance) {
    for (item_index j : c) {
        if (dominatedBy(oracle, i, j, tolerance)) {
            return true;
        }
    }
    return false;
}

ternary lessLexNotDominated(Oracle& oracle, item_index i, item_index j, const ItemIndexSeq& c, double tolerance) {
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

item_index max2LexNotDominated(Oracle& oracle, item_index i, item_index j, const ItemIndexSeq& c, double tolerance) {
    if (i == NULL_ITEM_INDEX) {
        if (j == NULL_ITEM_INDEX) {
            return NULL_ITEM_INDEX;
        } else {
            return j;
        }
    } else {
        if (j == NULL_ITEM_INDEX) {
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
                    return NULL_ITEM_INDEX;
                }
            }
        }
    }
}

item_index max4LexNotDominated(Oracle& oracle, const ItemIndexSeq& s,
        ItemIndexSeq::size_type offset, ItemIndexSeq::size_type n, const ItemIndexSeq& c, double tolerance) {
    switch (n) {
        case 1: {
            return s[offset];
        }
        case 2: {
            return max2LexNotDominated(oracle, s[offset], s[offset+1], c, tolerance);
        }
        case 3: {
            item_index max01 = max2LexNotDominated(oracle, s[offset], s[offset+1], c, tolerance/2);
            return max2LexNotDominated(oracle, max01, s[offset+2], c, tolerance/2);
        }
        case 4: {
            item_index max01 = max2LexNotDominated(oracle, s[offset], s[offset+1], c, tolerance/2);
            item_index max23 = max2LexNotDominated(oracle, s[offset+2], s[offset+3], c, tolerance/2);
            return max2LexNotDominated(oracle, max01, max23, c, tolerance/2);
        }
        default: {
            throw std::invalid_argument("n is not in range [1..4]");
        }
    }
}

item_index maxLexNotDominated(Oracle& oracle, const ItemIndexSeq& s, const ItemIndexSeq& c, double tolerance) {
    if (s.size() <= 4) {
        return max4LexNotDominated(oracle, s, 0, s.size(), c, tolerance);
    }
    ItemIndexSeq smax((s.size() - 1) / 4 + 1);
    item_index i = 0;
    for (; i < smax.size() - 1; i++) {
        smax[i] = max4LexNotDominated(oracle, s, 4*i, 4, c, tolerance);
    }
    // The last group may be smaller.
    smax[i] = max4LexNotDominated(oracle, s, 4*i, s.size() - 4*i, c, tolerance);
    return maxLexNotDominated(oracle, smax, c, tolerance);
}

void skySample(Oracle& oracle, const ItemIndexSeq& s, unsigned long n, double tolerance, ItemIndexSeq& result) {
    result.clear();
    for (unsigned long i = 0; i < n; i++) {
        item_index z = maxLexNotDominated(oracle, s, result, tolerance/n);
        if (z == NULL_ITEM_INDEX) {
            return;
        }
        result.push_back(z);
    }
}

void skyline(Oracle& oracle, const ItemIndexSeq& s, double tolerance, ItemIndexSeq& result) {
    // int i = 1;
    int pow2i = 2; // 2^i
    unsigned long ni = 4; // 2^(2^i)
    while (true) {
        result.clear();
        skySample(oracle, s, ni, tolerance/pow2i, result);
        if (result.size() < ni) {
            return;
        }
        // i += 1;
        pow2i *= 2;
        ni *= ni;
    }
}

void fullSkyline(Oracle& oracle, double tolerance, ItemIndexSeq& result) {
    ItemIndexSeq s(oracle.itemCount());
    std::iota(s.begin(), s.end(), 0);
    skyline(oracle, s, tolerance, result);
}