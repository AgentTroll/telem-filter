/**
 * @file
 */

#ifndef TELEM_FILTER_CSV_WRITER_H
#define TELEM_FILTER_CSV_WRITER_H

#include <fstream>

/**
 * @brief This class represents a streaming CSV writer.
 */
class csv_writer {
private:
    /**
     * The file output stream to the target CSV file this
     * class writes.
     */
    std::ofstream csv_file;

public:
    /**
     * Creates a new CSV file writer which outputs to the
     * file at the given path.
     *
     * @param file_path the path to the CSV file which to
     * write
     * @throws std::invalid_argument if the path to the
     * file is not valid
     */
    explicit csv_writer(const std::string &file_path);

    /**
     * Destructor. Will clean up and flush any unwritten
     * CSV lines to the output file.
     */
    ~csv_writer();

    /**
     * Writes the given item to the CSV file.
     *
     * @tparam T the item type
     * @param item the item to write
     * @return the reference to this CSV writer object
     */
    template<typename T>
    csv_writer &operator<<(const T &item);
};

template<typename T>
csv_writer &csv_writer::operator<<(const T &item) {
    csv_file << item;
    return *this;
}

#endif // TELEM_FILTER_CSV_WRITER_H
