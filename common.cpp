#include "common.hpp"

#include <cstdio>
#include <string>
#include <fstream>

Dataset::Dataset(size_type size, size_type ndims)
        : size_(size), ndims_(ndims), storage_(size * ndims) {
}

size_type Dataset::size() const {
    return size_;
}

size_type Dataset::ndims() const {
    return ndims_;
}

value_type* Dataset::data() {
    return storage_.data();
}

const value_type* Dataset::data() const {
    return storage_.data();
}

value_type& Dataset::operator()(size_type item, size_type dim) {
    return storage_[ndims_ * item + dim];
}

const value_type& Dataset::operator()(size_type item, size_type dim) const {
    return storage_[ndims_ * item + dim];
}

void datasetRead(Dataset& dataset, const char* filename) {
    FILE* f = std::fopen(filename, "rb");
    std::fread(dataset.data(), sizeof(value_type), dataset.size() * dataset.ndims(), f);
    std::fclose(f);
}

size_type datasetSizeParse(const char* s) {
    return std::stoul(s);
}

void skylineWrite(const Skyline& skyline, const char* filename) {
    std::ofstream f(filename);
    if (skyline.size() > 0) {
        f << skyline.data()[0];
        for (size_type i = 1; i < skyline.size(); i++) {
            f << "," << skyline.data()[i];
        }
        f << std::endl;
    }
    f.close();
}
