#ifndef GOGO_HEADER_GA_H
#define GOGO_HEADER_GA_H

#include "random.h"
#include "ai.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

//  1 なら s1 の勝ち
//  0 なら引き分け
// -1 なら s2 の勝ち
static int ga_battle(ai_seed* s1, ai_seed* s2, bool is_s1_first) {
    const int TURN_LIMIT = 150;

    if (!is_s1_first) {
        ai_seed* tmp = s1;
        s1 = s2;
        s2 = tmp;
    }

    game_state* state = generate_game_state();
    game_state_hash* history = (game_state_hash*)malloc(sizeof(game_state_hash) * (TURN_LIMIT + 1));
	if (history == NULL) {
		fprintf(stderr, "memory allocation error in ga_battle()\n");
		exit(EXIT_FAILURE);
	}

    int turn = 0;
    move_type move;
    while (turn < TURN_LIMIT) {
        move = ai_decide_move((turn % 2 == 0 ? s1 : s2), *state, turn % 2 == 0);

        write_move(*state, move, turn % 2 == 0);
        history[turn] = into_hash(*state);
        ++turn;

        int sennichite = check_sennichite(history, turn, turn % 2 == 0);
        if (sennichite) {
            free_game_state(state);
            return ((sennichite == 1) == is_s1_first ? 1 : -1);
        }

        if(is_checkmate(*state, turn % 2 == 0)) {
            free_game_state(state);
		    return ((turn % 2 == 0) == is_s1_first ? -1 : 1);
	    }
    }

    free_game_state(state);
    return 0;
}

// size 個で総当りトーナメントを行い, 上位 k 個を選出
static void ga_do_tournament(int size, ai_seed challengers[], int k, int winners[]) {
    int* points = (int*)malloc(size * sizeof(int));
    if (points == NULL) {
        fprintf(stderr, "memory allocation error in ga_do_tournament()");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            int res = ga_battle(&challengers[i], &challengers[j], (uint32_t)time(NULL) % 2);

            points[i] += res;
            points[j] -= res;
        }
    }

    int* top = (int*)malloc(k * sizeof(int));
    if (top == NULL) {
        fprintf(stderr, "memory allocation error in ga_do_tournament()");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < k; ++i) {
        top[i] = -size * size;
    }

    for (int i = 0; i < size; ++i) {
        if (points[i] <= top[k - 1]) {
            continue;
        }

        top[k - 1] = points[i];
        winners[k - 1] = i;

        for (int j = k - 1; j >= 1 && points[j] > points[j - 1]; --j) {
            int p = points[j];
            int idx = winners[j];

            points[j] = points[j - 1];
            points[j - 1] = p;

            winners[j] = winners[j - 1];
            winners[j - 1] = idx;
        }
    }
    
    free(top);
    free(points);
}

static void random_select(int n, ai_seed from[], int k, ai_seed result[], pcg64_state* rng) {
    static bool used[1024];
    memset(used, 0, sizeof(used));

    assert(n >= k);
    assert(n <= 1024);

    for (int i = 0; i < k; ++i) {
        int r = pcg64_random(rng) % (n - k);
        int cur = 0;
        while (r != 0) {
            if (!used[cur]) {
                --r;
            }
            ++cur;
        }

        ai_copy_seed(&from[cur], &result[i]);
        used[cur] = true;
    }
}

static void ga_cross_seeds(const ai_seed* s1, const ai_seed* s2, ai_seed* res, pcg64_state* rng) {
    // ToDo: implement
}

static void ga_mutate_seed(ai_seed* seed, pcg64_state* rng) {
    // ToDo: implement
}

static void ga_select_next(int n, ai_seed cur[], ai_seed next[], pcg64_state* rng) {
    const int num_tops = 10;
    const int tournament_size = 20;

    assert(n <= 1024 && n % num_tops == 0);

    int winners[num_tops];

    int num_adopted = 0;

    static ai_seed candidate[1024];
    for (int i = 0; i < n / num_tops; ++i) {
        random_select(n, cur, tournament_size, candidate, rng);
        ga_do_tournament(20, candidate, num_tops, winners);

        // 1 位と 2 位はそのまま採用
        ai_copy_seed(&cur[winners[0]], &next[i * num_tops]);
        ai_copy_seed(&cur[winners[1]], &next[i * num_tops + 1]);

        // 3-5 4-5 の組み合わせ以外は交配して採用
        int num_written = 2;
        for (int j = 0; j < num_tops; ++i) {
            for (int k = j + 1; k < num_tops && j + k <= 5; ++k) {
                ga_cross_seeds(&cur[winners[j]], &cur[winners[k]], &next[i * num_tops + num_written], rng);
                ++num_written;
            }
        }

        // 1 位以外は突然変異しうる
        for (int j = 1; j < num_tops; ++j) {
            ga_mutate_seed(&next[j], rng);
        }
    }
}

// N は数百程度を想定
static void ga_advance_turn(int n, ai_seed seeds[], int turn) {
    ai_seed* buf = (ai_seed*)malloc(n * sizeof(ai_seed));

    pcg64_state* rng = (pcg64_state*)malloc(sizeof(pcg64_state));
    pcg64_srandom_by_time(rng);

    if (buf == NULL) {
        fprintf(stderr, "memory allocation error in ga_advance_turn()");
        exit(EXIT_FAILURE);
    }

    ai_seed* ptr[2] = {seeds, buf};
    for (int i = 0; i < turn; ++i) {
        ga_select_next(n, ptr[i % 2], ptr[(i + 1) % 2], rng);
    }

    if (turn % 2 == 1) {
        memcpy(seeds, buf, n * sizeof(ai_seed));
    }

    free(buf);
}

#endif // GOGO_HEADER_GA_H