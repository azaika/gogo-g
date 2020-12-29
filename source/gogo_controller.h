#ifndef GOGO_HEADER_GOGO_CONTROLLER_H
#define GOGO_HEADER_GOGO_CONTROLLER_H

#include "game_state.h"
#include "move.h"
#include "ai.h"

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct gogo_controller_tag {
	game_state* state;
	game_state_hash* history;
	ai_seed seed;
	int turn;
	int player_turn_parity;
};

typedef struct gogo_controller_tag gogo_controller;

#define TURN_LIMIT (150)

static void init_gogo(gogo_controller* gc, bool is_player_first) {
	assert(gc != NULL);

	gc->state = generate_game_state();
	gc->turn = 0;
	gc->player_turn_parity = (is_player_first ? 0 : 1);

	gc->history = (game_state_hash*)malloc(sizeof(game_state_hash) * (TURN_LIMIT + 1));
	if (gc->history == NULL) {
		fprintf(stderr, "memory allocation error in init_game()\n");
		exit(EXIT_FAILURE);
	}

	FILE* file = fopen("seed.bin", "rb");
	if (file == NULL) {
		#ifdef DEBUG
			pcg64_state* rng = (pcg64_state*)malloc(sizeof(pcg64_state));
			pcg64_srandom_by_time(rng);
			ai_generate_random_seed(&gc->seed, rng);

			free(rng);
			return;
		#else
			fprintf(stderr, "failed to open 'seed.bin'.\n");
			exit(EXIT_SUCCESS);
		#endif
	}
	// このサイズはよしなに変える
	char buf[256];
	size_t num_read = fread(buf, sizeof(uint8_t), sizeof(gc->seed.table), file);
	if (num_read != sizeof(uint8_t) * sizeof(gc->seed.table)) {
		fprintf(stderr, "failed to read 'seed.bin' (%zu bytes read).\n", num_read);
		exit(EXIT_SUCCESS);
	}

	ai_deserialize_seed(buf, &gc->seed);

	fclose(file);
}

static void destroy_gogo(gogo_controller* gc) {
	free_game_state(gc->state);
	free(gc->history);
}

static bool is_first_player_turn(gogo_controller* gc) {
	assert(gc != NULL);
	return gc->turn % 2 == gc->player_turn_parity;
}

// どちらかが王を取って勝っているかどうかを判定する
// どちらも勝ちでないなら 0
// 先手が勝ちなら 1
// 後手が勝ちなら 2
// を返す
static int check_wins(gogo_controller* gc) {
	if(is_checkmate(*(gc->state), is_first_player_turn(gc))){
		return (gc->turn % 2 == 0) ? 2 : 1;
	}else{
		return 0;
	}
}

// 勝敗が決しているかを返す
static bool advance_turn(gogo_controller* gc) {
	assert(gc != NULL);

	if (gc->turn >= TURN_LIMIT)
		return false;

	move_type move;
	if (is_first_player_turn(gc)) {
		// プレイヤーの手番
		char input[6];
		scanf("%5s", input);

		if (!parse_move(*gc->state, input, &move)) {
			#ifdef DEBUG
			fprintf(stderr, "invalid input\n");
			#endif
			printf("You Lose\n");
			return false;
		}

		if (!validate_move(*gc->state, move, is_first_player_turn(gc))) {
			#ifdef DEBUG
			fprintf(stderr, "invalid input\n");
			#endif
			printf("You Lose\n");
			return false;
		}
	}
	else {
		// cpu の手番
		move = ai_decide_move(&gc->seed, *gc->state, gc->player_turn_parity == 1);

		if (!validate_move(*gc->state, move, is_first_player_turn(gc))) {
			#ifdef DEBUG
			fprintf(stderr, "invalid move by AI\n");
			#endif
			printf("You Win\n");
			return false;
		}

		print_move(move);
	}

	write_move(*gc->state, move, is_first_player_turn(gc));
	gc->history[gc->turn] = into_hash(*gc->state);
	++gc->turn;

	int sennichite = check_sennichite(gc->history, gc->turn, is_first_player_turn(gc));
	if (sennichite) {
		printf(sennichite == 1 ? "You Win\n" : "You Lose\n");
		return false;
	}

	int wins = check_wins(gc);
	if (wins) {
		printf(wins == 1 ? "You Win\n" : "You Lose\n");
		return false;
	}

	return true;
}

#endif // GOGO_HEADER_GOGO_CONTROLLER_H
