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

    //if文の中身がどう書けば良いのか迷走したので　
    //歩とと金と持ち歩の3箇所だけやっていただけるとそれを真似てやれるので教えていただきたいです
    value += 110 * count_pieces(state, PIECE_FU, is_first, false, false); // 歩があったら評価値+ 110
    if (0) value += 810; // 銀があったら
    if (0) value += 910; //金があったら
    if (0) value += 1290; //角があったら
    if (0) value += 1670; // 飛車があったら
    value += 890 * count_pieces(state, PIECE_FU, is_first, false, true); // と金があったら
    if (0) value += 930; // 成り銀があったら
    if (0) value += 1980; // 馬があったら
    if (0) value += 2400; // 龍があったら
    value += 150 * count_pieces(state, PIECE_FU, is_first, true, false); // 持ち歩があったら
    if (0) value += 1100; // 持ち銀があったら
    if (0) value += 1260; // 持ち金があったら
    if (0) value += 1460; // 持ち角があったら
    if (0) value += 2000; // 持ち飛車があったら
    if (0) value += 50000; // 王があったら
    
    //相手玉と自分の駒の相対的な位置関係による評価値変動 (初期値)
    //相手玉が2回行動してたどり着ける範囲(9箇所)と自分の駒8種類について
    //どう表現したら良いのかわからないので
    //相手が後手だった時、相手玉が5E(11000)の時の歩の評価のところだけでもやっていただけると
    //あとはそれを真似してやれるので教えていただきたいです。
    

    // 歩
    // 銀
    // 金、と金、成り銀
    // 角
    // 飛車
    // 馬
    // 龍
    // 自玉
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
        // ...以下同様


        value += table[kind][py - cy + 4][px - cx];
    }    

    //相手が後手だった時、相手玉が5E(11000)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉


    //相手が先手だった時,相手玉が1A(00000)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が後手だった時、相手玉が5D(10111)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が先手だった時、相手玉が1B(00001)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が後手だった時、相手玉が5C(10110)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が先手だった時、相手玉が1C(00010)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が後手だった時、相手玉が4E(10011)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が先手だった時、相手玉が2A(00101)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が後手だった時、相手玉が4D(10010)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が先手だった時、相手玉が2B(00110)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が後手だった時、相手玉が4C(10001)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が先手だった時、相手玉が2C(00111)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が後手だった時、相手玉が3E(01110)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が先手だった時、相手玉が3A(01010)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が後手だった時、相手玉が3D(01101)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が先手だった時、相手玉が3B(01011)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が後手だった時、相手玉が3C(01100)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

    //相手が先手だった時、相手玉が3C(01100)の時
    //歩
    //銀
    //金、と金、成り銀
    //角
    //飛車
    //馬
    //龍
    //自玉

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