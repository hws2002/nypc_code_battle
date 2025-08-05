// v9_2.cpp - Enhanced AI with BEST MODEL (62.08% win rate) insights
// Based on v9_enhanced but corrected with TRUE high-performance patterns
// Key insight: Best model prefers LARGER moves (size 4-5) much more than size-2

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <climits>

using namespace std;

const int ROWS = 10, COLS = 17;
const int INF = 1e9;

enum Player { NO_ONE = 0, ME = 1, OPPONENT = 2 };

struct Cell {
    int value;
    Player owner;
};

struct Move {
    int r1, c1, r2, c2;
    Move(int r1 = -1, int c1 = -1, int r2 = -1, int c2 = -1) : r1(r1), c1(c1), r2(r2), c2(c2) {}
    bool operator==(const Move& other) const {
        return r1 == other.r1 && c1 == other.c1 && r2 == other.r2 && c2 == other.c2;
    }
};

enum TTEntryType { EXACT, LOWER_BOUND, UPPER_BOUND };

struct TTEntry {
    int score;
    int depth;
    TTEntryType type;
    Move best_move;
};

class EnhancedAI {
private:
    vector<vector<Cell>> board;
    vector<vector<long long>> prefix_sum;
    unordered_map<uint64_t, TTEntry> transposition_table;
    chrono::steady_clock::time_point turn_start_time;
    uint64_t current_hash;
    int time_limit_ms;
    
    // Enhanced search statistics
    int nodes_searched;
    int tt_hits;
    
