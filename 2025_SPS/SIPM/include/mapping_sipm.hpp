#ifndef MAPPING_SIPM_HPP
#define MAPPING_SIPM_HPP

#include <array>
#include <cmath>
#include <string>

namespace SiPMCaloMapping {

using idx_t = unsigned int;

struct HWLoc;
struct PhysLoc;
struct PhysInfo;

// FERS / BOARD definition
constexpr unsigned int NCHANNELS_FERS = 64;  // number of channels per FERS / BOARD

// Module definition
constexpr unsigned int CHANNEL_NROWS_MODULE = 16;     // number of rows of channels (fibers per column)
constexpr unsigned int CHANNELS_NCOLUMNS_MODULE = 8;  // number of columns of channels per module

// Full detector definition
constexpr unsigned int NMODULES = 8;                                     // number of modules with SiPM
constexpr unsigned int CHANNEL_NROWS = CHANNEL_NROWS_MODULE * NMODULES;  // number of rows of channels (channels per column)
constexpr unsigned int CHANNEL_NCOLUMNS = CHANNELS_NCOLUMNS_MODULE;      // number of columns of channels (channels per row)
constexpr unsigned int NFERS = 16;                                       // number of FERS boards

// SiPM definition
constexpr unsigned int NFIBERS_PER_CHANNEL = 8;  // number of fibers per channel (grouping)

// Fiber definition
constexpr double FIBER_DIAMETER = 2.0;  // diameter of a fiber in mm, including absorber

// Module derived quantities
constexpr unsigned int NCHANNELS_MODULE = CHANNEL_NROWS_MODULE * CHANNELS_NCOLUMNS_MODULE;  // number of channels per module

// Full detector derived quantities
constexpr unsigned int NCHANNELS = NCHANNELS_MODULE * NMODULES;  // number of total channels

static_assert(NFERS * NCHANNELS_FERS == NCHANNELS, "Number of FERS times channels / FERS != Number of channels");

constexpr double LCELL = 128.0;  // length of a module in mm
constexpr double HCELL = 28.3;   // height of a module in mm

constexpr double WIDTH_CHANNEL = FIBER_DIAMETER * NFIBERS_PER_CHANNEL;  // width of a channel
constexpr double HEIGHT_CHANNEL = FIBER_DIAMETER;                       // height of a channel

/// Hardware location of the channel
struct HWLoc {
    unsigned int boardID;  ///< boardID (0-15)
    unsigned int ch;       ///< Channel number (0-63)

    constexpr HWLoc(unsigned int boardID_, unsigned int ch_) noexcept
        : boardID(boardID_), ch(ch_) {}  // c++11 compatiblity => (constexpr->empty) body => no check

    static constexpr bool is_valid(unsigned int boardID, unsigned int ch) noexcept {
        return (boardID < NFERS) and (ch < NCHANNELS_FERS);
    }
};

struct PhysLoc {
    unsigned int column;     ///< column in the module
    unsigned int row;        ///< row in the module
    unsigned int id_module;  ///< module id (1-8)

