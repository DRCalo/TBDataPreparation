#ifndef MAPPING_PMT_HPP
#define MAPPING_PMT_HPP

#include <array>
#include <string>

namespace PMTCaloMapping {

constexpr double CELL_WIDTH = 128.0;
constexpr double CELL_HEIGHT = 28.3;
constexpr double COLUMN_OFFSET = 3.0;
constexpr double ROW_OFFSET = 9.5;

constexpr unsigned int MAX_CHANNEL = 126;
constexpr unsigned int NCHANNELS_NONEMPTY = 124;
constexpr unsigned int NIDX = NCHANNELS_NONEMPTY;
constexpr std::array<unsigned int, 4> EMPTY_CHANNELS{{31, 63, 95, 127}};  // non-connected channels

struct HWLoc {
    unsigned int ch;

    constexpr HWLoc (unsigned int ch_) : ch(ch_) { }

    static bool is_valid(unsigned int ch) noexcept {
        for (const unsigned int empty : EMPTY_CHANNELS) {
            if (ch == empty) {
                return false;
            }
        }
        return (ch <= MAX_CHANNEL);
    }
};

struct PhysInfo {
    unsigned int column;
    unsigned int row;
    double x;
    double y;
    char type;
    std::string module_name;
};

namespace details {

constexpr std::array<unsigned int, NCHANNELS_NONEMPTY / 2> cols = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                    // 10x [0..9]
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 16x [10..25]
    3, 3, 3, 3, 3,                                   // 5x [26-30]
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5,                    // 10x [31-40]
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  // 16x [41..56]
    3, 3, 3, 3, 3                                    // 5x [57..61]
};

constexpr unsigned int colFromIdx(unsigned int idx) {
    return cols[idx];
}

constexpr std::array<unsigned int, NCHANNELS_NONEMPTY / 2> rows = {
    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
    1, 2, 3, 4, 5,
    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
    14, 15, 16, 17, 18};

constexpr unsigned int rowFromIdx(unsigned int idx) {
    return rows[idx];
}

constexpr double column2X(unsigned int column) noexcept {
    return (column - COLUMN_OFFSET) * CELL_WIDTH;
}

constexpr double row2Y(unsigned int row) noexcept {
    return (row - ROW_OFFSET) * CELL_HEIGHT;
}

const inline std::string cellName(unsigned int row, unsigned int col) {
    // std::format from c++20 -> constexpr
    return std::to_string(col) + ((row >= 10) ? std::to_string(row) : ("0" + std::to_string(row)));
}

}  // namespace details

/** Convert the index of the array [0-123] to the PMT channel [0-127]
    This function does not perform any range check.
*/
inline HWLoc getHWLocFromIdx(unsigned int idx) noexcept {
    for (const unsigned int empty : EMPTY_CHANNELS) {
        idx += (idx >= empty);
    }
    return {idx};
}

/** Convert the PMT channel [0-127] to the index of the array [0-123]
    This function does not perform any range check.
    If you pass an empty channel (31, 63, 95, 127) the result is undefined.
*/
inline int getIdxFromHWLoc(HWLoc hw) noexcept {
    unsigned int idx = hw.ch;
    for (const unsigned int empty : EMPTY_CHANNELS) {
        idx -= (hw.ch >= empty);
    }
    return idx;
}

[[nodiscard]] inline PhysInfo getPhysInfoFromIdx(const unsigned int idx) {
    const char type = idx < (NCHANNELS_NONEMPTY / 2) ? 'S' : 'C';
    const unsigned int col = details::colFromIdx(idx % (NCHANNELS_NONEMPTY / 2));
    const unsigned int row = details::rowFromIdx(idx % (NCHANNELS_NONEMPTY / 2));
    const double x = details::column2X(col);
    const double y = details::row2Y(row);
    const std::string module_name = details::cellName(row, col);
    return {col, row, x, y, type, module_name};
}

}  // namespace PMTCaloMapping

#endif