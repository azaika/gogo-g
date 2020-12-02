#ifndef GOGO_HEADER_MOVE_H
#define GOGO_HEADER_MOVE_H

#include "game_state.h"

#include <stdbool.h>
#include <assert.h>

// 駒を動かす手を表す
// piece は is_drop が true  のときだけ有効
// from  は is_drop が false のときだけ有効
struct move_type_tag {
    bool is_drop;     // 打ちかどうか
    piece_type piece;
    coord_type from;
    coord_type to;
    bool do_promote;
};

typedef struct move_type_tag move_type;

// (動かして良いかの確認をしないで) 駒を動かす
void write_move(game_state state, move_type move) {
    int index = (get_coord(state[move.piece * 2]) == move.from) ? move.piece * 2 : move.piece * 2 + 1;
    state[index] = state[index] & 0b00000 + move.to;
}

// 行き先に他の駒がいないかどうか確かめる
// allow_opponent が true の場合, 相手の駒に重なるのは OK とする
bool validate_absent(board_type* board, move_type move, bool allow_opponent, bool is_first) {
    if(allow_opponent == true)
        return true;
    else if(board->field[move.from / 5][move.from % 5].type == PIECE_EMPTY)
        return true;
    else
        return false;    
}

// 二歩状態でないかどうか確かめる
bool validate_twopawn(board_type* board) {
    for(int i = 0; i < 5; i++){
        bool exist_fu_first = false;
        bool exist_fu_second = false;
        for(int j = 0; j < 5; j++){
            if(board->field[i][j].type == PIECE_FU){
                if(board->field[i][j].first_ones){
                    if(exist_fu_first == true){
                        return true;
                    }else{
                        exist_fu_first = true;
                    }
                }else{
                    if(exist_fu_second == true){
                        return true;
                    }else{
                        exist_fu_second = true;
                    }
                }
            }
        }
    }
    return false;
}

// 自分の持ち駒に piece があるかを確かめる
bool has_piece(board_type* board, piece_type piece, bool is_first) {
    for(int i = 0; board->hand[is_first][i] != PIECE_EMPTY; i++){
        if(board->hand[is_first][i] == piece){
            return true;
        } 
    }
    return false;
}

// 成れるかどうかを返す
bool can_promote(move_type move, bool is_first) {
    if(move.from / 5 == 0 && !is_first) return true;
    else if(move.from / 5 == 4 && is_first) return true;
    else if(move.to / 5 == 0 && !is_first && !move.is_drop) return true;
    else if(move.to / 5 == 4 && is_first && !move.is_drop) return true;
    else return false;
}

// 成らないままかどうかを返す
bool is_still_unpromoted(board_type* board, move_type move) {
    return can_promote(move, board->field[move.from / 5][move.from % 5].first_ones) && move.do_promote;
}

// 自分が王手をしているかどうかを返す
bool is_check(board_type* board, bool is_first) {  
    return true;
}

bool is_checkmate(board_type* board, bool is_first){
    return true;
}

// 駒の動きとして移動が可能かどうか
// 障害物 (味方の駒など) も考慮する
bool able_to_move(board_type* board, move_type move, bool is_first) {
    // ToDo: implement
    return false;
}

bool validate_move(game_state state, move_type move, bool is_first) {
    // 成れないのに成ろうとしたら NG
    if (!can_promote(move, is_first) && move.do_promote)
        return false;

    board_type cur_board;
    make_board(state, &cur_board);

    if (move.is_drop) {
        // 打つ先に駒があるのは NG
        if (!validate_absent(&cur_board, move, false, is_first))
            return false;
        // 打つ先が敵陣なのは NG
        if (can_promote(move, is_first))
            return false;
        
        // 持ち駒に無い駒を打つのは NG
        if (!has_piece(&cur_board, move.piece, is_first))
            return false;
    }
    else if (able_to_move(&cur_board, move, is_first)) // 駒の動きとして移動が可能でないなら NG
        return false;
    
    // 歩が成れるのに成っていないのは NG
    if (move.piece == PIECE_FU && can_promote(move, is_first) && is_still_unpromoted(&cur_board, move))
        return false;

    game_state next_state;
    copy_game_state(next_state, state);
    write_move(next_state, move);

    board_type next_board;
    make_board(next_state, &next_board);

    if (move.is_drop && move.piece == PIECE_FU) {
        // 二歩は NG
        if (!validate_twopawn(&next_board))
            return false;
        // 打ち歩詰めは NG
        if (is_check(&next_board, is_first))
            return false;
    }

    // 相手から王手を掛けられているのに放置するのは NG
    if (is_check(&cur_board, !is_first) && is_check(&next_board, !is_first))
        return false;

    return true;
}

// 入力文字列を move_type に変換する
// 無効な文字列だった場合は false を返す。この場合 move の中身は壊れていても良い
static bool parse_move(const char* input, move_type* move) {
    // ToDo: implement
    return false;
}

// move を表示する
static void print_move(move_type move) {
    // ToDo: implement
}

#endif // GOGO_HEADER_MOVE_H