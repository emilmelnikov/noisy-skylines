#include "noisless_skyline.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>

#include "types.hpp"
#include "io.hpp"

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
    noislessSkyline(dataset, result);
    auto afterTime = std::chrono::steady_clock::now();

    std::sort(result.begin(), result.end());
    writeSkyline(output, result);

    auto runningTime = std::chrono::duration_cast<std::chrono::milliseconds>(afterTime - beforeTime).count();
    std::cout << runningTime << std::endl;

    return EXIT_SUCCESS;
}

item_index maxLex(const Dataset& dataset, const ItemIndexSet& items) {
    item_dimension d = dataset[0].size();
    item_index maximum = 0;

    for (item_index item : items) {
        for (item_dimension k = 0; k < d; k++) {
            if (dataset[item][k] < dataset[maximum][k]) {
                break;
            } else if (dataset[item][k] > dataset[maximum][k]) {
                maximum = item;
                break;
            }
        }
    }
    return maximum;
}

void removeDominated(item_index maximum, const Dataset& dataset, ItemIndexSet& items) {
    item_dimension d = dataset[0].size();

    auto it = items.begin();
    while (it != items.end()) {
        bool lt = false;
        item_dimension k = 0;
        for (; k < d; k++) {
            if (dataset[*it][k] < dataset[maximum][k]) {
                // Item i is less than item j on at least one dimension.
                lt = true;
            } else if (dataset[*it][k] > dataset[maximum][k]) {
                // Item i is not dominated by item j.
                break;
            }
        }

        if (k == d && lt) {
            // Item i is less than item j on at least one dimension,
            // and there are no dimensions on which item i is greater than item j.
            it = items.erase(it);
        } else {
            it++;
        }
    }
}

void noislessSkyline(const Dataset& dataset, ItemIndexSeq& result) {
    result.clear();

    ItemIndexSet notDominatedItems;
    for (item_index i = 0; i < dataset.size(); i++) {
        notDominatedItems.insert(i);
    }

    while (!notDominatedItems.empty()) {
        item_index maximum = maxLex(dataset, notDominatedItems);
        notDominatedItems.erase(maximum);
        removeDominated(maximum, dataset, notDominatedItems);
        result.push_back(maximum);
    }
}
