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
    int _status;        /* Ititialization check and status. 0xb1e55ed0 means
                         * struct is valid but unseeded. 0xb1e55ed2 means seeded,
                         * 0xb1e55ed3 means re-seeded.
                         */
    int algorithm;      /* ID of algorithm used */
    int seedsize;       /* Number of 32-bit words in seed array */
    int statesize;      /* 32-bit words in generator state */
    int bufsize;        /* 32-bit words in output buffer */
    int _leftover;      /* Used by next16() */
    struct _ojr_algorithm *ap;  /* Pointer to algorithm info structure */
    void *extra;        /* For use by caller */
    uint32_t *seed;     /* Seed array */
    uint32_t *state;    /* Generator state */
    uint32_t *buf;      /* Output buffer */
    uint32_t *bptr;     /* Points to ** after ** next word to output.
                         * bptr == buf means buffer is empty.
                         */
};

struct _ojr_algorithm {
    char *name;         /* Name of algorithm (lowercase, but not sensitive) */
    int seedsize;       /* Suggested size (in 32-bit words) of seed array */
    int statesize;      /* Size in words of generator state */
    int bufsize;        /* Size of output buffer. */

    /* Initialize the gnerator after allocating buffers, before seeding */
    void (*open)(struct _ojr_generator *);

    /* Release any resources generator may have grabbed */
    void (*close)(struct _ojr_generator *);

    /* Apply seed array to generator state */
    void (*seed)(struct _ojr_generator *);

    /* Reseed generator state with given int */
    void (*reseed)(struct _ojr_generator *, int);

    /* Give us a bufferfull of random bits */
    void (*refill)(struct _ojr_generator *);
};

typedef struct _ojr_algorithm ojr_algorithm;
typedef struct _ojr_generator ojr_generator;

/* Array of built-in algorithms, defined in libmain.c.
 */

extern ojr_algorithm *_ojr_algorithms[];


/* PROTOTYPES */

extern int ojr_algorithm_count(void);
extern int ojr_algorithm_id(const char *);
extern char *ojr_algorithm_name(int);
extern int ojr_algorithm_seedsize(int);
extern int ojr_algorithm_statesize(int);
extern int ojr_algorithm_bufsize(int);

extern int _ojr_structure_size(void);
extern void _ojr_init(ojr_generator *);
extern int _ojr_get_status(ojr_generator *);
extern void _ojr_set_status(ojr_generator *, int);
extern int _ojr_get_seedsize(ojr_generator *);
extern uint32_t *_ojr_get_seed(ojr_generator *);
extern int _ojr_get_statesize(ojr_generator *);
extern uint32_t *_ojr_get_state(ojr_generator *);
extern int _ojr_get_bufsize(ojr_generator *);
extern uint32_t *_ojr_get_buffer(ojr_generator *);
extern uint32_t *_ojr_get_buffer_ptr(ojr_generator *);
extern int _ojr_get_algorithm(ojr_generator *);

extern void _ojr_set_seed(ojr_generator *, uint32_t *, int);
extern void _ojr_set_state(ojr_generator *, uint32_t *, int);
extern void _ojr_set_buffer(ojr_generator *, uint32_t *, int);
extern void _ojr_set_buffer_ptr(ojr_generator *, uint32_t *);
extern void _ojr_set_algorithm(ojr_generator *, int);

extern void _ojr_call_open(ojr_generator *);
extern void _ojr_call_close(ojr_generator *);
extern void _ojr_call_seed(ojr_generator *);
extern void _ojr_call_reseed(ojr_generator *, int);
extern void _ojr_call_refill(ojr_generator *);

extern ojr_generator *ojr_open(int);
extern void ojr_close(ojr_generator *);
extern void ojr_get_system_entropy(uint32_t *, int);

extern void ojr_default_reseed(ojr_generator *g, int);
extern void ojr_default_seed(ojr_generator *g);

extern int ojr_int_seed(ojr_generator *, int);
extern int ojr_good_seed(ojr_generator *);
extern int ojr_array_seed(ojr_generator *, uint32_t *, int);
extern void ojr_reseed(ojr_generator *);

extern int ojr_get_seedsize(ojr_generator *);
extern int ojr_get_seed(ojr_generator *, uint32_t *, int);
extern int ojr_get_algorithm(ojr_generator *);
extern void *ojr_get_extra(ojr_generator *);
extern void ojr_set_extra(ojr_generator *, void *);

extern uint16_t ojr_next16(ojr_generator *);
extern uint32_t ojr_next32(ojr_generator *);
extern uint64_t ojr_next64(ojr_generator *);
extern double ojr_next_double(ojr_generator *);
extern double ojr_next_signed_double(ojr_generator *);
extern double ojr_next_gaussian(ojr_generator *);

extern int ojr_rand(ojr_generator *, int);
extern void ojr_discard(ojr_generator *, int);
extern void ojr_shuffle_int_array(ojr_generator *, int *, int, int);
extern void ojr_shuffle_pointer_array(ojr_generator *, void **, int, int);


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

int algorithmCount(void);
int algorithmID(const char *);
char *algorithmName(int);
int algorithmSeedSize(int);
int algorithmStateSize(int);
int algorithmBufSize(int);

void getSystemEntropy(Seed &, int);


class Generator {
private:
    ojr_generator *cg;
    void _init(int);
    int _copy_seed(uint32_t *, int);

public:
    Generator(void);
    Generator(int);
    Generator(const char *);
    ~Generator(void);

    int seed(uint32_t);
    int seed(Seed);
    int seed(void);
    void reseed(void);

    void getSeed(Seed &);
    int getAlgorithm(void);
    void *getExtra(void);
    void setExtra(void *);

    uint16_t next16(void);
    uint32_t next32(void);
    uint64_t next64(void);
    double nextDouble(void);
    double nextSignedDouble(void);
    double nextGaussian(void);

    int rand(int);
    void discard(int);

    template<typename T>
    void shuffle(std::vector<T> &vec, int count) {
        int i, r, size = vec.size();
        T t;

        if (size < 2) return;
        if (count == size) --count;

        for (i = 0; i < count; ++i) {
            r = ojr_rand(this->cg, size - i);
            t = vec[i]; vec[i] = vec[i + r]; vec[i + r] = t;
        }
    }

    template<typename T>
    void shuffle(std::vector<T> &vec) {
        this->shuffle(vec, vec.size());
    }
};

} /* namespace */

#endif /* __cplusplus for class definition */

#endif /* _OJRANDLIB_H */
