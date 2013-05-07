/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Test the basic functions of the RNG code.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "ojrandlib.h"
#include "stats.h"

static char *anames[] = { "jkiss", "mt19937", "mwc256" };

static uint32_t arr01[3] = { 0x97a09aff, 0xceb5542e, 0xdb01c792 };

static uint32_t arr04[3][4] = {
    { 0xd3721d07, 0x74267b8d, 0x7a6b727c, 0xc07d6213 },
    { 0xa1dc760c, 0xba264aba, 0xff41e7d0, 0x0e39538e },
    { 0xf272d444, 0x6ee04eb8, 0x37a0f78b, 0x28e7b33f }
};

static uint32_t arr16[3][16] = {
    { 0x2ae1f7eb, 0xe45ef953, 0x5312b879, 0xca0529e0, 0x00bb1b7f, 0x42ca278a,
      0x28083f8f, 0x5bb31243, 0x9bdcca3f, 0x5009b34b, 0x902acb3c, 0xb3f48702,
      0x8fe4259c, 0x8893a37a, 0x985fa9d9, 0xf7c70dc4 },
    { 0x12578d58, 0x6946bebf, 0x9feb9569, 0x4103852c, 0x1c707efa, 0x3bbec4f2,
      0x3a69ad0b, 0x7b5a93c8, 0xc0f5fc41, 0x999f5b4f, 0x06ba9fb3, 0xd8c6ade5,
      0x4d5e1db3, 0x89a31bf1, 0xc456c629, 0x5f5235b2 },
    { 0x6c8e6a9c, 0xdaa9fda6, 0x2b7a4152, 0x15784387, 0x6964beb4, 0x87e5cfd2,
      0x554e3e24, 0x78e4d7f3, 0xbbe0351a, 0x362940ab, 0x63aa0f4a, 0x63d40b53,
      0x4da3b19e, 0x7a8418e2, 0x4e548d2f, 0xd88e0f08 }
};

int statics(void) {
    int i, j, c = ojr_algorithm_count();
    uint32_t v1, v2;
    char *name;

    if (c < 3) return 130;
    for (i = 1; i <= c; ++i) {
        name = ojr_algorithm_name(i);
        if (NULL == name) return 140;

        if (i != ojr_algorithm_id(name)) return 150;
        if ((ojr_algorithm_seedsize(i) < 1) ||
            (ojr_algorithm_seedsize(i) > ojr_algorithm_statesize(i)) ||
            (ojr_algorithm_statesize(i) < 1) ||
            (ojr_algorithm_bufsize(i) < 1)) return 155;

        if (! isalpha(*name)) return 160;
        for (j = 1; name[j]; ++j) {
            if (! isalnum(name[j])) return 170;
        }
    }
    ojr_get_system_entropy(&v1, 1);
    ojr_get_system_entropy(&v2, 1);
    if (v1 == v2) return 180;

    if (ojr_structure_size() != sizeof(ojr_generator)) return 190;
    return 0;
}

int structureaccess(void) {
    ojr_generator gen;
    uint32_t state[3], buf[4], *bp;

    memset(&gen, 0xFF, sizeof(gen));

    ojr_init(&gen);
    if ((gen.init != 0x5eed1e55) ||
        (gen.leftover != 0) || (gen.seeded != 0) ||
        (gen.algorithm != 0) || (gen.statesize != 0) || (gen.bufsize != 0) ||
        (gen.state != NULL) || (gen.buf != NULL) || (gen.bptr != NULL) ||
        (gen.next != NULL) || (gen.extra != NULL)) return 200;

    ojr_set_seeded(&gen, 1);
    if (1 != ojr_get_seeded(&gen)) return 210;

    ojr_set_algorithm(&gen, 2);
    if (2 != ojr_get_algorithm(&gen)) return 210;

    ojr_set_state(&gen, state, 3);
    if ((ojr_get_statesize(&gen) != 3) ||
        (ojr_get_state(&gen) != state)) return 220;

    ojr_set_buffer(&gen, buf, 4);
    if ((ojr_get_bufsize(&gen) != 4) ||
        (ojr_get_buffer(&gen) != buf) || (gen.buf != gen.bptr)) return 230;

    bp = ojr_get_buffer_ptr(&gen);
    ++bp;
    ojr_set_buffer_ptr(&gen, bp);
    if (bp != ojr_get_buffer_ptr(&gen)) return 235;

    ojr_set_extra(&gen, (void *)6);
    if (ojr_get_extra(&gen) != (void *)6) return 240;

    return 0;
}

