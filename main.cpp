#include <fstream>
#include <iostream>
#include <sstream>

#include "skyline.hpp"

void readDataset(std::istream& in, Dataset& dataset);
void writeSkyline(std::ostream& out, const ItemIndexSeq& skyline);

/**
 * Read dataset from a file; values are in a tab-separated values format (compatible with CSV).
 */
void readDataset(std::istream& in, Dataset& dataset) {
    dataset.clear();

    while (!in.eof()) {
        std::string line;
        std::getline(in, line);
        if (line.empty()) {
            continue;
        }

        dataset.push_back(Item());
        std::stringstream valuestream(line);
        while (!valuestream.eof()) {
            item_attribute value;
            valuestream >> value;
            dataset.back().push_back(value);
        }
    }
}

/**
 * Write skyline indices to a file; skyline is in tab-separated values format (compatible with CSV).
 */
void writeSkyline(std::ostream& out, const ItemIndexSeq& result) {
    for (item_index index : result) {
        out << index << std::endl;
    }
}

int main(int argc, char** argv) {
    bool isNoislessMode = (argc == 3) && (std::string(argv[2]) == "noisless");
    bool isNoisyMode = (argc == 5) && (std::string(argv[2]) == "noisy");
    if (!isNoislessMode && !isNoisyMode) {
        std::cerr << "Usage: " << argv[0] << " datasetfile ('noisless' | 'noisy' error_probability tolerance)" << std::endl;
        return EXIT_FAILURE;
    }

    Dataset dataset;
    std::ifstream in(argv[1]);
    readDataset(in, dataset);
    in.close();

    ItemIndexSeq result;
    if (isNoislessMode) {
        noislessSkyline(dataset, result);
    } else if (isNoisyMode) {
        double errorProbability = std::stod(argv[3]);
        double tolerance = std::stod(argv[4]);
        Oracle oracle(dataset, errorProbability);
        fullSkyline(oracle, tolerance, result);
    }
    writeSkyline(std::cout, result);

    return EXIT_SUCCESS;
}
