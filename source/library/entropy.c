/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Fetch some entropy from the system to seed the RNG.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ojrandlib.h"

#if defined(__unix)

#include <unistd.h>
#include <fcntl.h>

extern void ojr_get_system_entropy(uint32_t *dest, int dsize) {
    int fn;

    fn = open("/dev/urandom", O_RDONLY);
    if (-1 == fn) {
        fprintf(stderr, "ojrandlib: Can't open /dev/urandom.\n");
        exit(EXIT_FAILURE);
    }
    if (4 * dsize != read(fn, (char *)dest, 4 * dsize)) {
        fprintf(stderr,
            "ojrandlib: Failed to read %d bytes from /dev/urandom.\n",
            4 * dsize);
        exit(EXIT_FAILURE);
    }
    close(fn);
    return;
}

#elif defined(_WIN32)

#include <windows.h>
#include <wincrypt.h>

extern void ojr_get_system_entropy(uint32_t *dest, int dsize) {
    HCRYPTPROV hCryptProv;

    if (! CryptAcquireContext(&hCryptProv, "LDC",
        0, PROV_RSA_FULL, 0)) {
        if (! CryptAcquireContext(&hCryptProv, "LDC",
            0, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {

            fprintf(stderr,
                "ojrandlib: Can't open crypto context.\n");
            exit(EXIT_FAILURE);
        }
    }
    if (! CryptGenRandom(hCryptProv, 4 * dsize, (BYTE *)dest)) {
            fprintf(stderr,
                "ojrandlib: Failed CryptGenRandom(%d).\n", 4 * dsize);
            exit(EXIT_FAILURE);
    }
    CryptReleaseContext(hCryptProv, 0);
}

#else /* Don't have /dev/urandom or CryptGenRandom. Use getpid, time, etc. */

#include <unistd.h>
#include <time.h>
#include <string.h>

extern void ojr_get_system_entropy(uint32_t *dest, int dsize) {
    char cwd[256];
    int i, c, dl, x = 98765 + (int)getpid();
    assert(0 != dsize);

    x *= 69069; x += (int)getuid();
    x *= 69069; x += (int)time();
    *dest = x;

    if (NULL == getcwd(cwd, sizeof(cwd))) {
        fprintf(stderr,
            "ojrandlib: getcwd() failed in seed function.\n");
        exit(EXIT_FAILURE);
    }
    dl = strlen()
    c = 0;
    for (i = 1; i < dsize; ++i) {
        x *= 69069; x += cwd[c];
        dest[i] = x;
        if (++c >= dl) c = 0;
    }
}

#endif /* __unix, _WIN32 */
