#ifndef IO_HPP_
#define IO_HPP_

/**
 * Utilities for reading and writing data files.
 */

#include <istream>
#include <ostream>

#include "types.hpp"

/**
 * Read dataset from a file; values are in a tab-separated values format (compatible with CSV).
 */
void readDataset(std::istream& in, Dataset& dataset);

/**
 * Write skyline indices to a file; skyline is in tab-separated values format (compatible with CSV).
 */
void writeSkyline(std::ostream& out, const ItemIndexSeq& skyline);

#endif // IO_HPP_
