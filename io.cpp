#include "io.hpp"

#include <istream>
#include <ostream>
#include <sstream>
#include <string>

void readDataset(std::istream& in, Dataset& dataset) {
    dataset.clear();

    while (!in.eof()) {
        std::string line;
        std::getline(in, line);
        if (line.empty()) {
            continue;
        }

        dataset.push_back(Item());
        std::stringstream colstream(line);
        std::string col;
        while (std::getline(colstream, col, ',')) {
            item_attribute value = string_to_item_attribute(col);
            dataset.back().push_back(value);
        }
    }
}

void writeSkyline(std::ostream& out, const ItemIndexSeq& result) {
    for (item_index index : result) {
        out << index << std::endl;
    }
}
