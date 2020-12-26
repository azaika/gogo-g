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
void write_move(game_state state, move_type move, bool is_first) {
    for(int i = 0; i < 12; i++){
        if(get_coord(state[i]) == move.to){
            state[i] &= 0b0000000;
            state[i] |= TEGOMA;
            state[i] |= ((!is_first) << 6);
            break;
        }
    }
    int index = (get_coord(state[move.piece * 2]) == move.from) ? move.piece * 2 : move.piece * 2 + 1;
    state[index] = ((state[index] >> 5) << 5) + move.to;
    state[index] |= move.do_promote << 5;
}

// 行き先に他の駒がいないかどうか確かめる
// allow_opponent が true の場合, 相手の駒に重なるのは OK とする
bool validate_absent(board_type* board, move_type move, bool allow_opponent, bool is_first) {
    if(allow_opponent == true && board->field[move.to / 5][move.to % 5].first_ones != is_first)
        return true;
    else if(board->field[move.to / 5][move.to % 5].type == PIECE_EMPTY)
        return true;
    else
        return false;    
}

// 二歩状態でないかどうか確かめる 二歩ならtrue
bool validate_twopawn(board_type* board) {
    for(int j = 0; j < 5; j++){
        bool exist_fu_first = false;
        bool exist_fu_second = false;
        for(int i = 0; i < 5; i++){
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
    for(int i = 0; board->hand[!is_first][i] != PIECE_EMPTY; i++){
        if(board->hand[!is_first][i] == piece){
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
    return !(can_promote(move, board->field[move.from / 5][move.from % 5].first_ones) && move.do_promote);
}

// 駒の動きとして移動が可能かどうか
// 障害物 (味方の駒など) も考慮する
bool able_to_move(board_type* board, move_type move, bool is_first) {
    if (move.is_drop == true && can_promote(move, is_first) && move.piece == PIECE_FU)
        return false;
    if (move.is_drop == false)
    {
        //移動先に駒があるのは NG
        if (!validate_absent(board, move, true, is_first))
            return false;
        
        if (board->field[move.from / 5][move.from % 5].first_ones != is_first)
        {
            //相手の駒を動かそうとした
            return false;
        }

        int diff_x = move.to / 5 - move.from / 5;
        int diff_y = move.to % 5 - move.from % 5;

        int d = 9; //方角（1~8）
        if (diff_x == -diff_y && diff_y > 0)
            d = (is_first) ? 7 : 0;
        if (diff_x == 0 && diff_y > 0)
            d = (is_first) ? 4 : 3;
        if (diff_x == diff_y && diff_y > 0)
            d = (is_first) ? 2 : 5;
        if (diff_x > 0 && diff_y == 0)
            d = (is_first) ? 1 : 6;
        if (diff_x == -diff_y && diff_y < 0)
            d = (is_first) ? 0 : 7;
        if (diff_x == 0 && diff_y < 0)
            d = (is_first) ? 3 : 4;
        if (diff_x == diff_y && diff_y < 0)
            d = (is_first) ? 5 : 2;
        if (diff_x < 0 && diff_y == 0)
            d = (is_first) ? 6 : 1;

        if (d == 9)
            return false; //どの方角でもない

        int vec_x = (diff_x >= 0) ? ((diff_x == 0) ? 0 : 1) : -1;
        int vec_y = (diff_y >= 0) ? ((diff_y == 0) ? 0 : 1) : -1;

        int ability = get_piece_ability(move.piece, board->field[move.from / 5][move.from % 5].promoted, d);
        if (ability == 0)
            return false;
        if (ability == 1 && (abs(diff_x) > 1 || abs(diff_y) > 1))
            return false;
        if (ability == 2)
        {
            int x_between = move.to / 5 - vec_x;
            int y_between = move.to % 5 - vec_y;
            //間に駒がある
            while (x_between != move.from / 5 || y_between != move.from % 5)
            {
                if (board->field[x_between][y_between].type != PIECE_EMPTY)
                {
                    return false;
                }
                x_between -= vec_x;
                y_between -= vec_y;
            }
        }
    }
    return true;
}

// 自分が王手をしているかどうかを返す
static bool is_check(board_type* board, bool is_first) {  
    coord_type king_coord;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            if(board->field[i][j].first_ones != is_first && board->field[i][j].type == PIECE_OU){
                king_coord = (coord_type) 5 * i + j;
            }
        }
    }
    bool res = false;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            if(board->field[i][j].first_ones == is_first){
                move_type catch_king;
                catch_king.is_drop = false;
                catch_king.piece = board->field[i][j].type;
                catch_king.from = (coord_type) 5 * i + j;
                catch_king.to = king_coord;
                catch_king.do_promote = false;
                res |= able_to_move(board, catch_king, is_first);
                catch_king.do_promote = true;
                res |= able_to_move(board, catch_king, is_first);
            }
        }
    }
    return res;
}

