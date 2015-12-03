#ifndef IO_HPP_
#define IO_HPP_

/**
 * Utilities for reading and writing data files.
 */

#include <istream>
#include <ostream>

#include "types.hpp"

/**
 * Read dataset from a CSV file.
 */
void readDataset(std::istream& in, Dataset& dataset);

/**
 * Write skyline indices to a CSV file.
 */
void writeSkyline(std::ostream& out, const ItemIndexSeq& skyline);

#endif // IO_HPP_
