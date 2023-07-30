#include "ZobristHash.hpp"

std::mt19937_64 ZobristHash::gen {seed};
std::uniform_int_distribution<uint64_t> ZobristHash::dist;
uint64_t ZobristHash::CONSTANTS[64][6][2];
uint64_t ZobristHash::BLACK_MOVE;
uint64_t ZobristHash::WHITE_SHORT_CASTLING, ZobristHash::WHITE_LONG_CASTLING;
uint64_t ZobristHash::BLACK_SHORT_CASTLING, ZobristHash::BLACK_LONG_CASTLING;

void ZobristHash::init() {
    for (auto &matrix : CONSTANTS)
        for (auto &array : matrix)
            for (auto &val : array)
                val = dist(gen);

    BLACK_MOVE = dist(gen);
    WHITE_SHORT_CASTLING = dist(gen);
    WHITE_LONG_CASTLING = dist(gen);

    BLACK_SHORT_CASTLING = dist(gen);
    BLACK_LONG_CASTLING = dist(gen);
}


ZobristHash::ZobristHash(const Pieces &pieces, bool black_move,
                         bool ws_castling, bool wl_castling, bool bs_castling, bool bl_castling)
{
    if (black_move)  m_hash ^= BLACK_MOVE;
    if (ws_castling) m_hash ^= WHITE_SHORT_CASTLING;
    if (wl_castling) m_hash ^= WHITE_LONG_CASTLING;
    if (bs_castling) m_hash ^= BLACK_SHORT_CASTLING;
    if (bl_castling) m_hash ^= BLACK_LONG_CASTLING;

    uint8_t side;
    for (uint8_t board = 0; board < 64; ++board) {
        if      (get_bit(pieces.side[BLACK], board)) side = BLACK;
        else if (get_bit(pieces.side[WHITE], board)) side = WHITE;
        else continue;

        for (uint8_t type = 0; type < 6; ++type)
            if (get_bit(pieces.pieces_bitboards[type][side], board)) {
                m_hash ^= CONSTANTS[board][type][side];
                break;
            }
    }
}
bool operator==(const ZobristHash &left, const ZobristHash &right) {
    return left.m_hash == right.m_hash;
}

void ZobristHash::xor_piece(uint8_t board, uint8_t type, uint8_t side) {
    if (board > 63 || type > 5 || side > 1) return;
    m_hash ^= CONSTANTS[board][type][side];
}
void ZobristHash::xor_move()         { m_hash ^= BLACK_MOVE;           }
void ZobristHash::xor_w_s_castling() { m_hash ^= WHITE_SHORT_CASTLING; }
void ZobristHash::xor_w_l_castling() { m_hash ^= WHITE_LONG_CASTLING;  }
void ZobristHash::xor_b_s_castling() { m_hash ^= BLACK_SHORT_CASTLING; }
void ZobristHash::xor_b_l_castling() { m_hash ^= BLACK_LONG_CASTLING;  }