int allequal(uint32_t *a, uint32_t *b, int count) {
    int i;
    for (i = 0; i < count; ++i) if (a[i] != b[i]) return 0;
    return 1;
}

int intseed(void) {
    int i, f = 0, a = ojr_rand(NULL, 3);
    uint32_t r, second[4];
    ojr_generator *g = ojr_open(anames[a]);
    ojr_int_seed(g, arr01[a]);

    for (i = 0; i < 4; ++i) {
        r = ojr_next32(g);
        if (r != arr04[a][i]) f = 10;
    }
    ojr_int_seed(g, 12345);
    for (i = 0; i < 4; ++i) second[i] = ojr_next32(g);
    if (allequal(arr04[a], second, 4)) f = 20;

    ojr_close(g);
    return f;
}

int arrayseed(void) {
    int i, f =0, a = ojr_rand(NULL, 3);
    uint32_t r, second[16], seed[4] = { 123, 234, 345, 456 };
    ojr_generator *g = ojr_open(anames[a]);
    ojr_array_seed(g, arr04[a], 4);

    for (i = 0; i < 16; ++i) {
        r = ojr_next32(g);
        if (r != arr16[a][i]) f = 30;
    }
    ojr_array_seed(g, seed, 4);
    for (i = 0; i < 16; ++i) second[i] = ojr_next32(g);
    if (allequal(arr16[a], second, 16)) f = 40;

    ojr_close(g);
    return f;
}

int goodseed(void) {
    int i, f = 0, a = ojr_rand(NULL, 3);
    uint32_t first[4], second[4];
    ojr_generator *g = ojr_open(anames[a]);

    ojr_system_seed(g);
    for (i = 0; i < 4; ++i) first[i] = ojr_next32(g);
    ojr_system_seed(g);
    for (i = 0; i < 4; ++i) second[i] = ojr_next32(g);

    if (allequal(first, second, 4)) f = 50;
    if (allequal(first, arr04[a], 4)) f = 60;
    if (allequal(second, arr04[a], 4)) f = 70;

    ojr_close(g);
    return f;
}

int outoforder(void) {
    int i, f = 0;
    ojr_generator *g1, *g2, *g3, *g4, *g5;
    uint32_t v1, v2, seed[4];

    g1 = ojr_open("mwc256");
    g2 = ojr_open("mt19937");
    g3 = ojr_open("jkiss");
    g4 = ojr_open("mwc256");
    g5 = ojr_open("jkiss");

    ojr_get_system_entropy(seed, 4);
    ojr_array_seed(g3, seed, 4);
    ojr_array_seed(g4, seed, 4);
    ojr_array_seed(g1, seed, 4);
    ojr_array_seed(g5, seed, 4);
    ojr_array_seed(g2, seed, 4);

    v1 = ojr_next32(g1);
    ojr_next32(g2);
    v2 = ojr_next32(g5);
    if (v2 != ojr_next32(g3)) f = 80;
    if (v1 != ojr_next32(g4)) f = 90;

    for (i = 0; i < 10; ++i) ojr_next32(g4);
    ojr_discard(g1, 10);
    if (ojr_next32(g1) != ojr_next32(g4)) f = 100;

    for (i = 0; i < 100; ++i) ojr_next32(g3);
    ojr_discard(g5, 100);
    if (ojr_next32(g3) != ojr_next32(g5)) f = 110;

    for (i = 0; i < 1000; ++i) ojr_next32(g1);
    ojr_discard(g4, 1000);
    if (ojr_next32(g1) != ojr_next32(g4)) f = 120;

    ojr_close(g2);
    ojr_close(g3);
    ojr_close(g1);
    ojr_close(g5);
    ojr_close(g4);

    return f;
}

int fuzz(int count) {
    int i, test, sub, f = 0;

    for (i = 0; i < count; ++i) {
        test = ojr_rand(NULL, 100);

        if (test < 30) {
            f = structureaccess();
        } else if (test < 60) {
            f = statics();
        } else if (test < 80) {
            sub = ojr_rand(NULL, 3);
            if (0 == sub) f = intseed();
            else if (1 == sub) f = arrayseed();
            else f = goodseed();
        } else {
            f = outoforder();
        }
        if (f) break;
    }
    return f;
}

int main(int argc, char *argv[]) {
    int f;

    f = fuzz(1000);
    printf("Basic functions test %sed.\n", f ? "fail" : "pass");
    if (f) printf("Error code: %d\n", f);

    return f ? EXIT_FAILURE : EXIT_SUCCESS;
}
