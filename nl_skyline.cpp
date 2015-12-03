#include "nl_skyline.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>

#include "types.hpp"
#include "io.hpp"

static int comparisonCount = 0;

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input output" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1]);
    std::ofstream output(argv[2]);

    Dataset dataset;
    readDataset(input, dataset);

    ItemIndexSeq result;
    auto beforeTime = std::chrono::steady_clock::now();
    nlSkyline(dataset, result);
    auto afterTime = std::chrono::steady_clock::now();

    std::sort(result.begin(), result.end());
    writeSkyline(output, result);

    auto runningTime = std::chrono::duration_cast<std::chrono::milliseconds>(afterTime - beforeTime).count();
    std::cout << runningTime << " " << comparisonCount << std::endl;

    return EXIT_SUCCESS;
}

void nlSkyline(const Dataset& dataset, ItemIndexSeq& result) {
    item_dimension d = dataset[0].size();
    result.clear();
    for (item_index i = 0; i < dataset.size(); i++) {
        bool inSkyline = true;
        // Try to find item j that dominates item i.
        for (item_index j = 0; j < dataset.size(); j++) {
            bool lt = false;
            item_dimension k = 0;
            for (; k < d; k++) {
                if (dataset[i][k] < dataset[j][k]) {
                    // Item i is less than item j on at least one dimension.
                    comparisonCount += 1;
                    lt = true;
                } else if (dataset[i][k] > dataset[j][k]) {
                    // Item i is not dominated by item j.
                    comparisonCount += 2;
                    break;
                }
            }
            if (k == d && lt) {
                // Item i is less than item j on at least one dimension,
                // and there are no dimensions on which item i is greater than item j.
                inSkyline = false;
                break;
            }
        }
        if (inSkyline) {
            result.push_back(i);
        }
    }
}
