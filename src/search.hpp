#ifndef ATENIKA_SEARCH_HPP
#define ATENIKA_SEARCH_HPP

#include "order_info.hpp"
#include <chrono>
#include <atomic>

namespace Search {
namespace hidden {

    // Limits
    extern int64_t _nodes;
    extern int32_t _ms_allocated;
    extern bool _without_time;
    extern int16_t _depth;
    extern std::atomic<bool> _stop;

    extern int64_t _fh; // cut-off at n move. The moves are accumulating
    extern int64_t _fhf; // cut-off at first move

    // Search
    extern OrderInfo _order_info;
    extern Move _best_move;
    extern int32_t _best_score;
    extern std::chrono::time_point<std::chrono::steady_clock> _start;

    extern std::string _mate; // for mate check

    void _debug(const Board &board, int depth, int elapsed);
    void _restart();
    bool _check_limits();

    int32_t _negamax(Board &board, int16_t depth, int32_t alpha, int32_t beta, bool null_move);
    int32_t _quiescence(Board &board, int32_t alpha, int32_t beta);

} // hidden

    void init();
    void stop(); // stop search if time has expired

    void set_time(int32_t ms_allocated);
    void set_depth(int16_t depth);

    std::string get_mate();
    Move *get_best_move();
    std::string get_allocated_sec();
    int32_t get_search_depth();

    void iter_deep(Board &board, bool debug);
} // Search

#endif //ATENIKA_SEARCH_HPP
