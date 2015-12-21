#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_set>

#include "common.hpp"

/** Skyline indices stored in a hash table for fast removals. */
using SkylineSet = std::unordered_set<size_type>;

/** Find maximal lexicographical element. */
size_type maxLex(const Dataset& dataset, const SkylineSet& items);

/** Remove items that are dominated by maximum. */
void removeDominated(size_type max, const Dataset& dataset, SkylineSet& items);

/** Compute noisless skyline with output-sensitive algorithm. */
void noisless(const Dataset& dataset, Skyline& skyline);

/** Total number of performed comparisons. */
static size_type comparisonCount = 0;

/** Main entry point. */
int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " input output size dimensions" << std::endl;
        return EXIT_FAILURE;
    }

    auto input = argv[1];
    auto output = argv[2];
    auto size = datasetSizeParse(argv[3]);
    auto dimensions = datasetSizeParse(argv[4]);

    Dataset dataset(size, dimensions);
    datasetRead(dataset, input);

    Skyline skyline;
    auto beforeTime = std::chrono::steady_clock::now();
    noisless(dataset, skyline);
    auto afterTime = std::chrono::steady_clock::now();

    std::sort(skyline.begin(), skyline.end());
    skylineWrite(skyline, output);

    auto runningTime = std::chrono::duration_cast<std::chrono::milliseconds>(afterTime - beforeTime).count();
    std::cout << runningTime << " " << comparisonCount << std::endl;

    return EXIT_SUCCESS;
}

size_type maxLex(const Dataset& dataset, const SkylineSet& items) {
    auto max = *items.cbegin();

    for (auto item : items) {
        if (item == max) {
            continue;
        }

        for (size_type k = 0; k < dataset.ndims(); k++) {
            bool gt = dataset(item,k) > dataset(max,k);
            comparisonCount++;
            if (gt) {
                max = item;
                break;
            }

            bool lt = dataset(item,k) < dataset(max,k);
            comparisonCount++;
            if (lt) {
                break;
            }
        }
    }

    return max;
}

void removeDominated(size_type max, const Dataset& dataset, SkylineSet& items) {
    auto it = items.begin();
    while (it != items.end()) {
        bool lt = false;
        size_type k = 0;
        for (; k < dataset.ndims(); k++) {
            bool gt = dataset(*it,k) > dataset(max,k);
            comparisonCount++;
            if (gt) {
                break;
            } else if (!lt) {
                lt = dataset(*it,k) < dataset(max,k);
                comparisonCount++;
            }
        }

        if (k == dataset.ndims() && lt) {
            it = items.erase(it);
        } else {
            it++;
        }
    }
}

void noisless(const Dataset& dataset, Skyline& skyline) {
    skyline.clear();
    comparisonCount = 0;

    SkylineSet notDominated;
    for (size_type i = 0; i < dataset.size(); i++) {
        notDominated.insert(i);
    }

    while (!notDominated.empty()) {
        size_type max = maxLex(dataset, notDominated);
        notDominated.erase(max);
        removeDominated(max, dataset, notDominated);
        skyline.push_back(max);
    }
}
