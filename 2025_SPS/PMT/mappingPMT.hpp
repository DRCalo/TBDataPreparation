#ifndef MAPPING_PMT_HPP
#define MAPPING_PMT_HPP

#include <array>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>

namespace PMTCaloMapping {

// mapping constants
constexpr unsigned int NCHANNEL = 192;
constexpr unsigned int NPMT = 124;

// geometrical constants
constexpr double CELL_WIDTH = 128.0;
constexpr double CELL_HEIGHT = 28.3;
constexpr double COLUMN_OFFSET = 3.0;
constexpr double ROW_OFFSET = 9.5;

using idx_t = unsigned int;

struct HWLoc;
struct PhysLoc;
struct PhysInfo;

[[nodiscard]] inline int getIdxFromHWLoc(const HWLoc& loc) noexcept;
[[nodiscard]] inline HWLoc getHWLocFromIdx(idx_t idx, unsigned int run) noexcept;
[[nodiscard]] inline int getIdxFromHWLoc(HWLoc hw, unsigned int run) noexcept;
[[nodiscard]] inline PhysInfo getPhysInfoFromIdx(const idx_t idx);
[[nodiscard]] inline PhysInfo getPhysInfoFromHWLoc(HWLoc hw, unsigned int run);
[[nodiscard]] inline HWLoc getHWLocFromPhysLoc(const PhysLoc& phys, unsigned int run);
[[nodiscard]] inline idx_t getIdxFromPhysLoc(const PhysLoc& phys);


struct HWLoc {
    unsigned int ch;

    constexpr HWLoc(unsigned int ch_) noexcept : ch(ch_) {}

    [[nodiscard]] static bool is_valid(unsigned int ch) noexcept {
        return (ch < NCHANNEL);
    }

    idx_t toIdx() const noexcept {
        return getIdxFromHWLoc(*this);
    }

    static HWLoc fromIdx(idx_t idx, unsigned int run) noexcept {
        return getHWLocFromIdx(idx, run);
    }
};

struct PhysLoc {
    unsigned int column;
    unsigned int row;
    char type;  // 'S' or 'C'

    constexpr bool operator==(const PhysLoc& other) const noexcept {
        return column == other.column && row == other.row && type == other.type;
    }

    constexpr bool is_valid() const noexcept {
        const bool valid_column = (column >= 1) && (column <= 5);
        if (!valid_column) {
            return false;
        }
        const bool valid_row = (((row == 1) && (row >= 1) && (row <= 5)) ||
                                ((row == 2) && (row >= 2) && (row <= 17)) ||
                                ((row == 3) && (row >= 1) && (row <= 18)) ||
                                ((row == 4) && (row >= 2) && (row <= 17)) ||
                                ((row == 5) && (row >= 5) && (row <= 14)));
        if (!valid_row) {
            return false;
        }
        const bool valid_type = (type == 'S' || type == 'C');
        return valid_type;
    }

    constexpr bool is_pmt() const noexcept {
        if (!is_valid()) {
            return false;
        }
        return !((column == 3) && ((row >= 6) && (row <= 13)));
    }

