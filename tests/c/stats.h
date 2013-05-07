/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int n;
    double mn, mx, r;
    long total;
    long *counts;
} counter;

counter *counter_open(int n, double mn, double mx);
void counter_close(counter *c);
void counter_inc_b(counter *c, int b);
void counter_inc_f(counter *c, double v);
double counter_print_stats(counter *c);