//自分が詰みかどうかを返す
static bool is_checkmate(game_state state,  bool is_first){
    board_type board;
    make_board(state, &board);

    piece_type hand_type = PIECE_EMPTY;
    for(int i = 0; i < 11; i++){
        if(board.hand[!is_first][i] != PIECE_EMPTY){
            hand_type = board.hand[!is_first][i];
            if(hand_type != PIECE_FU){
                break;
            }
        }
    }

    int my_king_coord_x;
    int my_king_coord_y;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            if(board.field[i][j].first_ones == is_first && board.field[i][j].type == PIECE_OU){
                my_king_coord_x = i;
                my_king_coord_y = j;
            }
        }
    }

    int dx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    int dy[8] = {1, 1, 0, -1, -1, -1, 0, 1};

    if(!is_check(&board, !is_first)){
        return false;
    }else{
        for(int i = 0; i < 8; i++){
            int now_x = my_king_coord_x + dx[i];
            int now_y = my_king_coord_y + dy[i];
            if(now_x < 0 || 4 < now_x || now_y < 0 || 4 < now_y){
                continue;
            }

            game_state new_state;
            board_type new_board;

            //駒を打って王手を防げるか
            if(board.field[now_x][now_y].type == PIECE_EMPTY && hand_type != PIECE_EMPTY){
                move_type guard_move;
                guard_move.is_drop = true;
                guard_move.piece = hand_type;
                guard_move.to = now_x * 5 + now_y;
                guard_move.from = my_king_coord_x * 5 + my_king_coord_y;
                guard_move.do_promote = false;

                copy_game_state(new_state, state);
                write_move(new_state, guard_move, is_first);
                make_board(new_state, &new_board);
                if((!validate_twopawn(&new_board)) && (!is_check(&new_board, !is_first))){
                    return false;
                }
            }

            move_type king_move;
            king_move.is_drop = false;
            king_move.piece = PIECE_OU;
            king_move.to = now_x * 5 + now_y;
            king_move.from = my_king_coord_x * 5 + my_king_coord_y;
            king_move.do_promote = false;
            //王を動かして防げるか
            if(able_to_move(&board, king_move, is_first)){
                copy_game_state(new_state, state);
                write_move(new_state, king_move, is_first);
                make_board(new_state, &new_board);
                if(!is_check(&new_board, !is_first)){
                    return false;
                }
            }
        }
        return true;
    }
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
        /* // 打つ先が敵陣なのは NG (これは歩でなければおそらくセーフです、able_to_move内で処理しました)
        if (can_promote(move, is_first))
            return false;*/
        
        // 持ち駒に無い駒を打つのは NG
        if (!has_piece(&cur_board, move.piece, is_first))
            return false;
    }
    else if (!able_to_move(&cur_board, move, is_first)) // 駒の動きとして移動が可能でないなら NG
        return false;
    
    // 歩が成れるのに成っていないのは NG
    if (move.piece == PIECE_FU && can_promote(move, is_first) && is_still_unpromoted(&cur_board, move))
        return false;

    game_state next_state;
    copy_game_state(next_state, state);
    write_move(next_state, move, is_first);

    board_type next_board;
    make_board(next_state, &next_board);

    if (move.is_drop && move.piece == PIECE_FU) {
        // 二歩は NG
        if (validate_twopawn(&next_board))
            return false;
        // 打ち歩詰めは NG (歩で王手はセーフなので後で直します)
        if (is_checkmate(next_state, !is_first))
            return false;
    }

    // 相手から王手を掛けられているのに放置するのは NG
    if (is_check(&cur_board, !is_first) && is_check(&next_board, !is_first))
        return false;

    return true;
}

// 入力文字列を move_type に変換する
// 無効な文字列だった場合は false を返す。この場合 move の中身は壊れていても良い
static bool parse_move(game_state state, const char* input, move_type* move) {
    char* drop_piece_list[5] = {"FU", "GI", "KI", "KK", "HI"};

    board_type cur_board;
    make_board(state, &cur_board);

    if(strlen(input) != 4 && strlen(input) != 5) return false;


    if(input[0] < '0' || input[0] > '4') return false;
    if(input[1] < 'A' || input[1] > 'E') return false;

    for(int i = 0; i < 5; i++){
        if(strcmp(drop_piece_list[i], input + 2) == 0){
            move->is_drop = true;
            move->piece = i;
            move->from = TEGOMA;
            move->to = ((int)(input[0] - '0')) * 5 + (int)(input[1] - 'A');
            move->do_promote = false;
            return true;
        }
    }

    if(input[2] < '0' || input[2] > '4') return false;
    if(input[3] < 'A' || input[3] > 'E') return false;
    if(strlen(input) == 5 && input[4] != 'N') return false;

    move->is_drop = false;
    move->piece = cur_board.field[((int)(input[0] - '0'))][(int)(input[1] - 'A')].type;
    move->from = ((int)(input[0] - '0')) * 5 + (int)(input[1] - 'A');
    move->to = ((int)(input[2] - '0')) * 5 + (int)(input[3] - 'A');
    move->do_promote = (strlen(input) == 5 && input[4] == 'N') ? true : false;

    return true;
}

// move を表示する
static void print_move(move_type move) {
    char* drop_piece_list[5] = {"FU", "GI", "KI", "KK", "HI"};
    char move_str[6] = {};
    if(move.is_drop){
        move_str[0] = (char)(move.to / 5) + '0';
        move_str[1] = (char)(move.to % 5) + 'A';
        move_str[2] = drop_piece_list[move.piece][0];
        move_str[3] = drop_piece_list[move.piece][1];
    }
    else{
        move_str[0] = (char)(move.from / 5) + '0';
        move_str[1] = (char)(move.from % 5) + 'A';
        move_str[2] = (char)(move.to / 5) + '0';
        move_str[3] = (char)(move.to % 5) + 'A';
        move_str[4] = (move.do_promote) ? 'N' : '\0';
    }
    printf("%s\n", move_str);
}

#endif // GOGO_HEADER_MOVE_H