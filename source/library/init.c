/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Library startup/shutdown code.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ojrandlib.h"


// Keep track of allocated generator objects
ojr_generator *ojr_genlist_head = NULL;

// Keep a statically-allocated default generator for simplicity.
extern ojr_algorithm ojr_algorithm_jkiss;
ojr_generator ojr_default_generator;
uint32_t ojr_default_buffer[4 + 256];


// On library load, create and seed the default generator
int ojr_library_startup(void) {
    assert(&ojr_algorithm_jkiss == ojr_algorithms[0]);
    assert(4 == ojr_algorithm_jkiss.statesize);
    assert(256 == ojr_algorithm_jkiss.bufsize);

    ojr_init(&ojr_default_generator);
    ojr_set_algorithm(&ojr_default_generator, 1);
    ojr_set_state(&ojr_default_generator, ojr_default_buffer, 4);
    ojr_set_buffer(&ojr_default_generator, ojr_default_buffer + 4, 256);
    ojr_call_open(&ojr_default_generator);

    ojr_system_seed(&ojr_default_generator);
    return 0;
}

int ojr_close_all(void) {
    int c = 0;
    ojr_generator *g = ojr_genlist_head;

    while (g) {
        ojr_genlist_head = g->next;
        if (g->state) free(g->state);
        if (g->buf) free(g->buf);
        free(g);
        g = ojr_genlist_head;
        ++c;
    }
    return c;
}

int ojr_library_shutdown(void) {

    int c = ojr_close_all();
    if (c) {
        fprintf(stderr, "ojrandlib: %d generator object%s not freed.\n",
            c, (c > 1) ? "s" : "");
    }
    return 0;
}

#ifdef _WIN32

#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved) {
    int r;

    if (DLL_PROCESS_ATTACH == reason) {
        r = ojr_library_startup();
        if (r) exit(EXIT_FAILURE);
    } else if (DLL_PROCESS_DETACH == reason) {
        ojr_library_shutdown();
    }
    return TRUE;
}

#else /* This is the Linux Way */

void _init(void) {
    int r;

    r = ojr_library_startup();
    if (r) exit(EXIT_FAILURE);
    return;
}

void _fini(void) {
    ojr_library_shutdown();
    return;
}

#endif
