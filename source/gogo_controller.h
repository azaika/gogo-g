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
#define MAX_SEARCH_DEPTH (6)

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
	char buf[512];
	size_t num_read = fread(buf, sizeof(uint8_t), sizeof(gc->seed.table), file);
	if (num_read != sizeof(uint8_t) * sizeof(gc->seed.table)) {
		fprintf(stderr, "failed to read 'seed.bin' (%zu/%zu bytes read).\n", num_read, sizeof(gc->seed.table));
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

static bool is_player_first(gogo_controller* gc) {
	assert(gc != NULL);
	return gc->player_turn_parity == 0;
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

		if (!validate_move(*gc->state, move, gc->turn % 2 == 0)) {
			#ifdef DEBUG
			fprintf(stderr, "invalid input\n");
			#endif
			printf("You Lose\n");
			return false;
		}
	}
	else {
		// cpu の手番
		move = ai_decide_move(&gc->seed, *gc->state, gc->turn % 2 == 0, MAX_SEARCH_DEPTH);

		print_move(move);

		if (!validate_move(*gc->state, move, gc->turn % 2 == 0)) {
			#ifdef DEBUG
			fprintf(stderr, "invalid move by AI\n");
			#endif
			printf("You Win\n");
			return false;
		}
	}

	write_move(*gc->state, move, gc->turn % 2 == 0);
	gc->history[gc->turn] = into_hash(*gc->state);

	int sennichite = check_sennichite(gc->history, gc->turn + 1, gc->turn % 2 == 0);
	if (sennichite) {
		printf((sennichite == 1) == is_first_player_turn(gc) ? "You Win\n" : "You Lose\n");
		return false;
	}

	int wins = check_wins(*gc->state);
	if (wins) {
		printf((wins == 1) == is_first_player_turn(gc) ? "You Win\n" : "You Lose\n");
		return false;
	}
	else if (is_checkmate(*gc->state, gc->turn % 2 == 0)) {
		printf(!is_player_first(gc) ? "You Win\n" : "You Lose\n");
		return false;
	}
	else if (is_checkmate(*gc->state, gc->turn % 2 == 1)) {
		printf(is_player_first(gc) ? "You Win\n" : "You Lose\n");
		return false;
	}

	++gc->turn;

	return true;
}

#endif // GOGO_HEADER_GOGO_CONTROLLER_H
