#ifndef ATENICA_UINT96_HPP
#define ATENICA_UINT96_HPP

#include <cstdint>
#include <bitset>

using bits96 = std::bitset<96>;

struct uint96 {
    uint64_t first;
    uint32_t second;

    // In that case we do not need a call chain, so nothing need to return
    void operator^=(const uint96 &object);
    friend bool operator==(const uint96 &left, const uint96 &right);

    // Required for hashing, because using a std::bitset in std::unordered_map
    explicit operator std::bitset<96>() const;
};

#endif //ATENICA_UINT96_HPP
