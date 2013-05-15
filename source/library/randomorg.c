/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Fetch some bytes from random.org.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#if defined(_WIN32)

#include <winsock.h>

#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#endif

#include "ojrandlib.h"

static const char *fetch_request =
    "GET /integers/?num=800&min=0&max=65535&col=32&base=16&format=plain&md=new\r\n"
    "Host: www.random.org\r\nUser-Agent: OJRandLib\r\n\r\n";

static int fetch(char *text, int size) {
    int sock, r, tr = 0;
    struct hostent *host;
    struct sockaddr_in addr;

#if defined(_WIN32)
    WSADATA wsadata;
    if (SOCKET_ERROR == WSAStartup(MAKEWORD(1,1), &wsadata)) return 0;
#endif
    if (NULL == (host = gethostbyname("www.random.org"))) return 0;
    if (-1 == (sock = socket(AF_INET, SOCK_STREAM, 0))) return 0;

    do {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(80);
        addr.sin_addr.s_addr= *((unsigned long *)(host->h_addr_list[0]));
        if (-1 == connect(sock, (struct sockaddr *)&addr, sizeof(addr))) break;
        if (-1 == send(sock, fetch_request, strlen(fetch_request), 0)) break;

        while (tr < size) {
            r = recv(sock, text + tr, size - tr, 0);
            if (0 == r) break;
            tr += r;
        }
    } while (0);

#if defined(_WIN32)
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    return tr;
}

// Fetch about 4k at a time
static uint32_t buffer[400], *bptr = buffer;

static int refill(void) {
    int r, c;
    char buf[4096];
    unsigned a[32];

    r = fetch(buf, sizeof(buf));
    if (r < 4000) return 0;
    buf[r] = '\0';

    for (int i = 0; i < 25; ++i) {
        c = sscanf(buf + 160 * i,
            "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x "
            "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
            a, a+1, a+2, a+3, a+4, a+5, a+6, a+7, a+8, a+9, a+10,
            a+11, a+12, a+13, a+14, a+15, a+16, a+17, a+18, a+19,
            a+20, a+21, a+22, a+23, a+24, a+25, a+26, a+27, a+28,
            a+29, a+30, a+31);
        if (32 != c) return 0;

        for (int j = 0; j < 16; ++j) {
            buffer[16 * i + j] = (uint32_t)((a[2 * j] << 16) + a[2 * j + 1]);
        }
    }
    return 400;
}

int ojr_get_random_org(uint32_t *buf, int size) {
    int c = 0;

    for (int i = 0; i < size; ++i) {
        if (bptr == buffer) {
            if (400 != refill()) return c;
            bptr = buffer + 400;
        }
        buf[i] = *--bptr;
        ++c;
    }
    return c;
}
