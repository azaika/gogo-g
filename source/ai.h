#ifndef GOGO_HEADER_AI_H
#define GOGO_HEADER_AI_H

#include "game_state.h"
#include "game_state_hash.h"
#include "move.h"
#include "random.h"

#include <stdbool.h>
#include <string.h>

#define SEARCH_MEASURE 15

struct ai_seed_tag {
    // 相手玉と自分の駒の相対的な位置関係による評価値
    // 相手玉と自分の駒8種類について
    // 歩 0
    // 銀 1
    // 金、と金、成り銀 2
    // 角 3
    // 飛車 4
    // 馬 5
    // 龍 6
    // 自玉 7
    uint8_t table[8][9][5];
};

typedef struct ai_seed_tag ai_seed;

static void ai_copy_seed(const ai_seed* src, ai_seed* dest) {
    memcpy(dest->table, src->table, sizeof(src->table));
}

// ランダムな seed を作成する
static void ai_generate_random_seed(ai_seed* seed, pcg64_state* rng) {
    // 乱数が欲しい場合は
    // pcg64_random(rng)
    // とすれば 0 ～ 2^64 - 1 の範囲で乱数が得られる

    static uint8_t initial_table[8][9][5] = {
        {{}},
        {{}},
        {{}},
        {{}},
        {{}},
        {{}},
        {{}},
        {{}}
    };

    int size = sizeof(seed->table);
    uint8_t* t = (uint8_t*)(seed->table);
    for (int i = 0; i < size; ++i) {
        t[i] = pcg64_random(rng);
    }
}

// ai_seed をバイト列に書き込む
static void ai_serialize_seed(const ai_seed* seed, char* buf) {
    memcpy(buf, seed->table, sizeof(seed->table));
}

// バイト列を ai_seed に読み込む
static void ai_deserialize_seed(const char* buf, ai_seed* seed) {
    memcpy(seed->table, buf, sizeof(seed->table));
}
 
static int count_pieces(game_state state, piece_type type, bool is_first, bool is_tegoma, bool promoted) {
    int ret = 0;
    for (int i = 0; i < 2; ++i) {
        if (is_first != is_first_ones(state[type * 2 + i])) {
            continue;
        }
        if (is_tegoma) {
            if (get_coord(state[type * 2 + i]) == TEGOMA) {
                ++ret;
            }
        }
        else if (promoted != is_promoted(state[type * 2 + i])) {
            ++ret;
        }
    }

    return ret;
}

// 評価関数
// is_first : 先手番のターンかどうか
static int ai_evaluate(ai_seed* seed, game_state state, bool is_first) {
    int value = 0; // 評価値

    //駒の価値　(初期値)について
    value += 550 * count_pieces(state, PIECE_FU, is_first, false, false); // 歩があったら評価値+550
    value += 4050 * count_pieces(state, PIECE_GI, is_first, false, false); //銀があったら810
    value += 4550 * count_pieces(state, PIECE_KI, is_first, false, false); //金があったら
    value += 6450 * count_pieces(state, PIECE_KK, is_first, false, false); //角があったら
    value += 8350 * count_pieces(state, PIECE_HI, is_first, false, false); // 飛車があったら
    value += 4450 * count_pieces(state, PIECE_FU, is_first, false, true); // と金があったら
    value += 4650 * count_pieces(state, PIECE_GI, is_first, false, true); //成り銀があったら
    value += 9900 * count_pieces(state, PIECE_KK, is_first, false, true); //馬があったら
    value += 12000 * count_pieces(state, PIECE_HI, is_first, true, true); // 龍があったら
    value += 750 * count_pieces(state, PIECE_FU, is_first, true, false); // 持ち歩があったら
    value += 5500 * count_pieces(state, PIECE_GI, is_first, true, false); //持ち銀があったら
    value += 6300 * count_pieces(state, PIECE_KI, is_first, true, false); //持ち金があったら
    value += 7300 * count_pieces(state, PIECE_KK, is_first, true, false); //持ち角があったら
    value += 10000 * count_pieces(state, PIECE_HI, is_first, true, false); //持ち飛車があったら
    value += 250000 * count_pieces(state, PIECE_OU, is_first, false, false); //王があったら
    value += 250000 * count_pieces(state, PIECE_OU, is_first, true, false); //王があったら

    coord_type p = get_coord(state[PIECE_OU * 2 + (is_first ? 1 : 0)]);
    int px = p % 5, py = p / 5;

    for (int i = 0; i < NUM_KOMA; ++i) {
        if (is_first_ones(state[i]) != is_first) {
            continue;
        }
        coord_type c = get_coord(state[i]);
        int cx = c % 5, cy = c / 5;
        if (c == TEGOMA) {
            continue;
        }

        if (cx > px) {
            int tmp = cx;
            cx = px;
            px = tmp;
        }

        int kind;
        if (i / 2 == PIECE_FU) {
            kind = (is_promoted(state[i]) ? 2 : 0);
        }
        else if (i / 2 == PIECE_GI) {
            kind = (is_promoted(state[i]) ? 2 : 1);
        }
        else if (i / 2 ==  PIECE_KI) {
            kind = 2;
        }
        else if (i /2 == PIECE_KK) {
            kind = (is_promoted(state[i]) ?  5 : 3);
        }
        else if (i / 2 == PIECE_HI) {
            kind = (is_promoted(state[i]) ?  6 : 4);
        }
        else if (i / 2 == PIECE_OU) {
            kind = 7;
        }

        value += seed->table[kind][py - cy + 4][px - cx];
    }    

    return value;
}

