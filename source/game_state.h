#ifndef GOGO_HEADER_GAMESTATE_H
#define GOGO_HEADER_GAMESTATE_H

#include "piece.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define NUM_KOMA (12)

// 盤面の状態を piece_state 12 個の配列として保持
typedef piece_state(game_state)[NUM_KOMA];

// 盤面の状態を整数として扱いたいときに使う
// __int128_t は標準 C 言語には無いが, gcc による拡張でサポートされている
typedef __int128_t game_state_hash;

static game_state_hash into_hash(game_state state) {
    game_state_hash hash = 0;
    for (int i = 0; i < NUM_KOMA; ++i)
        hash += ((game_state_hash)state[i]) << (i * 8);
    
    return hash;
}

static void from_hash(game_state_hash hash, game_state state){
    for (int i = NUM_KOMA-1; i >= 0; --i){
        state[i] = (piece_state)(hash & 0b11111111);
        hash >>= 8;
    }
}

static const game_state initial_state = {
    0b0000101, // △歩
    0b1010011, // ▼歩
    0b0000010, // △銀
    0b1010110, // ▼銀
    0b0000001, // △金
    0b1010111, // ▼金
    0b0000011, // △角
    0b1010101, // ▼角
    0b0000100, // △飛
    0b1010100, // ▼飛
    0b0000000, // △王
    0b1011000, // ▼玉
};

static void copy_game_state(game_state dest, const game_state src) {
    assert(dest != NULL);
    assert(src != NULL);

    if (dest == src)
        return;
    
    memcpy(dest, src, sizeof(game_state));
}

static game_state* generate_game_state() {
    game_state* state = (game_state*)malloc(sizeof(game_state));
    if (state == NULL) {
        fprintf(stderr, "memory allocation error in generate_game_state()\n");
        exit(EXIT_FAILURE);
    }

    copy_game_state(*state, initial_state);

    return state;
}

static void free_game_state(game_state* state) {
    if (state != NULL)
        free(state);
}

// 各マスの状態を (人間に扱いやすい形で) 表す
// promoted, first_ones は type が PIECE_EMPTY でないときだけ意味を持つ
struct cell_state_tag {
    piece_type type;
    bool promoted;
    bool first_ones;
};

typedef struct cell_state_tag cell_state;

// 盤面の状態を (人間に扱いやすい形で) 表す
struct board_type_tag {
    piece_type hand[2][11];
    cell_state field[5][5];
};

typedef struct board_type_tag board_type;

static board_type* board_new() {
    board_type* b = (board_type*)malloc(sizeof(board_type));
    if (b == NULL) {
        fprintf(stderr, "memory allocation error in board_new()\n");
        exit(EXIT_FAILURE);
    }

    return b;
}

static void board_free(board_type* b) {
    if (b != NULL)
        free(b);
}

// state を基に board に情報を書き込む
// board は確保された有効なポインタである必要がある
static void make_board(game_state state, board_type* board) {
    int num_hand[2] = {};
    for (int i = 0; i < 22; ++i)
        board->hand[i / 11][i % 11] = PIECE_EMPTY;
    
    for (int i = 0; i < 25; ++i)
        board->field[i / 5][i % 5].type = PIECE_EMPTY;
    
    for (int i = 0; i < NUM_KOMA; ++i) {
        piece_state p = state[i];
        int8_t coord = get_coord(p);

        if (coord == TEGOMA) {
            int w = (is_first_ones(p) ? 0 : 1);
            board->hand[w][num_hand[w]] = (piece_type)((i / 2) % 6);
            ++num_hand[w];
        }
        else {
            cell_state* cell = &board->field[coord / 5][coord % 5];
            cell->type = (piece_type)((i / 2) % 6);
            cell->promoted = is_promoted(p);
            cell->first_ones = is_first_ones(p);
        }
    }
}

static void print_board(game_state state) {
    board_type board;
    
    make_board(state, &board);

    printf("▼手駒: ");
    for (int i = 0; board.hand[1][i] != PIECE_EMPTY; ++i)
        printf("%s", into_name(board.hand[1][i], false));
    
    printf("\n");

    printf("__________________________\n");
    for (int y = 4; y >= 0; --y) {
        
        for (int x = 0; x < 5; ++x) {
            cell_state* cell = &board.field[y][x];

            if (cell->type != PIECE_EMPTY) {
                const char* which_ones = (cell->first_ones ? "▲" : "▼");
                const char* koma = into_name(cell->type, cell->promoted);

                printf("|%s%s ", which_ones, koma);
            }
            else
                printf("|　　");
        }
        printf("|\n");
        printf("|____|____|____|____|____|\n");
    }
    
    printf("▲手駒: ");
    for (int i = 0; board.hand[0][i] != PIECE_EMPTY; ++i)
        printf("%s", into_name(board.hand[0][i], false));

    printf("\n");
}

#endif // GOGO_HEADER_GAMESTATE_H