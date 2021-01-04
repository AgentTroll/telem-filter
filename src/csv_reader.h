/**
 * @file
 */

#ifndef TELEM_FILTER_CSV_READER_H
#define TELEM_FILTER_CSV_READER_H

#include <fstream>
#include <sstream>
#include <vector>

/**
 * @brief Represents a stream CSV reader.
 */
class csv_reader {
private:
    /**
     * Represents the stream to the CSV file.
     */
    std::ifstream csv_file;
    /**
     * Represents the cells for the currently read row of
     * the CSV file.
     */
    std::vector<std::string> cells;

public:
    /**
     * Creates a new instance of the reader which opens a
     * stream to the CSV file at the given path.
     *
     * @param file_path the path to the CSV file
     * @throws std::invalid_argument if the file cannot be
     * opened
     */
    explicit csv_reader(const std::string &file_path);

    /**
     * Destructor. Will close the stream and clean up and
     * resources consumed by this class.
     */
    ~csv_reader();

    /**
     * Reads the next line of the CSV file into memory.
     *
     * @return true if the line could be read
     */
    bool read_line();

    /**
     * Reads the 0-indexed cell of the current line.
     *
     * The read_line() method must have returned true prior
     * to calling this method.
     *
     * @tparam T the type of the cell to be read
     * @param idx the 0-indexed cell column
     * @return the item from the cell
     */
    template<typename T>
    T read(size_t idx) const;
};

template<typename T>
T csv_reader::read(size_t idx) const {
    std::stringstream ss{cells[idx]};
    T cell;
    ss >> cell;

    return cell;
}

#endif // TELEM_FILTER_CSV_READER_H
