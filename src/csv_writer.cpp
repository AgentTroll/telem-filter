#include "csv_writer.h"

csv_writer::csv_writer(const std::string &file_path) {
    csv_file.open(file_path);
    if (!csv_file.good()) {
        csv_file.close();
        throw std::invalid_argument{"File could not be opened."};
    }
}

csv_writer::~csv_writer() {
    csv_file.close();
}
