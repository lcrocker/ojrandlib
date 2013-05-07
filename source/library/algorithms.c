/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Algorithm list and info.
 */

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <assert.h>

#include "ojrandlib.h"


/* This is the global list of available algorithms. Each algorithm source file
 * must contain a structure whose name is public, and these are listed here.
 */
extern ojr_algorithm ojr_algorithm_jkiss;
extern ojr_algorithm ojr_algorithm_mt19937;
extern ojr_algorithm ojr_algorithm_mwc256;

ojr_algorithm *ojr_algorithms[] = {
    &ojr_algorithm_jkiss,
    &ojr_algorithm_mt19937,
    &ojr_algorithm_mwc256,
    NULL,
};


static int _ojr_algorithm_count = 0;

int ojr_algorithm_count(void) {
    if (0 == _ojr_algorithm_count) {
        ojr_algorithm **aap = ojr_algorithms;
        while (NULL != *aap) { ++aap; ++_ojr_algorithm_count; }
    }
    return _ojr_algorithm_count;
}

char *ojr_algorithm_name(int id) {
    if (id < 0 || id > ojr_algorithm_count()) return NULL;
    if (0 == id) id = 1;
    return (ojr_algorithms[id - 1])->name;
}

int ojr_algorithm_id(const char *name) {
    int i, c = ojr_algorithm_count();
    if (NULL == name) return 0;

    for (i = 0; i < c; ++i) {
        if (0 == STRCASECMP(name, (ojr_algorithms[i])->name)) {
            return i + 1;
        }
    }
    return 0;
}

int ojr_algorithm_seedsize(int id) {
    assert(id >= 0 && id <= ojr_algorithm_count());
    if (0 == id) id = 1;
    return (ojr_algorithms[id - 1])->seedsize;
}

int ojr_algorithm_statesize(int id) {
    assert(id >= 0 && id <= ojr_algorithm_count());
    if (0 == id) id = 1;
    return (ojr_algorithms[id - 1])->statesize;
}

int ojr_algorithm_bufsize(int id) {
    assert(id >= 0 && id <= ojr_algorithm_count());
    if (0 == id) id = 1;
    return (ojr_algorithms[id - 1])->bufsize;
}