    HWLoc toHWLoc(unsigned int run) noexcept {
        return getHWLocFromPhysLoc(*this, run);
    }

};

struct PhysInfo : public PhysLoc {
    double x;
    double y;
    std::string module_name;
};

namespace details {

// this is the table in the twiki
constexpr std::array<std::optional<PhysLoc>, NCHANNEL> channel_map_original = {{
    // ADC0
    PhysLoc{1, 5, 'S'},   // ch 0
    PhysLoc{1, 6, 'S'},   // ch 1
    PhysLoc{1, 7, 'S'},   // ch 2
    PhysLoc{1, 8, 'S'},   // ch 3
    PhysLoc{1, 9, 'S'},   // ch 4
    PhysLoc{1, 10, 'S'},  // ch 5
    PhysLoc{1, 11, 'S'},  // ch 6
    PhysLoc{1, 12, 'S'},  // ch 7
    PhysLoc{1, 13, 'S'},  // ch 8
    PhysLoc{1, 14, 'S'},  // ch 9
    PhysLoc{2, 2, 'S'},   // ch 10
    PhysLoc{2, 3, 'S'},   // ch 11
    PhysLoc{2, 4, 'S'},   // ch 12
    PhysLoc{2, 5, 'S'},   // ch 13
    PhysLoc{2, 6, 'S'},   // ch 14
    PhysLoc{2, 7, 'S'},   // ch 15
    PhysLoc{2, 8, 'S'},   // ch 16
    PhysLoc{2, 9, 'S'},   // ch 17
    PhysLoc{2, 10, 'S'},  // ch 18
    PhysLoc{2, 11, 'S'},  // ch 19
    PhysLoc{2, 12, 'S'},  // ch 20
    PhysLoc{2, 13, 'S'},  // ch 21
    PhysLoc{2, 14, 'S'},  // ch 22
    PhysLoc{2, 15, 'S'},  // ch 23
    PhysLoc{2, 16, 'S'},  // ch 24
    PhysLoc{2, 17, 'S'},  // ch 25
    PhysLoc{3, 1, 'S'},   // ch 26
    PhysLoc{3, 2, 'S'},   // ch 27
    PhysLoc{3, 3, 'S'},   // ch 28
    PhysLoc{3, 4, 'S'},   // ch 29
    PhysLoc{3, 5, 'S'},   // ch 30
    std::nullopt,         // ch 31 (PS)
    // ADC1
    PhysLoc{5, 5, 'S'},   // ch 32
    PhysLoc{5, 6, 'S'},   // ch 33
    PhysLoc{5, 7, 'S'},   // ch 34
    PhysLoc{5, 8, 'S'},   // ch 35
    PhysLoc{5, 9, 'S'},   // ch 36
    PhysLoc{5, 10, 'S'},  // ch 37
    PhysLoc{5, 11, 'S'},  // ch 38
    PhysLoc{5, 12, 'S'},  // ch 39
    PhysLoc{5, 13, 'S'},  // ch 40
    PhysLoc{5, 14, 'S'},  // ch 41
    PhysLoc{4, 2, 'S'},   // ch 42
    PhysLoc{4, 3, 'S'},   // ch 43
    PhysLoc{4, 4, 'S'},   // ch 44
    PhysLoc{4, 5, 'S'},   // ch 45
    PhysLoc{4, 6, 'S'},   // ch 46
    PhysLoc{4, 7, 'S'},   // ch 47
    PhysLoc{4, 8, 'S'},   // ch 48
    PhysLoc{4, 9, 'S'},   // ch 49
    PhysLoc{4, 10, 'S'},  // ch 50
    PhysLoc{4, 11, 'S'},  // ch 51
    PhysLoc{4, 12, 'S'},  // ch 52
    PhysLoc{4, 13, 'S'},  // ch 53
    PhysLoc{4, 14, 'S'},  // ch 54
    PhysLoc{4, 15, 'S'},  // ch 55
    PhysLoc{4, 16, 'S'},  // ch 56
    PhysLoc{4, 17, 'S'},  // ch 57
    PhysLoc{3, 14, 'S'},  // ch 58
    PhysLoc{3, 15, 'S'},  // ch 59
    PhysLoc{3, 16, 'S'},  // ch 60
    PhysLoc{3, 17, 'S'},  // ch 61
    PhysLoc{3, 18, 'S'},  // ch 62
    std::nullopt,         // ch 63 (Veto)
    // ADC2
    PhysLoc{1, 5, 'C'},   // ch 64
    PhysLoc{1, 6, 'C'},   // ch 65
    PhysLoc{1, 7, 'C'},   // ch 66
    PhysLoc{1, 8, 'C'},   // ch 67
    PhysLoc{1, 9, 'C'},   // ch 68
    PhysLoc{1, 10, 'C'},  // ch 69
    PhysLoc{1, 11, 'C'},  // ch 70
    PhysLoc{1, 12, 'C'},  // ch 71
    PhysLoc{1, 13, 'C'},  // ch 72
    PhysLoc{1, 14, 'C'},  // ch 73
    PhysLoc{2, 2, 'C'},   // ch 74
    PhysLoc{2, 3, 'C'},   // ch 75
    PhysLoc{2, 4, 'C'},   // ch 76
    PhysLoc{2, 5, 'C'},   // ch 77
    PhysLoc{2, 6, 'C'},   // ch 78
    PhysLoc{2, 7, 'C'},   // ch 79
    PhysLoc{2, 8, 'C'},   // ch 80
    PhysLoc{2, 9, 'C'},   // ch 81
    PhysLoc{2, 10, 'C'},  // ch 82
    PhysLoc{2, 11, 'C'},  // ch 83
    PhysLoc{2, 12, 'C'},  // ch 84
    PhysLoc{2, 13, 'C'},  // ch 85
    PhysLoc{2, 14, 'C'},  // ch 86
    PhysLoc{2, 15, 'C'},  // ch 87
    PhysLoc{2, 16, 'C'},  // ch 88
    PhysLoc{2, 17, 'C'},  // ch 89
    PhysLoc{3, 1, 'C'},   // ch 90
    PhysLoc{3, 2, 'C'},   // ch 91
    PhysLoc{3, 3, 'C'},   // ch 92
    PhysLoc{3, 4, 'C'},   // ch 93
    PhysLoc{3, 5, 'C'},   // ch 94
    std::nullopt,         // ch 95
    // ADC3
    PhysLoc{5, 5, 'C'},   // ch 96
    PhysLoc{5, 6, 'C'},   // ch 97
    PhysLoc{5, 7, 'C'},   // ch 98
    PhysLoc{5, 8, 'C'},   // ch 99
    PhysLoc{5, 9, 'C'},   // ch 100
    PhysLoc{5, 10, 'C'},  // ch 101
    PhysLoc{5, 11, 'C'},  // ch 102
    PhysLoc{5, 12, 'C'},  // ch 103
    PhysLoc{5, 13, 'C'},  // ch 104
    PhysLoc{5, 14, 'C'},  // ch 105
    PhysLoc{4, 2, 'C'},   // ch 106
    PhysLoc{4, 3, 'C'},   // ch 107
    PhysLoc{4, 4, 'C'},   // ch 108
    PhysLoc{4, 5, 'C'},   // ch 109
    PhysLoc{4, 6, 'C'},   // ch 110
    PhysLoc{4, 7, 'C'},   // ch 111
    PhysLoc{4, 8, 'C'},   // ch 112
    PhysLoc{4, 9, 'C'},   // ch 113
    PhysLoc{4, 10, 'C'},  // ch 114
    PhysLoc{4, 11, 'C'},  // ch 115
    PhysLoc{4, 12, 'C'},  // ch 116
    PhysLoc{4, 13, 'C'},  // ch 117
    PhysLoc{4, 14, 'C'},  // ch 118
    PhysLoc{4, 15, 'C'},  // ch 119
    PhysLoc{4, 16, 'C'},  // ch 120
    PhysLoc{4, 17, 'C'},  // ch 121
    PhysLoc{3, 14, 'C'},  // ch 122
    PhysLoc{3, 15, 'C'},  // ch 123
    PhysLoc{3, 16, 'C'},  // ch 124
    PhysLoc{3, 17, 'C'},  // ch 125
    PhysLoc{3, 18, 'C'},  // ch 126
    PhysLoc{3, 14, 'C'},  // ch 127
    // ADC4
    std::nullopt,  // ch 128 (L2)
    std::nullopt,  // ch 129 (L3)
    std::nullopt,  // ch 130 (L4)
    std::nullopt,  // ch 131 (L5)
    std::nullopt,  // ch 132 (L7)
    std::nullopt,  // ch 133 (L8)
    std::nullopt,  // ch 134 (L9)
    std::nullopt,  // ch 135 (L10)
    std::nullopt,  // ch 136 (L8)
    std::nullopt,  // ch 137
    std::nullopt,  // ch 138
    std::nullopt,  // ch 139
    std::nullopt,  // ch 140
    std::nullopt,  // ch 141
    std::nullopt,  // ch 142
    std::nullopt,  // ch 143
    std::nullopt,  // ch 144
    std::nullopt,  // ch 145
    std::nullopt,  // ch 146
    std::nullopt,  // ch 147
    std::nullopt,  // ch 148
    std::nullopt,  // ch 149
    std::nullopt,  // ch 150
    std::nullopt,  // ch 151
    std::nullopt,  // ch 152
    std::nullopt,  // ch 153
    std::nullopt,  // ch 154
    std::nullopt,  // ch 155
    std::nullopt,  // ch 156
    std::nullopt,  // ch 157
    std::nullopt,  // ch 158
    std::nullopt,  // ch 159
    // ADC5
    std::nullopt,  // ch 160
    std::nullopt,  // ch 161
    std::nullopt,  // ch 162
    std::nullopt,  // ch 163
    std::nullopt,  // ch 164
    std::nullopt,  // ch 165
    std::nullopt,  // ch 166
    std::nullopt,  // ch 167
    std::nullopt,  // ch 168
    std::nullopt,  // ch 169
    std::nullopt,  // ch 170
    std::nullopt,  // ch 171
    std::nullopt,  // ch 172
    std::nullopt,  // ch 173
    std::nullopt,  // ch 174
    std::nullopt,  // ch 175
    std::nullopt,  // ch 176
    std::nullopt,  // ch 177
    std::nullopt,  // ch 178
    std::nullopt,  // ch 179
    std::nullopt,  // ch 180
    std::nullopt,  // ch 181
    std::nullopt,  // ch 182
    std::nullopt,  // ch 183
    std::nullopt,  // ch 184
    std::nullopt,  // ch 185
    std::nullopt,  // ch 186
    std::nullopt,  // ch 187
    std::nullopt,  // ch 188
    std::nullopt,  // ch 189
    std::nullopt,  // ch 190
    std::nullopt   // ch 191
}};

// helper to create modified constexpr arrays
template <typename T, std::size_t N>
constexpr std::array<T, N> modified(std::array<T, N> base, std::initializer_list<std::pair<std::size_t, T>> changes) {
    for (auto [i, v] : changes)
        base[i] = v;
    return base;
}

// up to run 523
constexpr auto channel_map_from524 = modified(channel_map_original, {{32, std::nullopt},
                                                                     {122, std::nullopt}});

// from run 537
constexpr auto channel_map_from537 = modified(channel_map_from524, {{95, PhysLoc{5, 5, 'S'}},
                                                                    {127, PhysLoc{3, 14, 'C'}}});

[[nodiscard]] inline const auto& get_channel_map(unsigned int run) noexcept {
    if (run >= 537) {
        return channel_map_from537;
    } else if (run >= 524) {
        return channel_map_from524;
    }
    return channel_map_original;
}

// this is the desired ordering of the cells
constexpr std::array<PhysLoc, NPMT> cell_ordering = {{{1, 5, 'S'}, {1, 5, 'C'},  // 0, 1
                                                      {1, 6, 'S'},
                                                      {1, 6, 'C'},
                                                      {1, 7, 'S'},
                                                      {1, 7, 'C'},
                                                      {1, 8, 'S'},
                                                      {1, 8, 'C'},
                                                      {1, 9, 'S'},
                                                      {1, 9, 'C'},
                                                      {1, 10, 'S'},
                                                      {1, 10, 'C'},  // 10, 11
                                                      {1, 11, 'S'},
                                                      {1, 11, 'C'},
                                                      {1, 12, 'S'},
                                                      {1, 12, 'C'},
                                                      {1, 13, 'S'},
                                                      {1, 13, 'C'},
                                                      {1, 14, 'S'},
                                                      {1, 14, 'C'},

                                                      {2, 2, 'S'},
                                                      {2, 2, 'C'},  // 20, 21
                                                      {2, 3, 'S'},
                                                      {2, 3, 'C'},
                                                      {2, 4, 'S'},
                                                      {2, 4, 'C'},
                                                      {2, 5, 'S'},
                                                      {2, 5, 'C'},
                                                      {2, 6, 'S'},
                                                      {2, 6, 'C'},
                                                      {2, 7, 'S'},
                                                      {2, 7, 'C'},  // 30, 31
                                                      {2, 8, 'S'},
                                                      {2, 8, 'C'},
                                                      {2, 9, 'S'},
                                                      {2, 9, 'C'},
                                                      {2, 10, 'S'},
                                                      {2, 10, 'C'},
                                                      {2, 11, 'S'},
                                                      {2, 11, 'C'},
                                                      {2, 12, 'S'},
                                                      {2, 12, 'C'},  // 40, 41
                                                      {2, 13, 'S'},
                                                      {2, 13, 'C'},
                                                      {2, 14, 'S'},
                                                      {2, 14, 'C'},
                                                      {2, 15, 'S'},
                                                      {2, 15, 'C'},
                                                      {2, 16, 'S'},
                                                      {2, 16, 'C'},
                                                      {2, 17, 'S'},
                                                      {2, 17, 'C'},  // 50, 51

                                                      {3, 1, 'S'},
                                                      {3, 1, 'C'},
                                                      {3, 2, 'S'},
                                                      {3, 2, 'C'},
                                                      {3, 3, 'S'},
                                                      {3, 3, 'C'},
                                                      {3, 4, 'S'},
                                                      {3, 4, 'C'},
                                                      {3, 5, 'S'},
                                                      {3, 5, 'C'},  // 60, 61
                                                      {3, 14, 'S'},
                                                      {3, 14, 'C'},
                                                      {3, 15, 'S'},
                                                      {3, 15, 'C'},
                                                      {3, 16, 'S'},
                                                      {3, 16, 'C'},
                                                      {3, 17, 'S'},
                                                      {3, 17, 'C'},
                                                      {3, 18, 'S'},
                                                      {3, 18, 'C'},  // 70, 71

                                                      {4, 2, 'S'},
                                                      {4, 2, 'C'},
                                                      {4, 3, 'S'},
                                                      {4, 3, 'C'},
                                                      {4, 4, 'S'},
                                                      {4, 4, 'C'},
                                                      {4, 5, 'S'},
                                                      {4, 5, 'C'},
                                                      {4, 6, 'S'},
                                                      {4, 6, 'C'},  // 80, 81
                                                      {4, 7, 'S'},
                                                      {4, 7, 'C'},
                                                      {4, 8, 'S'},
                                                      {4, 8, 'C'},
                                                      {4, 9, 'S'},
                                                      {4, 9, 'C'},
                                                      {4, 10, 'S'},
                                                      {4, 10, 'C'},
                                                      {4, 11, 'S'},
                                                      {4, 11, 'C'},  // 90, 91
                                                      {4, 12, 'S'},
                                                      {4, 12, 'C'},
                                                      {4, 13, 'S'},
                                                      {4, 13, 'C'},
                                                      {4, 14, 'S'},
                                                      {4, 14, 'C'},
                                                      {4, 15, 'S'},
                                                      {4, 15, 'C'},
                                                      {4, 16, 'S'},
                                                      {4, 16, 'C'},  // 100, 101
                                                      {4, 17, 'S'},
                                                      {4, 17, 'C'},

                                                      {5, 5, 'S'},
                                                      {5, 5, 'C'},
                                                      {5, 6, 'S'},
                                                      {5, 6, 'C'},
                                                      {5, 7, 'S'},
                                                      {5, 7, 'C'},
                                                      {5, 8, 'S'},
                                                      {5, 8, 'C'},  // 110, 111
                                                      {5, 9, 'S'},
                                                      {5, 9, 'C'},
                                                      {5, 10, 'S'},
                                                      {5, 10, 'C'},
                                                      {5, 11, 'S'},
                                                      {5, 11, 'C'},
                                                      {5, 12, 'S'},
                                                      {5, 12, 'C'},
                                                      {5, 13, 'S'},
                                                      {5, 13, 'C'},  // 120, 121
                                                      {5, 14, 'S'},
                                                      {5, 14, 'C'}}};

/**
 * @brief Creates an index mapping from a target array to an original array of optionals.
 *
 * This function constructs a mapping that shows where each element in the target_ordered
 * array can be found in the original array. For each element in target_ordered, it searches
 * through the original array to find a matching value and records its index.
 *
 * @tparam T The type of elements stored in both arrays
 * @tparam N The size of the original array
 * @tparam M The size of the target_ordered array
 * @param invalid_index Value stored when a target element cannot be matched. Defaults to N.
 * @return A size-M array where element j gives the index in @p original that stores
 *         @p target_ordered[j], or @p invalid_index when the target value is absent.
 * Example:
 * original = [A, B, C, D, E]
 * target_ordered = [B, D, F]
 * result = [1, 3, 5]  // 5 is invalid index
 */
template <typename T, std::size_t N, std::size_t M>
std::array<std::size_t, M> make_index_map(
    const std::array<std::optional<T>, N>& original,
    const std::array<T, M>& target_ordered,
    std::size_t invalid_index = N) {
    std::array<std::size_t, M> indices{};
    indices.fill(invalid_index);

    // N x M complexity, can be improved with hash map to N + M
    for (std::size_t j = 0; j < M; ++j) {
        const auto& target = target_ordered[j];

        for (std::size_t i = 0; i < N; ++i) {
            if (original[i] && *original[i] == target) {
                indices[j] = i;
                break;
            }
        }
    }

    return indices;
}

const auto map_hw_to_index_original = make_index_map(channel_map_original, cell_ordering);
const auto map_hw_to_index_from524 = make_index_map(channel_map_from524, cell_ordering);
const auto map_hw_to_index_from537 = make_index_map(channel_map_from537, cell_ordering);

template <std::size_t N, std::size_t M>
constexpr std::array<std::size_t, M> make_inverse_map(
    const std::array<std::size_t, N>& index_map,
    std::size_t invalid_index = M) {
    std::array<std::size_t, M> inv{};
    inv.fill(invalid_index);

    for (std::size_t i = 0; i < N; ++i) {
        const std::size_t j = index_map[i];
        if (j != invalid_index and j < M) {
            inv[j] = i;
        }
    }
    return inv;
}

const auto map_hw_to_index_original_inv = make_inverse_map<NPMT, NCHANNEL>(map_hw_to_index_original);
const auto map_hw_to_index_from524_inv = make_inverse_map<NPMT, NCHANNEL>(map_hw_to_index_from524);
const auto map_hw_to_index_from537_inv = make_inverse_map<NPMT, NCHANNEL>(map_hw_to_index_from537);

[[nodiscard]] inline const auto& get_map_hw_to_index(unsigned int run) noexcept {
    if (run >= 537) {
        return map_hw_to_index_from537;
    } else if (run >= 524) {
        return map_hw_to_index_from524;
    }
    return map_hw_to_index_original;
}

[[nodiscard]] inline const auto& get_map_hw_to_index_inv(unsigned int run) noexcept {
    if (run >= 537) {
        return map_hw_to_index_from537_inv;
    } else if (run >= 524) {
        return map_hw_to_index_from524_inv;
    }
    return map_hw_to_index_original_inv;
}

constexpr unsigned int rowFromIdx(idx_t idx) {
    return cell_ordering[idx].row;
}

constexpr unsigned int colFromIdx(idx_t idx) {
    return cell_ordering[idx].column;
}

constexpr double column2X(unsigned int column) noexcept {
    return (column - COLUMN_OFFSET) * CELL_WIDTH;
}

constexpr double row2Y(unsigned int row) noexcept {
    return (row - ROW_OFFSET) * CELL_HEIGHT;
}

[[nodiscard]] inline std::string cellName(unsigned int row, unsigned int col) {
    // std::format from c++20 -> constexpr
    return std::to_string(col) + ((row >= 10) ? std::to_string(row) : ("0" + std::to_string(row)));
}

}  // namespace details

/** Convert the index of the array [0-123] to the PMT channel [0-127]
    This function does not perform any range check.
*/
[[nodiscard]] inline HWLoc getHWLocFromIdx(idx_t idx, unsigned int run) noexcept {
    const unsigned int hw = details::get_map_hw_to_index(run)[idx];
    return HWLoc(hw);
}

/** Convert the PMT channel [0-127] to the index of the array [0-123]
    This function does not perform any range check.
    If you pass an empty channel (31, 63, 95, 127) the result is undefined.
*/
[[nodiscard]] inline int getIdxFromHWLoc(HWLoc hw, unsigned int run) noexcept {
    return details::get_map_hw_to_index_inv(run)[hw.ch];
}

[[nodiscard]] inline PhysInfo getPhysInfoFromIdx(const idx_t idx) {
    const PhysLoc cell = details::cell_ordering[idx];
    return {cell.column, cell.row, cell.type, details::column2X(cell.column), details::row2Y(cell.row), details::cellName(cell.row, cell.column)};
}

[[nodiscard]] inline PhysInfo getPhysInfoFromHWLoc(HWLoc hw, unsigned int run) {
    const auto maybe_cell = details::get_channel_map(run)[hw.ch];
    if (not maybe_cell) {
        throw std::out_of_range("Channel " + std::to_string(hw.ch) + " is not mapped to any cell");
    }
    const PhysLoc cell = *maybe_cell;
    return {cell.column, cell.row, cell.type, details::column2X(cell.column), details::row2Y(cell.row), details::cellName(cell.row, cell.column)};
}

[[nodiscard]] inline HWLoc getHWLocFromPhysLoc(const PhysLoc& phys, unsigned int run) {
    const auto& channel_map = details::get_channel_map(run);
    for (std::size_t ch = 0; ch < channel_map.size(); ++ch) {
        if (channel_map[ch] && *channel_map[ch] == phys) {
            return HWLoc(static_cast<unsigned int>(ch));
        }
    }
    throw std::out_of_range("Physical location not mapped to any channel");
}

[[nodiscard]] inline idx_t getIdxFromPhysLoc(const PhysLoc& phys) {
    for (std::size_t idx = 0; idx < details::cell_ordering.size(); ++idx) {
        if (details::cell_ordering[idx] == phys) {
            return static_cast<int>(idx);
        }
    }
    throw std::out_of_range("Physical location not mapped to any index");
}

}  // namespace PMTCaloMapping

#endif