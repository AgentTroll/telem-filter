#include "csv_reader.h"

csv_reader::csv_reader(const std::string &file_path) {
    csv_file.open(file_path);

    if (!csv_file.good()) {
        csv_file.close();
        throw std::invalid_argument{"Failed to open file."};
    }
}

csv_reader::~csv_reader() {
    csv_file.close();
}

bool csv_reader::read_line() {
    std::string line;
    if (!std::getline(csv_file, line)) {
        return false;
    }

    cells.clear();
    std::stringstream ss{line};

    std::string cell;
    while (std::getline(ss, cell, ',')) {
        cells.push_back(cell);
    }

    return true;
}
