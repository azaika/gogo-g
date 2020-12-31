#ifndef GOGO_HEADER_GA_H
#define GOGO_HEADER_GA_H

#include "random.h"
#include "ai.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

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
        move = ai_decide_move((turn % 2 == 0 ? s1 : s2), *state, turn % 2 == 0, 3);

        write_move(*state, move, turn % 2 == 0);
        history[turn] = into_hash(*state);

        int sennichite = check_sennichite(history, turn + 1, turn % 2 == 0);
        if (sennichite) {
            free_game_state(state);
            return ((sennichite == 1) == is_s1_first ? 1 : -1);
        }

        int wins = check_wins(*state);
	    if (wins) {
            free_game_state(state);
	    	return ((wins == 1) == is_s1_first ? 1 : -1);
	    }
	    else if (is_checkmate(*state, turn % 2 == 0)) {
            free_game_state(state);
            return (!is_s1_first ? 1 : -1);
	    }
	    else if (is_checkmate(*state, turn % 2 == 1)) {
            free_game_state(state);
	    	return (is_s1_first ? 1 : -1);
	    }

        ++turn;
    }

    free_game_state(state);
    return 0;
}

// size 個で総当りトーナメントを行い, 上位 k 個を選出
static void ga_do_tournament(int size, ai_seed challengers[], int k, int winners[]) {
    int* points = (int*)malloc(size * sizeof(int));
    if (points == NULL) {
        fprintf(stderr, "memory allocation error in ga_do_tournament()\n");
        exit(EXIT_FAILURE);
    }

    memset(points, 0, sizeof(int) * size);

    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            int res = ga_battle(&challengers[i], &challengers[j], (uint32_t)time(NULL) % 2);
            printf("battle finished! (%d, %d) = %d\n", i, j, res);

            points[i] += res;
            points[j] -= res;
        }
    }

    printf("all battle finished!\n");

    int* top = (int*)malloc(k * sizeof(int));
    if (top == NULL) {
        fprintf(stderr, "memory allocation error in ga_do_tournament()\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < k; ++i) {
        top[i] = -5 * size * size;
    }

    for (int i = 0; i < size; ++i) {
        if (points[i] <= top[k - 1]) {
            continue;
        }

        top[k - 1] = points[i];
        winners[k - 1] = i;

        for (int j = k - 1; j >= 1 && top[j] > top[j - 1]; --j) {
            int p = top[j];
            int idx = winners[j];

            top[j] = top[j - 1];
            top[j - 1] = p;

            winners[j] = winners[j - 1];
            winners[j - 1] = idx;
        }
    }

    printf("tournament finished!\n");
    
    free(top);
    free(points);
}

// O(k^2)
static void random_select(int n, int k, int result[], pcg64_state* rng) {
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

        result[i] = cur;
        used[cur] = true;
    }
}

static uint8_t ga_clamp(int x) {
    return (
        x >= 256 ? 255 :
        x < 0 ? 0 :
        x
    );
}

static void ga_cross_seeds(const ai_seed* s1, const ai_seed* s2, ai_seed* res, pcg64_state* rng) {
    assert(sizeof(s1->table) <= 512);

    int size = sizeof(s1->table);
    static int from_s2[512];
    random_select(size, size / 2, from_s2, rng);
    
    ai_copy_seed(s1, res);

    uint8_t* t = (uint8_t*)(res->table);
    for (int i = 0; i < size / 2; ++i) {
        t[from_s2[i]] = ((uint8_t*)(s2->table))[from_s2[i]];
    }

    for (int i = 0; i < size; ++i) {
        t[i] = ga_clamp(t[i] + (int)(pcg64_random(rng) % 20) - 10);
    }
}

static void ga_mutate_seed(ai_seed* seed, pcg64_state* rng) {
    // 1/30 の確率
    if (pcg64_random(rng) % 30 != 0) {
        return;
    }

    int kind = pcg64_random(rng) % 8;
    int size = sizeof(seed->table[kind]);
    uint8_t* t = (uint8_t*)seed->table[kind];
    for (int i = 0; i < size; ++i) {
        t[i] = pcg64_random(rng);
    }
}

// トーナメント方式を繰り返して次世代の遺伝子を選ぶ
static void ga_select_next(int n, const ai_seed cur[], ai_seed next[], pcg64_state* rng) {
    const int num_tops = 7;
    const int tournament_size = 20;

    assert(n <= 512);

    int winners[num_tops];

    static ai_seed cand[512];
    static int cand_idx[512];
    for (int i = 0; i < n / tournament_size; ++i) {
        random_select(n, tournament_size, cand_idx, rng);
        for (int j = 0; j < tournament_size; ++j) {
            ai_copy_seed(&cur[cand_idx[i]], &cand[i]);
        }

        ga_do_tournament(tournament_size, cand, num_tops, winners);

        // 1 ～ 3 位はそのまま採用
        ai_copy_seed(&cur[winners[0]], &next[i * tournament_size]);
        ai_copy_seed(&cur[winners[1]], &next[i * tournament_size + 1]);
        ai_copy_seed(&cur[winners[2]], &next[i * tournament_size + 2]);
        
        // 4-7, 5-6, 5-7, 6-7 の組み合わせ以外は交配して採用
        int num_written = 3;
        for (int j = 0; j < num_tops; ++j) {
            for (int k = j + 1; k < num_tops && j + k < 9; ++k) {
                ga_cross_seeds(&cur[winners[j]], &cur[winners[k]], &next[i * tournament_size + num_written], rng);
                ++num_written;
            }
        }

        printf("completed seeds crossing!\n");

        // 1 位以外は突然変異しうる
        for (int j = 1; j < tournament_size; ++j) {
            ga_mutate_seed(&next[j], rng);
        }

        printf("completed seeds mutating!\n");
    }
}

// N は数百程度を想定
static void ga_advance_turn(int n, ai_seed seeds[], int turn) {
    ai_seed* buf = (ai_seed*)malloc(n * sizeof(ai_seed));
    if (buf == NULL) {
        fprintf(stderr, "memory allocation error in ga_advance_turn()\n");
        exit(EXIT_FAILURE);
    }

    pcg64_state rng;
    pcg64_srandom_by_time(&rng);

    ai_seed* ptr[2] = { seeds, buf };
    for (int i = 0; i < turn; ++i) {
        printf("[begin] turn %d\n", i);
        ga_select_next(n, ptr[i % 2], ptr[(i + 1) % 2], &rng);
        printf("[DONE] turn %d\n", i);
    }

    if (turn % 2 == 1) {
        memcpy(seeds, buf, n * sizeof(ai_seed));
    }

    free(buf);
}

#endif // GOGO_HEADER_GA_H