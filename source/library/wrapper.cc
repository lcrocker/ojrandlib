/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * C++ Wrapper.
 */

#include <cstdlib>
#include <cstring>

#include "ojrandlib.h"

namespace oj {

int algorithmCount() { return ojr_algorithm_count(); }
char *algorithmName(int id) { return ojr_algorithm_name(id); }

void getSystemEntropy(Seed &vec, int count) {
    vec.resize(count);
    ojr_get_system_entropy(vec.data(), count);
}

void getRandomOrg(Seed &vec, int count) {
    vec.resize(count);
    ojr_get_random_org(vec.data(), count);
}

uint16_t next16(void){ return ojr_next16(DEFGEN); }
uint32_t next32(void) { return ojr_next32(DEFGEN); }
uint64_t next64(void) { return ojr_next64(DEFGEN); }
double nextDouble(void) { return ojr_next_double(DEFGEN); }
double nextSignedDouble(void) { return ojr_next_signed_double(DEFGEN); }
double nextNormal(void) { return ojr_next_normal(DEFGEN); }
double nextExponential(void) { return ojr_next_exponential(DEFGEN); }
int rand(int limit) { return ojr_rand(DEFGEN, limit); }

void Generator::_init(int id) {
    if (0 == id) id = 1;
    ojr_algorithm *ap = ojr_algorithms[id - 1];

    this->cg = new ojr_generator;
    ojr_init(this->cg);
    ojr_set_algorithm(this->cg, id);
    ojr_set_state(this->cg, new uint32_t[ap->statesize], ap->statesize);
    ojr_set_buffer(this->cg, new uint32_t[ap->bufsize], ap->bufsize);
    ojr_call_open(this->cg);
    ojr_system_seed(this->cg);
}

Generator::Generator(const char *name) {
    Generator::_init(ojr_algorithm_id(name));
}
Generator::Generator() { Generator::_init(1); }

Generator::~Generator() {
    ojr_call_close(this->cg);
    delete this->cg->buf;
    delete this->cg->state;
    delete this->cg;
}

void Generator::seed(Seed v) { ojr_array_seed(this->cg, v.data(), v.size()); }
void Generator::seed(int val) { ojr_int_seed(this->cg, val); }
void Generator::seed() { ojr_system_seed(this->cg); }
void Generator::reseed(Seed v) { ojr_reseed(this->cg, v.data(), v.size()); }

uint16_t Generator::next16() { return ojr_next16(this->cg); }
uint32_t Generator::next32() { return ojr_next32(this->cg); }
uint64_t Generator::next64() { return ojr_next64(this->cg); }

double Generator::nextDouble() { return ojr_next_double(this->cg); }
double Generator::nextSignedDouble() { return ojr_next_signed_double(this->cg); }
double Generator::nextNormal() { return ojr_next_normal(this->cg); }
double Generator::nextExponential() { return ojr_next_exponential(this->cg); }

int Generator::rand(int limit) { return ojr_rand(this->cg, limit); }
void Generator::discard(int count) { ojr_discard(this->cg, count); }

} /* namespace */
