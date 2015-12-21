#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <cstddef>
#include <vector>

using size_type = std::size_t;
using value_type = double;

/** Simple wrapper for dataset storage. */
class Dataset {
public:
    Dataset(size_type size, size_type ndims);
    size_type size() const;
    size_type ndims() const;
    value_type* data();
    const value_type* data() const;
    value_type& operator()(size_type item, size_type dim);
    const value_type& operator()(size_type item, size_type dim) const;
private:
    const size_type size_;
    const size_type ndims_;
    std::vector<value_type> storage_;
};

/** Fill dataset with data from binary file in row-major format. */
void datasetRead(Dataset& dataset, const char* filename);

/** Convert string to dataset count/dimension. */
size_type datasetSizeParse(const char* s);

/** Simple container for skyline indices. */
using Skyline = std::vector<size_type>;

/** Write skyline indices to a file. */
void skylineWrite(const Skyline& skyline, const char* filename);

#endif // COMMON_HPP_
