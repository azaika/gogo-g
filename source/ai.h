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