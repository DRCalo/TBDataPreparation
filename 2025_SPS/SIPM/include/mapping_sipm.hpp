#ifndef MAPPING_SIPM_HPP
#define MAPPING_SIPM_HPP

#include <cmath>
#include <format>
#include <stdexcept>
#include <string>

namespace SiPMCaloMapping {

constexpr double LCELL = 128.0;  // length of a module in mm
constexpr double HCELL = 28.3;   // height of a module in mm

constexpr unsigned int FIBER_NCOLUMNS = 64;      // number of columns of fibers (fibers per row)
constexpr unsigned int FIBER_NROWS = 16;         // number of rows of fibers (fibers per column)
constexpr double FIBER_DIAMETER = 2.0;           // diameter of a fiber in mm
constexpr unsigned int NFIBERS_PER_CHANNEL = 8;  // number of fibers per channel (grouping)

constexpr unsigned int NFIBERS_MODULE = FIBER_NCOLUMNS * FIBER_NROWS;  // number of fibers in a module
constexpr unsigned int NMODULES = 8;                                   // number of modules with SiPM
constexpr unsigned int NFIBERS = NFIBERS_MODULE * NMODULES;            // number of total fibers

constexpr unsigned int CHANNELS_NCOLUMNS = FIBER_NCOLUMNS / NFIBERS_PER_CHANNEL;  // number of columns of channels (channels per row)
constexpr unsigned int CHANNELS_NROWS = FIBER_NROWS;                              // number of rows of channels (channels per column)
constexpr unsigned int NCHANNELS_MODULE = CHANNELS_NCOLUMNS * CHANNELS_NROWS;     // number of channels per module
constexpr unsigned int NCHANNELS_FERS = 64;                                       // number of channels per FERS
constexpr unsigned int NCHANNELS = NCHANNELS_MODULE * NMODULES;                   // number of total channels
constexpr unsigned int NFERS = 16;                                                // number of FERS boards
static_assert(NFERS * NCHANNELS_FERS == NCHANNELS, "Number of FERS times channels / FERS != Number of channels");

constexpr double WIDTH_CHANNEL = FIBER_DIAMETER * NFIBERS_PER_CHANNEL;  // width of a channel
constexpr double HEIGHT_CHANNEL = FIBER_DIAMETER;                       // height of a channel

/// Hardware location of the channel
struct HWLoc {
    unsigned int fersId;  ///< FERS board id (1-16)
    unsigned int ch;      ///< Channel number (0-63)

    constexpr HWLoc(unsigned int fersId_, unsigned int ch_)
        : fersId(fersId_), ch(ch_) {}  // c++11 compatiblity => (constexpr->empty) body => no check

    static constexpr bool is_valid(unsigned int fersId, unsigned int ch) {
        return ((fersId >= 1) and (fersId <= NFERS)) and (ch < NCHANNELS_FERS);
    }
};

/// Physical info of the channel
struct PhysInfo {
    char type;                ///< 'S' or 'C'
    unsigned int row;         ///< row in the module
    unsigned int column;      ///< column in the module
    double x;                 ///< x position
    double y;                 ///< y position
    double x_local;           ///< x position in the module
    double y_local;           ///< y position in the module
    std::string module_name;  ///< module name (3xx)
};

/// Get hardware location from index
[[nodiscard]] constexpr HWLoc getHWLocFromIdx(unsigned int idx) noexcept {
    return {idx / NCHANNELS_FERS + 1, idx % NCHANNELS_FERS};
}

/// Get physical information from index
[[nodiscard]] inline PhysInfo getPhysInfoFromIdx(unsigned int idx) {
    const HWLoc hw = getHWLocFromIdx(idx);
    unsigned int icol = hw.ch / CHANNELS_NROWS;
    unsigned int first_column_ch = hw.ch % CHANNELS_NROWS;
    static constexpr unsigned int row_map[16] = {14, 12, 10, 8, 6, 4, 2, 0, 1, 3, 5, 7, 9, 11, 13, 15};
    const unsigned int irow = row_map[first_column_ch];
    icol += ((hw.fersId - 1) % 2) * (CHANNELS_NCOLUMNS / 2);

    const unsigned int id_module = idx / NCHANNELS_MODULE + 1;

    const double x_offset = (irow % 2) * FIBER_DIAMETER / 2.0 - FIBER_DIAMETER / 4.;
    const char type_fiber = (irow % 2 == 0) ? 'S' : 'C';

    const double y_module = (irow - (FIBER_NROWS - 1) / 2.0) * FIBER_DIAMETER * std::sqrt(3.0) / 2.0;
    const double x_module = (icol * NFIBERS_PER_CHANNEL + (NFIBERS_PER_CHANNEL - 1) / 2.0 - (FIBER_NCOLUMNS - 1) / 2.0) * FIBER_DIAMETER + x_offset;

    const double x = x_module;
    const double y = y_module + HCELL * (id_module - 1) - HCELL * 3.5;

    const unsigned int modnum = id_module + 5;
    std::string module_name = "3";
    if (modnum < 10) module_name += "0";
    module_name += std::to_string(modnum);

    return {type_fiber, irow, icol, x, y, x_module, y_module, module_name};
}

/// Get index from hardware location
[[nodiscard]] constexpr unsigned int getIdxFromHWLoc(const HWLoc& hw) noexcept {
    return (hw.fersId - 1) * NCHANNELS_FERS + hw.ch;
}

}  // namespace SiPMCaloMapping

#endif  // MAPPING_SIPM_HPP