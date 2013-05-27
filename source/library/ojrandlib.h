/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
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

/* Main working generator object
 */
struct _ojr_generator {
    int init;       // Initialization and version check
    int leftover;   // Used by next16()
    int flags;
    int algorithm;  // 1-based index
    int statesize;  // Size of generator state in 32-bit words
    int bufsize;    // Size of output buffer
    uint32_t *state;    // Generator state, allocated at runtime
    uint32_t *buf;      // Output buffer, allocated at runtime
    uint32_t *bptr;     // Points to address *after* next output word
                        // bptr = buf means empty buffer
    struct _ojr_generator *next;    // For list of allocated generators
    void *extra;        // For miscellaneous client use
    void *padding[4];   // Guarding against ABI change
};

// Flags
#define OJRF_SEEDED 0x01

/* Algorithm description. Should be immutable.
 */
struct _ojr_algorithm {
    char *name;         // Simple text identifier
    int seedsize;       // Recommended, in 32-bit words. Must be <= statesize
    int statesize;      // Total size of state, in words
    int bufsize;        // Size of output buffer

    // Functions that implement the generator
    void (*open)(struct _ojr_generator *);
    void (*close)(struct _ojr_generator *);
    void (*seed)(struct _ojr_generator *, uint32_t *, int);
    void (*reseed)(struct _ojr_generator *, uint32_t *, int);
    void (*refill)(struct _ojr_generator *);

    void *extra;
    void *padding[4];
};

typedef struct _ojr_algorithm ojr_algorithm;
typedef struct _ojr_generator ojr_generator;


/* GLOBALS */

// Defined in algorithms.c
extern ojr_algorithm *ojr_algorithms[];

// Defined in init.c
extern ojr_generator *ojr_genlist_head;
extern ojr_generator ojr_default_generator;

#define DEFGEN (&ojr_default_generator)

#if defined(STATICLIB)
#  define OJRSTARTUP() ojr_library_startup()
#  define OJRSHUTDOWN() ojr_library_shutdown()
#else
#  define OJRSTARTUP()
#  define OJRSHUTDOWN()
#endif

/* PROTOTYPES */

/* Basic C API
 */
extern ojr_generator *ojr_open(const char *);
extern void ojr_close(ojr_generator *);
extern void ojr_system_seed(ojr_generator *);
extern void ojr_network_seed(ojr_generator *);
extern void ojr_int_seed(ojr_generator *, int);
extern void ojr_array_seed(ojr_generator *, uint32_t *, int);
extern void ojr_reseed(ojr_generator *, uint32_t *, int);

extern void ojr_get_system_entropy(uint32_t *, int);
extern int ojr_get_random_org(uint32_t *, int);
extern int ojr_algorithm_count(void);
extern char *ojr_algorithm_name(int);

extern uint16_t ojr_next16(ojr_generator *);
extern uint32_t ojr_next32(ojr_generator *);
extern uint64_t ojr_next64(ojr_generator *);
extern double ojr_next_double(ojr_generator *);
extern double ojr_next_signed_double(ojr_generator *);

extern double ojr_next_exponential(ojr_generator *);
extern double ojr_next_normal(ojr_generator *);

extern int ojr_rand(ojr_generator *, int);
extern void ojr_discard(ojr_generator *, int);

extern void ojr_shuffle_int_array(ojr_generator *, int *, int, int);
extern void ojr_shuffle_pointer_array(ojr_generator *, void **, int, int);

/* Internal structure access, mostly for use by language bindings.
 */
extern int ojr_algorithm_id(const char *);
extern int ojr_algorithm_seedsize(int);
extern int ojr_algorithm_statesize(int);
extern int ojr_algorithm_bufsize(int);

extern int ojr_structure_size(void);
extern void ojr_init(ojr_generator *);
extern int ojr_get_seeded(ojr_generator *);
extern void ojr_set_seeded(ojr_generator *, int);

extern int ojr_get_algorithm(ojr_generator *);
extern void ojr_set_algorithm(ojr_generator *, int);

extern int ojr_get_statesize(ojr_generator *);
extern uint32_t *ojr_get_state(ojr_generator *);
extern void ojr_set_state(ojr_generator *, uint32_t *, int);

extern int ojr_get_bufsize(ojr_generator *);
extern uint32_t *ojr_get_buffer(ojr_generator *);
extern uint32_t *ojr_get_buffer_ptr(ojr_generator *);
extern void ojr_set_buffer(ojr_generator *, uint32_t *, int);
extern void ojr_set_buffer_ptr(ojr_generator *, uint32_t *);

extern void *ojr_get_extra(ojr_generator *);
extern void ojr_set_extra(ojr_generator *, void *);

extern void ojr_call_open(ojr_generator *);
extern void ojr_call_close(ojr_generator *);
extern void ojr_call_seed(ojr_generator *, uint32_t *, int);
extern void ojr_call_reseed(ojr_generator *, uint32_t *, int);
extern void ojr_call_refill(ojr_generator *);

extern void ojr_default_reseed(ojr_generator *, uint32_t *, int);
extern void ojr_default_seed(ojr_generator *, uint32_t *, int);


#ifdef __cplusplus
} /* end of extern "C" */
#endif /* __cplusplus */


// Macro versions of some generator functions. Not recommended
// for client use, as they eliminate error checking.

#define OJR_NEXT32(g) ((((g)->bptr == (g)->buf) ? ( \
ojr_call_refill(g), (g)->bptr = (g)->buf + (g)->bufsize \
) : 0), *--(g)->bptr)

#define OJR_NEXT64(g) ((((g)->bptr - (g)->buf) >= 2) \
? ((g)->bptr -= 2, *(uint64_t *)((g)->bptr)) \
: (((uint64_t)(ojr_next32(g)) << 32) | ojr_next32(g)))


/* C++ declarations */

#ifdef __cplusplus
#include <vector>
namespace oj {

typedef std::vector<uint32_t> Seed;

int algorithmCount(void);
char *algorithmName(int);
void getSystemEntropy(Seed &, int);
void getRandomOrg(Seed &, int);

uint16_t next16(void);
uint32_t next32(void);
uint64_t next64(void);
double nextDouble(void);
double nextSignedDouble(void);
double nextNormal(void);
double nextExponential(void);
int rand(int);


class Generator {
private:
    ojr_generator *cg;
    void _init(int);

public:
    Generator(void);
    Generator(const char *);
    ~Generator(void);

    void seed(Seed);
    void seed(int);
    void seed(void);
    void reseed(Seed);

    uint16_t next16(void);
    uint32_t next32(void);
    uint64_t next64(void);
    double nextDouble(void);
    double nextSignedDouble(void);
    double nextNormal(void);
    double nextExponential(void);

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