    constexpr bool operator==(const PhysLoc& other) const noexcept {
        return (column == other.column) and (row == other.row) and (id_module == other.id_module);
    }
};

/// Physical info of the channel
struct PhysInfo : public PhysLoc {
    char type;                ///< 'S' or 'C'
    unsigned int fersId;      ///< fersId (1-16)
    double x;                 ///< x position
    double y;                 ///< y position
    double x_local;           ///< x position in the module
    double y_local;           ///< y position in the module
    std::string module_name;  ///< module name (3xx)
};

/// Get hardware location from index
[[nodiscard]] constexpr HWLoc getHWLocFromIdx(idx_t idx) noexcept {
    return {idx / NCHANNELS_FERS, idx % NCHANNELS_FERS};
}

[[nodiscard]] inline unsigned int getFersIdFromBoardID(unsigned int boardID) noexcept {
    constexpr std::array<unsigned int, NFERS> boardID_to_fersID = {2, 1, 4, 3, 6, 5, 8, 7, 10, 9, 15, 16, 13, 14, 11, 12};
    return boardID_to_fersID[boardID];
}

[[nodiscard]] inline unsigned int getBoardIDFromFersId(unsigned int fersID) noexcept {
    constexpr std::array<unsigned int, NFERS> fersID_to_boardID = {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 14, 15, 12, 13, 10, 11};
    return fersID_to_boardID[fersID - 1];
}

inline constexpr std::array<unsigned int, CHANNEL_NROWS_MODULE> nominal_chFirstColumn2row = {14, 12, 10, 8, 6, 4, 2, 0, 1, 3, 5, 7, 9, 11, 13, 15};
inline constexpr std::array<unsigned int, CHANNEL_NROWS_MODULE> nominal_row2chFirstColumn = {7, 8, 6, 9, 5, 10, 4, 11, 3, 12, 2, 13, 1, 14, 0, 15};
inline constexpr std::array<unsigned int, 4> nominal_columnOrder = {0, 1, 2, 3};  // column oreder from one FERS
inline constexpr std::array<unsigned int, 4> module306_left_columnOrder = {1, 0, 3, 2};
inline constexpr std::array<unsigned int, 4> module306_right_columnOrder = {1, 0, 2, 3};
inline constexpr std::array<unsigned int, 4> module309_right_columnOrder = {1, 0, 2, 3};
inline constexpr std::array<std::array<unsigned int, 4>, NFERS> columnOrder = {
    module306_right_columnOrder, module306_left_columnOrder,  // 306
    nominal_columnOrder, nominal_columnOrder,                 // 307
    nominal_columnOrder, nominal_columnOrder,                 // 308
    module309_right_columnOrder, nominal_columnOrder,         // 309
    nominal_columnOrder, nominal_columnOrder,                 // 310
    nominal_columnOrder, nominal_columnOrder,                 // 311
    nominal_columnOrder, nominal_columnOrder,                 // 312
    nominal_columnOrder, nominal_columnOrder                  // 313
};

[[nodiscard]] inline PhysLoc getPhysLocFromIdx(idx_t idx) {
    const HWLoc hw = getHWLocFromIdx(idx);
    const auto fersId = getFersIdFromBoardID(hw.boardID);

    unsigned int first_column_ch = hw.ch % CHANNEL_NROWS_MODULE;                // the channel number of the channels in the first column, same row
    const unsigned int irow = nominal_chFirstColumn2row[first_column_ch];

    const char type_fiber = (irow % 2 == 0) ? 'S' : 'C';
    const auto& columnOrderThis = type_fiber == 'S' ? columnOrder[hw.boardID] : nominal_columnOrder;

    unsigned int icol = columnOrderThis[hw.ch / CHANNEL_NROWS_MODULE];          // this is a number from 0 to 3
    icol += ((fersId - 1) % 2) * (CHANNELS_NCOLUMNS_MODULE / 2);                // adjust for left/right fersId
    

    const unsigned int id_module = (fersId - 1) / 2 + 1;

    return PhysLoc{icol, irow, id_module};
}

/// Get physical information from index
[[nodiscard]] inline PhysInfo getPhysInfoFromIdx(idx_t idx) {
    const PhysLoc phys = getPhysLocFromIdx(idx);
    const HWLoc hw = getHWLocFromIdx(idx);
    const auto fersId = getFersIdFromBoardID(hw.boardID);

    unsigned int icol = phys.column;
    unsigned int irow = phys.row;
    unsigned int id_module = phys.id_module;

    const double x_offset = (irow % 2 == 0 ? -1.0 : 1.0) * FIBER_DIAMETER / 4.0;

    const char type_fiber = (irow % 2 == 0) ? 'S' : 'C';

    const double y_module = (irow - (CHANNEL_NROWS_MODULE - 1) / 2.0) * FIBER_DIAMETER * std::sqrt(3.0) / 2.0;
    const double x_module = (icol * NFIBERS_PER_CHANNEL + (NFIBERS_PER_CHANNEL - 1) / 2.0 - (CHANNEL_NCOLUMNS * NFIBERS_PER_CHANNEL - 1) / 2.0) * FIBER_DIAMETER + x_offset;

    const double x = x_module;
    const double y = y_module + HCELL * (id_module - 1) - HCELL * 3.5;

    const unsigned int modnum = id_module + 5;
    std::string module_name = "3";
    if (modnum < 10) module_name += "0";
    module_name += std::to_string(modnum);

    return PhysInfo{icol, irow, id_module, type_fiber, fersId, x, y, x_module, y_module, module_name};
}

/// Get index from hardware location
[[nodiscard]] constexpr unsigned int getIdxFromHWLoc(const HWLoc& hw) noexcept {
    return hw.boardID * NCHANNELS_FERS + hw.ch;
}

}  // namespace SiPMCaloMapping

#endif  // MAPPING_SIPM_HPP
