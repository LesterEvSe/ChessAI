#include "board.hpp"

// example of short FEN: rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR
// start from last row. lowercase letters - black, uppercase - white
// p - pawn, r - rook, n - knight, b - bishop, k - king, q - queen
Board::Board(std::string short_fen) {
    int8_t pos = 56;
    for (uint8_t i = 0; i < short_fen.size(); ++i, ++pos)
        switch (short_fen[i]) {
            case 'p' : m_pieces[BLACK][PAWN]   |= (ONE << pos); break;
            case 'r' : m_pieces[BLACK][ROOK]   |= (ONE << pos); break;
            case 'n' : m_pieces[BLACK][KNIGHT] |= (ONE << pos); break;
            case 'b' : m_pieces[BLACK][BISHOP] |= (ONE << pos); break;
            case 'k' : m_pieces[BLACK][KING]   |= (ONE << pos); break;
            case 'q' : m_pieces[BLACK][QUEEN]  |= (ONE << pos); break;

            case 'P' : m_pieces[WHITE][PAWN]   |= (ONE << pos); break;
            case 'R' : m_pieces[WHITE][ROOK]   |= (ONE << pos); break;
            case 'N' : m_pieces[WHITE][KNIGHT] |= (ONE << pos); break;
            case 'B' : m_pieces[WHITE][BISHOP] |= (ONE << pos); break;
            case 'K' : m_pieces[WHITE][KING]   |= (ONE << pos); break;
            case 'Q' : m_pieces[WHITE][QUEEN]  |= (ONE << pos); break;

            case '/' : pos -= 17; break; // Because we have pos increment at the end
            default  : pos += static_cast<int8_t>(short_fen[i] - '1'); // Because of increment too
        }

    update_bitboards();
    m_hash.set_hash(*this); // Order is important! Hash need after all initializations
}

void Board::update_bitboards() {
    m_side[WHITE] = m_pieces[WHITE][PAWN] |
                    m_pieces[WHITE][ROOK] |
                    m_pieces[WHITE][KNIGHT] |
                    m_pieces[WHITE][BISHOP] |
                    m_pieces[WHITE][KING] |
                    m_pieces[WHITE][QUEEN];

    m_side[BLACK] = m_pieces[BLACK][PAWN] |
                    m_pieces[BLACK][ROOK] |
                    m_pieces[BLACK][KNIGHT] |
                    m_pieces[BLACK][BISHOP] |
                    m_pieces[BLACK][KING] |
                    m_pieces[BLACK][QUEEN];

    m_all = m_side[WHITE] | m_side[BLACK];
}

Color Board::get_curr_player_move()     const { return m_player_move; }
Color Board::get_opponent_player_move() const { return m_player_move == WHITE ? BLACK : WHITE; }

bitboard Board::get_pieces(Color color, PieceType piece) const { return m_pieces[color][piece]; }
bitboard Board::get_side_pieces(Color color)             const { return m_side[color]; }
bitboard Board::get_all_pieces()  const { return m_all;  }
bitboard Board::get_free_cells()  const { return ~m_all; }

uint8_t Board::get_half_moves()   const { return m_half_moves_counter;  }
uint8_t Board::get_en_passant()   const { return m_en_passant_file;     }

bool Board::get_white_qs_castle() const { return m_castling_rights & 1; }
bool Board::get_white_ks_castle() const { return m_castling_rights & 2; }
bool Board::get_black_qs_castle() const { return m_castling_rights & 4; }
bool Board::get_black_ks_castle() const { return m_castling_rights & 8; }


bool Board::in_check(Color color) const {
    bitboard pieces = get_pieces(color, KING);
    uint8_t king_cell = lsb(pieces);
    return under_attack(king_cell, color);
}

// This allows us not to go through absolutely all the pieces in order
// to say whether some piece attack this cell or not.

// Main idea. If on the current square there is for example a WHITE KNIGHT,
// and it attacks BLACK KNIGHT, therefore the cell under attack.
// TODO NEED TO TEST
bool Board::under_attack(uint8_t cell, Color color) const {
    Color opposite = get_opponent_player_move();
    if (get_pieces(opposite, PAWN) & Pawn::get_attacks(cell, color)) return true;
    if (get_pieces(opposite, KNIGHT) & Attacks::get_knight_attacks(cell)) return true;

    // Sliding Attacks
    bitboard blockers = get_side_pieces(color);
    if ((get_pieces(opposite, ROOK) | get_pieces(opposite, QUEEN)) &
        Attacks::get_rook_attacks(cell, blockers)) return true;

    if ((get_pieces(opposite, BISHOP) | get_pieces(opposite, QUEEN)) &
        Attacks::get_bishop_attacks(cell, blockers)) return true;
    return false;
}

void Board::add_piece(Color color, PieceType piece, uint8_t cell) {
    set1(m_pieces[color][piece], cell);
    m_hash.xor_piece(color, piece, cell);
}

void Board::remove_piece(Color color, PieceType piece, uint8_t cell) {
    set0(m_pieces[color][piece], cell);
    m_hash.xor_piece(color, piece, cell);
}

void Board::makemove(const Move &move) {

}

std::ostream& operator<<(std::ostream &out, const Board &pieces) {
    out << "   ";
    for (char let = 'A'; let <= 'H'; ++let)
        out << ' ' << let;
    out << std::endl;

    for (int8_t row = 7; row >= 0; --row) {
        out << row+1 << " |";
        for (uint8_t col = 0; col < 8; ++col) {
            int8_t temp = row * 8 + col;
            if      (pieces.m_pieces[BLACK][PAWN]   & (ONE << temp)) out << " p";
            else if (pieces.m_pieces[BLACK][ROOK]   & (ONE << temp)) out << " r";
            else if (pieces.m_pieces[BLACK][KNIGHT] & (ONE << temp)) out << " n";
            else if (pieces.m_pieces[BLACK][BISHOP] & (ONE << temp)) out << " b";
            else if (pieces.m_pieces[BLACK][KING]   & (ONE << temp)) out << " k";
            else if (pieces.m_pieces[BLACK][QUEEN]  & (ONE << temp)) out << " q";

            else if (pieces.m_pieces[WHITE][PAWN]   & (ONE << temp)) out << " P";
            else if (pieces.m_pieces[WHITE][ROOK]   & (ONE << temp)) out << " R";
            else if (pieces.m_pieces[WHITE][KNIGHT] & (ONE << temp)) out << " N";
            else if (pieces.m_pieces[WHITE][BISHOP] & (ONE << temp)) out << " B";
            else if (pieces.m_pieces[WHITE][KING]   & (ONE << temp)) out << " K";
            else if (pieces.m_pieces[WHITE][QUEEN]  & (ONE << temp)) out << " Q";
            else out << " -";
        }
        out << std::endl;
    }
    out << "--------------------\n\n";
    return out;
}
