#ifndef GOGO_HEADER_AI_H
#define GOGO_HEADER_AI_H

#include "game_state.h"
#include "move.h"
#include "random.h"

#include <stdbool.h>

struct ai_seed_tag {
    // ToDo: implement
};

typedef struct ai_seed_tag ai_seed;

static void ai_copy_seed(const ai_seed* src, ai_seed* dest) {
    // ToDo: implement
}

// ランダムな seed を作成する
static void ai_generate_random_seed(ai_seed* seed, pcg64_state* rng) {
    // ToDo: implement
}

// ai_seed を文字列に変換する
static void ai_serialize_seed(ai_seed* seed, const char* buf) {
    // ToDo: implement
}

// 文字列を ai_seed に変換する
// 失敗した場合は NULL を返す
static ai_seed* ai_deserialize_seed(const char* str) {
    // ToDo: implement
    return NULL;
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
static int ai_evaluate(game_state state, bool is_first) {
    int value = 0; //valueは評価値

    // board_type board;
    // make_board(state, &board);

    //駒の価値　(初期値)について
    value += 110 * count_pieces(state, PIECE_FU, is_first, false, false); // 歩があったら評価値+ 110
    value += 810 * count_pieces(state, PIECE_GI, is_first, false, false); //銀があったら810
    value += 910 * count_pieces(state, PIECE_KI, is_first, false, false); //金があったら
    value += 1290 * count_pieces(state, PIECE_KK, is_first, false, false); //角があったら
    value += 1670 * count_pieces(state, PIECE_HI, is_first, false, false); // 飛車があったら
    value += 890 * count_pieces(state, PIECE_FU, is_first, false, true); // と金があったら
    value += 930 * count_pieces(state, PIECE_GI, is_first, false, true); //成り銀があったら
    value += 1980 * count_pieces(state, PIECE_KK, is_first, false, true); //馬があったら
    value += 2400 * count_pieces(state, PIECE_HI, is_first, true, true); // 龍があったら
    value += 150 * count_pieces(state, PIECE_FU, is_first, true, false); // 持ち歩があったら
    value += 1100 * count_pieces(state, PIECE_GI, is_first, true, false); //持ち銀があったら
    value += 1260 * count_pieces(state, PIECE_KI, is_first, true, false); //持ち金があったら
    value += 1460 * count_pieces(state, PIECE_KK, is_first, true, false); //持ち角があったら
    value += 2000 * count_pieces(state, PIECE_HI, is_first, true, false); //持ち飛車があったら
    value += 50000 * count_pieces(state, PIECE_OU, is_first, false, false); //王があったら
    
    //相手玉と自分の駒の相対的な位置関係による評価値変動 (初期値)
    //相手玉と自分の駒8種類について
    // 歩 0
    // 銀 1
    // 金、と金、成り銀 2
    // 角 3
    // 飛車 4
    // 馬 5
    // 龍 6
    // 自玉 7
    uint8_t table[8][9][5] = {
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {}
    }; // 0～255

    coord_type p = get_coord(state[PIECE_OU * 2 + (is_first ? 1 : 0)]);
    int px = p % 5, py = p / 5;

    for (int i = 0; i < 12; ++i) {
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
            kind = (is_promoted(state[i]) ?  3 : 5);
        }
        else if (i / 2 == PIECE_HI) {
            kind = (is_promoted(state[i]) ?  4 : 6);
        }
        else if (i / 2 == PIECE_OU) {
            kind = 7;
        }

    value += table[kind][py - cy + 4][px - cx];
    }    

    return value;
}

static move_type ai_decide_move(ai_seed* seed, game_state state, bool is_first) {
    // ToDo: implement
    // テスト用で書き換えてありますが特に意味はありません
    move_type move;
    move.is_drop = false;
    move.piece = PIECE_OU;
    move.from = 24;
    move.to = 19;
    move.do_promote = false;

    return move;
}

#endif // GOGO_HEADER_AI_H