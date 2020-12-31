#pragma GCC diagnostic ignored "-Wunused-function"

#include "ai.h"
#include "ga.h"

#include <stdio.h>
#include <string.h>

int main() {
    const int n = 200;

    ai_seed seeds[n];
    pcg64_state rng;
    pcg64_srandom_by_time(&rng);
    for (int i = 0; i < 200; ++i) {
        ai_generate_random_seed(&seeds[i], &rng);
    }
    
    printf("generated initial seeds.\n");

    ga_advance_turn(n, seeds, 10);

    printf("selecting top seeds...\n");

    int top10_idx[10];
    for (int i = 0; i < 10; ++i) {
        ga_do_tournament(n/10, seeds + n / 10 * i, 1, top10_idx + i);
        printf("[DONE] (%d/%d)\n", n / 10 * (i + 1), n);
    }

    ai_seed top10[10];
    for (int i = 0; i < 10; ++i) {
        ai_copy_seed(&seeds[top10_idx[i]], top10 + i);
    }

    int top = 0;
    ga_do_tournament(10, top10, 1, &top);
    
    char buf[512];
    ai_serialize_seed(&seeds[top], buf);

    size_t len = sizeof(seeds[0].table);

    FILE* file = fopen("seed.bin", "wb");
    if (file == NULL) {
        fprintf(stderr, "failed to open 'seed.bin'.\n");
        return 0;
    }

    if (fwrite(buf, sizeof(char), len, file) != len * sizeof(char)) {
        fprintf(stderr, "cannot write result to file.\n");
    }
    fclose(file);
}