    // Zobrist hashing
    uint64_t zobrist_keys[ROWS][COLS][3];

public:
    void init_zobrist() {
        srand(42);
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                for (int p = 0; p < 3; ++p) {
                    zobrist_keys[r][c][p] = ((uint64_t)rand() << 32) | rand();
                }
            }
        }
    }

    void init_board(const vector<string>& initial_board_str) {
        board.assign(ROWS, vector<Cell>(COLS));
        current_hash = 0;
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                board[r][c] = {initial_board_str[r][c] - '0', NO_ONE};
                current_hash ^= zobrist_keys[r][c][NO_ONE];
            }
        }
        calculate_prefix_sum();
    }

    void calculate_prefix_sum() {
        prefix_sum.assign(ROWS + 1, vector<long long>(COLS + 1, 0));
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                prefix_sum[r + 1][c + 1] = board[r][c].value + prefix_sum[r][c + 1] + prefix_sum[r + 1][c] - prefix_sum[r][c];
            }
        }
    }

    bool is_valid_move(int r1, int c1, int r2, int c2) {
        long long current_sum = 0;
        for (int r = r1; r <= r2; ++r) {
            for (int c = c1; c <= c2; ++c) {
                if (board[r][c].owner == NO_ONE) {
                    current_sum += board[r][c].value;
                }
            }
        }
        if (current_sum != 10) return false;

        bool edge_ok[4] = {false};
        for (int r = r1; r <= r2; ++r) {
            for (int c = c1; c <= c2; ++c) {
                if (board[r][c].owner == NO_ONE) {
                    if (r == r1) edge_ok[0] = true;
                    if (r == r2) edge_ok[1] = true;
                    if (c == c1) edge_ok[2] = true;
                    if (c == c2) edge_ok[3] = true;
                }
            }
        }
        return edge_ok[0] && edge_ok[1] && edge_ok[2] && edge_ok[3];
    }

    // REVOLUTIONARY: Evaluation based on BEST MODEL (62.08% win rate) insights
    int evaluate_move_best_model(const Move& move) {
        int score = 0;
        
        int move_value = 0;
        int empty_cells = 0;
        int move_size = (move.r2 - move.r1 + 1) * (move.c2 - move.c1 + 1);
        
        // Count value and empty cells
        for (int r = move.r1; r <= move.r2; ++r) {
            for (int c = move.c1; c <= move.c2; ++c) {
                move_value += board[r][c].value;
                if (board[r][c].owner == NO_ONE) {
                    empty_cells++;
                }
            }
        }
        
        // PRIMARY: Efficiency remains important
        double efficiency = (double)move_value / move_size;
        score += (int)(efficiency * 100.0);  // Slightly reduced to make room for size bonuses
        
        // REVOLUTIONARY: TRUE best model size preferences
        // Based on probability-normalized preferences from 62.08% win rate model
        if (move_size == 1) {
            // Impossible but handle gracefully
            score += 0;
        } else if (move_size == 2) {
            // REDUCED preference (2.57x vs previous 5.85x)
            score += 12;  // Reduced from 20
        } else if (move_size == 3) {
            // INCREASED preference (5.78x vs previous 4.05x) 
            score += 18;  // Increased from 10
        } else if (move_size == 4) {
            // MAJOR INCREASE (11.16x vs previous 6.25x)
            score += 35;  // Changed from -8 penalty to +35 bonus!
        } else if (move_size >= 5) {
            // MASSIVE INCREASE (68.03x vs previous 9.52x)
            score += 55;  // Changed from penalty to huge bonus!
        }
        
        // Territory control bonus for larger moves (new insight)
        if (move_size >= 4) {
            // Large moves provide strategic territory control
            score += move_size * 8;  // Additional bonus for territory
        }
        
        // Empty cells preference (refined)
        score += empty_cells * 22;  // Slightly adjusted
        
        // Edge preference with size consideration
        bool is_edge = (move.r1 == 0 || move.r1 == ROWS-1 || 
                       move.c1 == 0 || move.c1 == COLS-1 ||
                       move.r2 == 0 || move.r2 == ROWS-1 ||
                       move.c2 == 0 || move.c2 == COLS-1);
        if (is_edge) {
            // Larger edge moves get more bonus
            score += 12 + (move_size >= 4 ? 8 : 0);
        }
        
        // Game phase adaptation
        int total_cells = ROWS * COLS;
        int occupied_cells = 0;
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                if (board[r][c].owner != NO_ONE) {
                    occupied_cells++;
                }
            }
        }
        double game_progress = (double)occupied_cells / total_cells;
        
        // Late game: prefer even larger moves for territory control
        if (game_progress > 0.6 && move_size >= 4) {
            score += (int)(20 * game_progress);  // Late game large move bonus
        }
        
        return score;
    }

    vector<Move> generate_and_order_moves(const Move& hash_move = Move(-1,-1,-1,-1)) {
        vector<Move> moves;
        
        // Generate all valid moves
        for (int r1 = 0; r1 < ROWS; ++r1) {
            for (int c1 = 0; c1 < COLS; ++c1) {
                if (board[r1][c1].owner != NO_ONE) continue;
                
                for (int r2 = r1; r2 < ROWS; ++r2) {
                    for (int c2 = c1; c2 < COLS; ++c2) {
                        if (is_valid_move(r1, c1, r2, c2)) {
                            moves.push_back(Move(r1, c1, r2, c2));
                        }
                    }
                }
            }
        }
        
        // Enhanced move ordering with BEST MODEL evaluation
        sort(moves.begin(), moves.end(), [this, &hash_move](const Move& a, const Move& b) {
            // Hash move gets highest priority
            if (a == hash_move) return true;
            if (b == hash_move) return false;
            
            // Then sort by BEST MODEL evaluation
            return evaluate_move_best_model(a) > evaluate_move_best_model(b);
        });
        
        return moves;
    }

    // Enhanced evaluation function
    int evaluate() {
        int my_cells = 0, opp_cells = 0;
        int my_score = 0, opp_score = 0;
        int empty_cells = 0;
        
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                if (board[r][c].owner == ME) {
                    my_cells++;
                    my_score += board[r][c].value;
                } else if (board[r][c].owner == OPPONENT) {
                    opp_cells++;
                    opp_score += board[r][c].value;
                } else {
                    empty_cells++;
                }
            }
        }
        
        // Enhanced evaluation with game progress consideration
        double game_progress = 1.0 - (double)empty_cells / (ROWS * COLS);
        
        // Adaptive weights based on game progress
        int material_weight = 15 + (int)(25 * game_progress);  // 15-40
        int cell_weight = 8 + (int)(12 * game_progress);       // 8-20
        
        return (my_score - opp_score) * material_weight + (my_cells - opp_cells) * cell_weight;
    }

    // Enhanced alpha-beta search with transposition table
    int alpha_beta(int depth, int alpha, int beta, bool is_maximizing, Move& best_move_found) {
        nodes_searched++;
        
        if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - turn_start_time).count() > time_limit_ms) {
            return 0;
        }
        
        int alpha_orig = alpha;
        Move hash_move(-1, -1, -1, -1);
        
        // Transposition table lookup
        auto it = transposition_table.find(current_hash);
        if (it != transposition_table.end() && it->second.depth >= depth) {
            tt_hits++;
            const TTEntry& entry = it->second;
            hash_move = entry.best_move;
            
            if (entry.type == EXACT) return entry.score;
            if (entry.type == LOWER_BOUND) alpha = max(alpha, entry.score);
            else if (entry.type == UPPER_BOUND) beta = min(beta, entry.score);
            if (alpha >= beta) return entry.score;
        }

        if (depth == 0) return evaluate();

        vector<Move> moves = generate_and_order_moves(hash_move);
        if (moves.empty()) return evaluate();

        Move local_best_move = moves[0];

        if (is_maximizing) {
            int max_eval = -INF;
            for (const auto& move : moves) {
                if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - turn_start_time).count() > time_limit_ms) break;
                
                // Apply move with hash update
                vector<Cell> backup_cells;
                uint64_t backup_hash = current_hash;
                for (int r = move.r1; r <= move.r2; ++r) {
                    for (int c = move.c1; c <= move.c2; ++c) {
                        backup_cells.push_back(board[r][c]);
                        current_hash ^= zobrist_keys[r][c][board[r][c].owner];
                        board[r][c].owner = ME;
                        current_hash ^= zobrist_keys[r][c][ME];
                    }
                }
                
                Move dummy_move;
                int eval = alpha_beta(depth - 1, alpha, beta, false, dummy_move);
                
                if (eval > max_eval) {
                    max_eval = eval;
                    local_best_move = move;
                }
                alpha = max(alpha, eval);
                
                // Restore move
                current_hash = backup_hash;
                int idx = 0;
                for (int r = move.r1; r <= move.r2; ++r) {
                    for (int c = move.c1; c <= move.c2; ++c) {
                        board[r][c] = backup_cells[idx++];
                    }
                }
                
                if (beta <= alpha) break;  // Alpha-beta cutoff
            }
            
            best_move_found = local_best_move;
            
            // Store in transposition table
            TTEntry entry;
            entry.score = max_eval;
            entry.depth = depth;
            entry.best_move = local_best_move;
            if (max_eval <= alpha_orig) entry.type = UPPER_BOUND;
            else if (max_eval >= beta) entry.type = LOWER_BOUND;
            else entry.type = EXACT;
            transposition_table[current_hash] = entry;
            
            return max_eval;
        } else {
            int min_eval = INF;
            for (const auto& move : moves) {
                if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - turn_start_time).count() > time_limit_ms) break;
                
                // Apply move with hash update
                vector<Cell> backup_cells;
                uint64_t backup_hash = current_hash;
                for (int r = move.r1; r <= move.r2; ++r) {
                    for (int c = move.c1; c <= move.c2; ++c) {
                        backup_cells.push_back(board[r][c]);
                        current_hash ^= zobrist_keys[r][c][board[r][c].owner];
                        board[r][c].owner = OPPONENT;
                        current_hash ^= zobrist_keys[r][c][OPPONENT];
                    }
                }
                
                Move dummy_move;
                int eval = alpha_beta(depth - 1, alpha, beta, true, dummy_move);
                
                if (eval < min_eval) {
                    min_eval = eval;
                    local_best_move = move;
                }
                beta = min(beta, eval);
                
                // Restore move
                current_hash = backup_hash;
                int idx = 0;
                for (int r = move.r1; r <= move.r2; ++r) {
                    for (int c = move.c1; c <= move.c2; ++c) {
                        board[r][c] = backup_cells[idx++];
                    }
                }
                
                if (beta <= alpha) break;  // Alpha-beta cutoff
            }
            
            best_move_found = local_best_move;
            
            // Store in transposition table
            TTEntry entry;
            entry.score = min_eval;
            entry.depth = depth;
            entry.best_move = local_best_move;
            if (min_eval <= alpha_orig) entry.type = UPPER_BOUND;
            else if (min_eval >= beta) entry.type = LOWER_BOUND;
            else entry.type = EXACT;
            transposition_table[current_hash] = entry;
            
            return min_eval;
        }
    }

    // Enhanced search with iterative deepening
    Move find_best_move(long long my_time, long long opp_time) {
        turn_start_time = chrono::steady_clock::now();
        
        // More aggressive time management
        time_limit_ms = min(my_time / 8, 600LL);  // Increased from /15 to /8, cap at 600ms
        
        nodes_searched = 0;
        tt_hits = 0;
        
        vector<Move> moves = generate_and_order_moves();
        if (moves.empty()) {
            return Move(-1, -1, -1, -1);
        }

        Move best_move = moves[0];
        int best_score = -INF;

        // Iterative deepening: try depths 1, 2, 3, 4
        for (int depth = 1; depth <= 4; ++depth) {
            if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - turn_start_time).count() > time_limit_ms * 0.8) {
                break;  // Reserve 20% time for safety
            }
            
            Move current_best;
            int score = alpha_beta(depth, -INF, INF, true, current_best);
            
            if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - turn_start_time).count() <= time_limit_ms) {
                best_move = current_best;
                best_score = score;
            } else {
                break;  // Time exceeded, use previous result
            }
        }

        return best_move;
    }

    void apply_move(const Move& move, Player player) {
        if (move.r1 == -1) return;
        
        for (int r = move.r1; r <= move.r2; ++r) {
            for (int c = move.c1; c <= move.c2; ++c) {
                current_hash ^= zobrist_keys[r][c][board[r][c].owner];
                board[r][c].owner = player;
                current_hash ^= zobrist_keys[r][c][player];
            }
        }
    }
};

EnhancedAI ai;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    ai.init_zobrist();
    
    string line;
    
    while (getline(cin, line)) {
        istringstream iss(line);
        string command;
        iss >> command;
        
        if (command == "READY") {
            cout << "OK" << endl;
        } else if (command == "INIT") {
            vector<string> board_data;
            string token;
            while (iss >> token) {
                board_data.push_back(token);
            }
            ai.init_board(board_data);
        } else if (command == "TIME") {
            long long my_time, opp_time;
            iss >> my_time >> opp_time;
            
            Move best_move = ai.find_best_move(my_time, opp_time);
            ai.apply_move(best_move, ME);
            
            cout << best_move.r1 << " " << best_move.c1 << " " 
                 << best_move.r2 << " " << best_move.c2 << endl;
        } else if (command == "OPP") {
            Move opp_move;
            int time_used;
            iss >> opp_move.r1 >> opp_move.c1 >> opp_move.r2 >> opp_move.c2 >> time_used;
            ai.apply_move(opp_move, OPPONENT);
        } else if (command == "FINISH") {
            break;
        }
    }
    
    return 0;
}
