/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#ifndef _OJRANDLIB_H
#define _OJRANDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


/* STRUCTURES */

struct _ojr_generator {
    int _status, _leftover, _id;
    int seedsize, statesize, bufsize;
    struct _ojr_algorithm *algorithm;
    void *extra;
    uint32_t *seed, *state, *buf, *bptr;
};

struct _ojr_algorithm {
    char *name;
    int seedsize, statesize, bufsize;
    void (*open)(struct _ojr_generator *g);
    void (*close)(struct _ojr_generator *g);
    void (*seed)(struct _ojr_generator *g);
    void (*reseed)(struct _ojr_generator *g);
    void (*refill)(struct _ojr_generator *g);
};

typedef struct _ojr_algorithm ojr_algorithm;
typedef struct _ojr_generator ojr_generator;

/* Array of built-in algorithms, defined in libmain.c.
 */
extern ojr_algorithm *_ojr_algorithms[];


/* PROTOTYPES */

extern int ojr_algorithm_count(void);
extern int ojr_algorithm_id(const char *name);
extern char *ojr_algorithm_name(int id);
extern int ojr_algorithm_seedsize(int id);
extern int ojr_algorithm_statesize(int id);

extern ojr_generator *ojr_new(int id);
extern void ojr_close(ojr_generator *g);

extern void _ojr_default_seed(ojr_generator *g);
extern void _ojr_default_reseed(ojr_generator *g);

extern int ojr_seed(ojr_generator *g, uint32_t *seed, int size);
extern int ojr_reseed(ojr_generator *g, uint32_t *seed, int size);
extern int ojr_get_seedsize(ojr_generator *g);
extern int ojr_get_seed(ojr_generator *g, uint32_t *seed, int size);
extern int ojr_get_algorithm(ojr_generator *g);

extern void ojr_get_system_entropy(uint32_t *dest, int dsize);

extern uint16_t ojr_next16(ojr_generator *g);
extern uint32_t ojr_next32(ojr_generator *g);
extern uint64_t ojr_next64(ojr_generator *g);
extern double ojr_next_double(ojr_generator *g);
extern double ojr_next_signed_double(ojr_generator *g);
extern double ojr_next_gaussian(ojr_generator *g);

extern int ojr_rand(ojr_generator *g, int limit);
extern void ojr_shuffle_int_array(
    ojr_generator *g, int *array, int size, int count);
extern void ojr_shuffle_pointer_array(
    ojr_generator *g, void **array, int size, int count);


/* OTHER */

#if defined(_WIN32)
#define STRCASECMP(s,t) stricmp((s),(t))
#else
#define STRCASECMP(s,t) strcasecmp((s),(t))
#endif


#ifdef __cplusplus
} /* end of extern "C" */
#endif /* __cplusplus */


#ifdef __cplusplus

#include <vector>

namespace oj {

typedef std::vector<uint32_t> Seed;

class Generator {
private:
    ojr_generator *cg;

public:
    Generator();
    Generator(int id);
    Generator(const char *name);
    ~Generator();

    static int algorithmCount();
    static int algorithmID(const char *name);
    static char *algorithmName(int id);
    static int algorithmSeedsize(int id);
    static int algorithmStatesize(int id);

    int seed(uint32_t val);
    int seed(Seed vec);
    int seed();

    int reseed(uint32_t val);
    int reseed(Seed vec);
    int reseed();

    int seedSize();
    void getSeed(Seed &os);
    int getAlgorithm();

    uint16_t next16();
    uint32_t next32();
    uint64_t next64();
    double nextDouble();
    double nextSignedDouble();
    double nextGaussian();

    int rand(int limit);
};

} /* namespace */

#endif /* __cplusplus for class definition */

#endif /* _OJRANDLIB_H */
