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
        ai_generate_random_seed(&seeds[n], &rng);
    }
    ga_advance_turn(n, seeds, 100);

    int top[1];
    ga_do_tournament(200, seeds, 1, top);
    
    char buf[256];
    ai_serialize_seed(&seeds[top[0]], buf);

    size_t len = strlen(buf);

    FILE* file = fopen("result.bin", "wb");
    if (file == NULL) {
        fprintf(stderr, "failed to open 'result.bin'.\n");
        return 0;
    }

    if (fwrite(buf, sizeof(char), len, file) != len * sizeof(char)) {
        fprintf(stderr, "cannot write result to file.\n");
    }
    fclose(file);
}