#ifndef NEUTREEKO_HEADER_PIECE_H
#define NEUTREEKO_HEADER_PIECE_H

#include <stdbool.h>
#include <stdint.h>

enum piece_type_tag {
    PIECE_FU = 0, // 歩
    PIECE_GI = 1, // 銀
    PIECE_KI = 2, // 金
    PIECE_KK = 3, // 角
    PIECE_HI = 4, // 飛
    PIECE_OU = 5, // 王, 玉
    PIECE_EMPTY = 6
};

typedef enum piece_type_tag piece_type;

// 各駒 (歩1, 歩2, 銀1, ... , 飛2, 王, 玉) につき
// 座標を 0～24 (どちらかの手駒の場合は 25) として 5 bit
// どちらのプレイヤーが持っているかを 1 bit
// 合計 6 bit * 12 = 72 bit の情報として保持する (残り 56 bit は使わない)
// __int128_t は標準 C 言語には無いが, gcc による拡張でサポートされている
typedef __int128_t game_state;

static const game_state initial_state =
    ((__int128_t)0b000110 << (6 * 0)) +
    ((__int128_t)0b110011 << (6 * 1)) +
    ((__int128_t)0b000101 << (6 * 2)) +
    ((__int128_t)0b110100 << (6 * 3)) +
    ((__int128_t)0b000011 << (6 * 4)) +
    ((__int128_t)0b110101 << (6 * 5)) +
    ((__int128_t)0b000010 << (6 * 6)) +
    ((__int128_t)0b110110 << (6 * 7)) +
    ((__int128_t)0b000001 << (6 * 8)) +
    ((__int128_t)0b110111 << (6 * 9)) +
    ((__int128_t)0b000000 << (6 *10)) +
    ((__int128_t)0b111000 << (6 *11));

// 座標を表す型
// 5 bit 整数として扱う
typedef int8_t coord_type;
// 駒の状態を表す型
// 6 bit 整数として扱う
typedef int8_t piece_state;

static piece_state get_piece_state(game_state state, piece_type piece, bool which) {
    return (piece_state)((state >> ((int)piece * 2 + which)) & 0b111111);
}

static bool is_first_ones(piece_state piece) {
    return ((piece >> 5) & 1) == 0;
}

static coord_type get_coord(piece_state piece) {
    return (coord_type)(piece & 0b11111);
}

static const coord_type TEGOMA = 0b11001;

// 駒の種類を駒名に変換する
static const char* into_name(piece_type p) {
    switch (p) {
    case PIECE_FU:
        return "歩";
    case PIECE_GI:
        return "銀";
    case PIECE_KI:
        return "金";
    case PIECE_KK:
        return "角";
    case PIECE_HI:
        return "飛";
    case PIECE_OU:
        return "王";
    default:
        return 0;
    }
}

static const int FIELD_SIZE = 5;

#endif // NEUTREEKO_HEADER_PIECE_H