#ifndef GOGO_HEADER_PIECE_H
#define GOGO_HEADER_PIECE_H

#include <stdbool.h>
#include <stdint.h>

// 各駒の状態を表す型
// 座標を 0～24 (どちらかの手駒の場合は 25) として 5 bit
// 成っているかどうかを 1 bit
// どちらのプレイヤーが持っているかを 1 bit
// 合計 7 bit の情報を整数として扱う
typedef int8_t piece_state;

// 座標を表す型
// 5 bit 整数として扱う
typedef int8_t coord_type;

// 駒が先手番のものかどうか
static bool is_first_ones(piece_state piece) {
    return ((piece >> 6) & 1) == 0;
}

// 駒が成っているかどうか
static bool is_promoted(piece_state piece) {
    return ((piece >> 5) & 1) == 1;
}

// 駒の座標を取得する
static coord_type get_coord(piece_state piece) {
    return (coord_type)(piece & 0b11111);
}

static const coord_type TEGOMA = 25;

enum piece_type_tag {
    PIECE_FU = 0, // 歩/と
    PIECE_GI = 1, // 銀/成銀
    PIECE_KI = 2, // 金
    PIECE_KK = 3, // 角/馬
    PIECE_HI = 4, // 飛/龍
    PIECE_OU = 5, // 王, 玉
    PIECE_EMPTY = 6
};

typedef enum piece_type_tag piece_type;

// 駒の種類を駒名に変換する
static const char* into_name(piece_type type, bool promoted) {
    switch (type) {
    case PIECE_FU:
        return (promoted ? "と" : "歩");
    case PIECE_GI:
        return (promoted ? "成" : "銀");
    case PIECE_KI:
        return (promoted ? "？" : "金"); // 金は成らないはずである
    case PIECE_KK:
        return (promoted ? "馬" : "角");
    case PIECE_HI:
        return (promoted ? "龍" : "飛");
    case PIECE_OU:
        return (promoted ? "！" : "王"); // 王も成らないはずである
    case PIECE_EMPTY:
        return "　";                     // 全角スペース
    default:
        return 0;
    }
}

enum piece_ability_tag {
    IMMOVABLE  = 0,
    REACH_ONE  = 1,
    REACH_LINE = 2
};

typedef enum piece_ability_tag piece_ability;

// direction
// 0 1 2
// 3 ↑ 4
// 5 6 7
static piece_ability get_piece_ability(piece_type type, bool promoted, int direction) {
    static const piece_ability fu[8] = {
        0, 1, 0,
        0,    0,
        0, 0, 0
    };
    static const piece_ability gin[8] = {
        1, 1, 1,
        0,    0,
        1, 0, 1
    };
    static const piece_ability kin[8] = {
        1, 1, 1,
        1,    1,
        0, 1, 0
    };
    static const piece_ability kaku[8] = {
        2, 0, 2,
        0,    0,
        2, 0, 2
    };
    static const piece_ability uma[8] = {
        2, 1, 2,
        1,    1,
        2, 1, 2
    };
    static const piece_ability hisha[8] = {
        0, 2, 0,
        2,    2,
        0, 2, 0
    };
    static const piece_ability ryu[8] = {
        1, 2, 1,
        2,    2,
        1, 2, 1
    };
    static const piece_ability ou[8] = {
        1, 1, 1,
        1,    1,
        1, 1, 1
    };

    switch (type) {
    case PIECE_FU:
        return (promoted ? kin[direction] : fu[direction]);
    case PIECE_GI:
        return (promoted ? kin[direction] : gin[direction]);
    case PIECE_KI:
        return kin[direction];
    case PIECE_KK:
        return (promoted ? uma[direction] : kaku[direction]);
    case PIECE_HI:
        return (promoted ? ryu[direction] : hisha[direction]);
    case PIECE_OU:
        return ou[direction];
    default:
        return IMMOVABLE;
    }
}

#endif // GOGO_HEADER_PIECE_H