static void all_possible_moves(game_state state, move_type* possible_moves, int* move_number, bool is_first){
    int moves_index = 0;
    for(int i = 0; i < NUM_KOMA; i++){
        if(is_first_ones(state[i]) == is_first){
            for(coord_type j = 0; j < 25; j++){
                move_type move_tmp;
                move_tmp.is_drop = get_coord(state[i] == TEGOMA);
                move_tmp.piece = (piece_type)((i / 2) % 6);
                move_tmp.from = get_coord(state[i]);
                move_tmp.to = j;
                for(int k = 0; k <= 1; k++){
                    move_tmp.do_promote = k;
                    if(validate_move(state, move_tmp, is_first)){
                        possible_moves[moves_index] = move_tmp;
                        moves_index++;
                    }
                }
            }
        }
    }
    *move_number = moves_index;
}

static int alpha_beta_min(ai_seed* seed, game_state state, bool is_first, int search_depth, int alpha, int beta, move_type* best_move, hash_table table);

static int alpha_beta_max(ai_seed* seed, game_state state, bool is_first, int search_depth, int alpha, int beta, move_type* best_move, hash_table table){
    int value;
    if(search_depth <= 0){
        return ai_evaluate(seed, state, is_first);
    }

    move_type possible_moves[200];
    int move_number = 0;
    all_possible_moves(state, possible_moves, &move_number, is_first);

    search_depth -= move_number / SEARCH_MEASURE;
    
    *best_move = possible_moves[0];
    
    for(int i = 0; i < move_number; i++){
        game_state next_state;
        for(int j = 0; j < NUM_KOMA; j++){
            next_state[j] = state[j];
        }
        write_move(next_state, possible_moves[i], is_first);
        move_type best_move_next;
        
        int value = 0;        
        int depth = search_depth + 1;
        game_state_hash hash_next = into_hash(next_state);

        value = hash_search_value(table, hash_next);   
        depth = hash_search_depth(table, hash_next);
        if(depth <= search_depth){
            value = -1;
        } 
        
        if(value == -1){
            value = alpha_beta_min(seed, next_state, !is_first, search_depth-1, alpha, beta, &best_move_next, table);
            hash_insert(table, hash_next, search_depth, value, best_move_next);
        }else{
            *best_move = hash_search_move(table, hash_next);
        }

        if(value > alpha){
            alpha = value;
            *best_move = possible_moves[i];
            if(alpha >= beta){
                return beta;
            }
        }
    }
    return alpha;
}

static int alpha_beta_min(ai_seed* seed, game_state state, bool is_first, int search_depth, int alpha, int beta, move_type* best_move, hash_table table) {
    int value;
    if(search_depth <= 0){
        return ai_evaluate(seed, state, is_first);
    }

    move_type possible_moves[200];
    int move_number = 0;
    all_possible_moves(state, possible_moves, &move_number, is_first);

    search_depth -= move_number / SEARCH_MEASURE;

    *best_move = possible_moves[0];

    for(int i = 0; i < move_number; i++){
        game_state next_state;
        for(int j = 0; j < NUM_KOMA; j++){
            next_state[j] = state[j];
        }
        write_move(next_state, possible_moves[i], is_first);
        move_type best_move_next;

        int value = 0;
        int depth = search_depth + 1;
        
        game_state_hash hash_next = into_hash(next_state);

        
        value = hash_search_value(table, hash_next);   
        depth = hash_search_depth(table, hash_next);
        if(depth <= search_depth){
            value = -1;
        }      
                
        if(value == -1){
            value = alpha_beta_min(seed, next_state, !is_first, search_depth-1, alpha, beta, &best_move_next, table);
            hash_insert(table, hash_next, search_depth, value, best_move_next);
        }else{
            *best_move = hash_search_move(table, hash_next);
        }

        if(value < beta){
            beta = value;            
            *best_move = possible_moves[i];
            if(beta <= alpha){
                return alpha;
            }
        }
    }
    return beta;
}

static move_type ai_decide_move(ai_seed* seed, game_state state, bool is_first, hash_table table) {
    move_type move;
    int search_depth = 7;
    int INF = 100000000;

    alpha_beta_max(seed, state, !is_first, search_depth, -INF, INF, &move, table);
    return move;
}

#endif // GOGO_HEADER_AI_H