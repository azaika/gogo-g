#include "ai.h"
#include "ga.h"

int main() {
    const int n = 200;

    ai_seed seeds[n];
    pcg64_state rng;
    pcg64_srandom_by_time(&rng);
    for (int i = 0; i < 200; ++i) {
        ai_generate_random_seed(&seeds[n], &rng);
    }
    ga_advance_turn(n, seeds, 100);
}