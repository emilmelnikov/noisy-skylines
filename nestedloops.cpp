#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>

#include "common.hpp"

/** Compute noisless skyline with simple nested loops. */
void nestedloops(const Dataset& dataset, Skyline& skyline);

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
    nestedloops(dataset, skyline);
    auto afterTime = std::chrono::steady_clock::now();

    std::sort(skyline.begin(), skyline.end());
    skylineWrite(skyline, output);

    auto runningTime = std::chrono::duration_cast<std::chrono::milliseconds>(afterTime - beforeTime).count();
    std::cout << runningTime << " " << comparisonCount << std::endl;

    return EXIT_SUCCESS;
}

void nestedloops(const Dataset& dataset, Skyline& skyline) {
    skyline.clear();
    comparisonCount = 0;
    for (size_type i = 0; i < dataset.size(); i++) {
        bool inSkyline = true;
        // Try to find item j that dominates item i.
        for (size_type j = 0; j < dataset.size(); j++) {
            bool lt = false;
            size_type k = 0;
            for (; k < dataset.ndims(); k++) {
                bool gt = dataset(i,k) > dataset(j,k);
                comparisonCount++;
                if (gt) {
                    // Item i is not dominated by item j.
                    break;
                } else if (!lt) {
                    // Item i is less than item j on at least one dimension.
                    lt = dataset(i,k) < dataset(j,k);
                    comparisonCount++;
                }
            }
            if (k == dataset.ndims() && lt) {
                // Item i is less than item j on at least one dimension,
                // and there are no dimensions on which item i is greater than item j.
                inSkyline = false;
                break;
            }
        }
        if (inSkyline) {
            skyline.push_back(i);
        }
    }
}
