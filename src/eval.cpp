#include "eval.hpp"

bitboard Eval::hidden::_wp_passed_mask[64];
bitboard Eval::hidden::_bp_passed_mask[64];

void Eval::init() {
    for (uint8_t i = 0; i < 8; ++i) {
        hidden::_wp_passed_mask[i] = (hidden::COL[i] >> 1) & ~FILE_H |
                                      hidden::COL[i] |
                                     (hidden::COL[i] << 1) & ~FILE_A;
        hidden::_bp_passed_mask[56 + i] = hidden::_wp_passed_mask[i];

        hidden::_wp_passed_mask[i] &= ~RANK_1;
        hidden::_bp_passed_mask[56 + i] &= ~RANK_8;
    }

    for (uint8_t i = 8; i < 64; ++i) {
        hidden::_wp_passed_mask[i] = hidden::_wp_passed_mask[i-8] << 8;
        hidden::_bp_passed_mask[63-i] = hidden::_bp_passed_mask[63-i+8] >> 8;
    }
}

int32_t Eval::evaluate(const Board &board) {
    int32_t score = 0;
    int32_t mat_white = 0;
    int32_t mat_black = 0;

    const bitboard bP = board.get_pieces(BLACK, PAWN); // for white passed (opposite color)
    const bitboard wP = board.get_pieces(WHITE, PAWN); // for black passed (here too)
    const bitboard all_pawns = bP | wP; // for open and semi open files for rooks and queens

    // Pawns
    bitboard pieces = board.get_pieces(WHITE, PAWN);
    while (pieces) {
        const uint8_t sq = pop_lsb(pieces);
        score += hidden::PST[PAWN][hidden::FLIP[sq]];
        mat_white += hidden::MATERIAL_BONUS[PAWN];

        score += (pieces & hidden::ISOLATED_PAWNS_MASK[get_file(sq)]) ? 0 : hidden::ISOLATED_PAWN;
        score += (bP & hidden::_wp_passed_mask[sq] ? 0 : hidden::PASSED_PAWNS[get_rank(sq)]);
    }

    pieces = board.get_pieces(BLACK, PAWN);
    while (pieces) {
        const uint8_t sq = pop_lsb(pieces);
        score -= hidden::PST[PAWN][sq];
        mat_black += hidden::MATERIAL_BONUS[PAWN];

        score -= (pieces & hidden::ISOLATED_PAWNS_MASK[get_file(sq)]) ? 0 : hidden::ISOLATED_PAWN;
        score -= (wP & hidden::_bp_passed_mask[sq] ? 0 : hidden::PASSED_PAWNS[7 - get_rank(sq)]);
    }

    // Knights
    pieces = board.get_pieces(WHITE, KNIGHT);
    while (pieces) {
        score += hidden::PST[KNIGHT][hidden::FLIP[pop_lsb(pieces)]];
        mat_white += hidden::MATERIAL_BONUS[KNIGHT];
    }

    pieces = board.get_pieces(BLACK, KNIGHT);
    while (pieces) {
        score -= hidden::PST[KNIGHT][pop_lsb(pieces)];
        mat_black += hidden::MATERIAL_BONUS[KNIGHT];
    }

    // Bishops
    pieces = board.get_pieces(WHITE, BISHOP);
    score += ((pieces & WHITE_SQUARES) && (pieces & BLACK_SQUARES)) ? hidden::BISHOP_PAIR : 0;

    while (pieces) {
        score += hidden::PST[BISHOP][hidden::FLIP[pop_lsb(pieces)]];
        mat_white += hidden::MATERIAL_BONUS[BISHOP];
    }

    pieces = board.get_pieces(BLACK, BISHOP);
    score -= ((pieces & WHITE_SQUARES) && (pieces & BLACK_SQUARES)) ? hidden::BISHOP_PAIR : 0;

    while (pieces) {
        score -= hidden::PST[BISHOP][pop_lsb(pieces)];
        mat_black += hidden::MATERIAL_BONUS[BISHOP];
    }

    // Rooks
    pieces = board.get_pieces(WHITE, ROOK);
    while (pieces) {
        const uint8_t sq = hidden::FLIP[pop_lsb(pieces)];
        score += hidden::PST[ROOK][sq];
        mat_white += hidden::MATERIAL_BONUS[ROOK];

        const bitboard file = hidden::COL[get_file(sq)];
        if (!(all_pawns & file))
            score += hidden::ROOK_OPEN_FILE;
        else if (!(wP & file))
            score += hidden::ROOK_SEMI_OPEN_FILE;
    }

    pieces = board.get_pieces(BLACK, ROOK);
    while (pieces) {
        const uint8_t sq = pop_lsb(pieces);
        score -= hidden::PST[ROOK][sq];
        mat_black += hidden::MATERIAL_BONUS[ROOK];

        const bitboard file = hidden::COL[get_file(sq)];
        if (!(all_pawns & file))
            score -= hidden::ROOK_OPEN_FILE;
        else if (!(bP & file))
            score -= hidden::ROOK_SEMI_OPEN_FILE;
    }

    // Queens
    pieces = board.get_pieces(WHITE, QUEEN);
    while (pieces) {
        const uint8_t sq = hidden::FLIP[pop_lsb(pieces)];
        score += hidden::PST[QUEEN][sq];
        mat_white += hidden::MATERIAL_BONUS[QUEEN];

        const bitboard file = hidden::COL[get_file(sq)];
        if (!(all_pawns & file))
            score += hidden::QUEEN_OPEN_FILE;
        else if (!(wP & file))
            score += hidden::QUEEN_SEMI_OPEN_FILE;
    }

    pieces = board.get_pieces(BLACK, QUEEN);
    while (pieces) {
        const uint8_t sq = pop_lsb(pieces);
        score -= hidden::PST[QUEEN][sq];
        mat_black += hidden::MATERIAL_BONUS[QUEEN];

        const bitboard file = hidden::COL[get_file(sq)];
        if (!(all_pawns & file))
            score -= hidden::QUEEN_OPEN_FILE;
        else if (!(bP & file))
            score -= hidden::QUEEN_SEMI_OPEN_FILE;
    }

    // Kings
    score += mat_white > hidden::ENDGAME_MAT ?
            hidden::KING_M[hidden::FLIP[lsb(board.get_pieces(WHITE, KING))]] :
            hidden::KING_E[hidden::FLIP[lsb(board.get_pieces(WHITE, KING))]];

    score -= mat_black > hidden::ENDGAME_MAT ?
             hidden::KING_M[lsb(board.get_pieces(BLACK, KING))] :
             hidden::KING_E[lsb(board.get_pieces(BLACK, KING))];

    score += mat_white - mat_black;
    return board.get_curr_move() == WHITE ? score : -score;
}