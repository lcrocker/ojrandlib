/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
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
int algorithmID(const char *name) { return ojr_algorithm_id(name); }
char *algorithmName(int id) { return ojr_algorithm_name(id); }
int algorithmSeedSize(int id) { return ojr_algorithm_seedsize(id); }
int algorithmStateSize(int id) { return ojr_algorithm_statesize(id); }
int algorithmBufSize(int id) { return ojr_algorithm_bufsize(id); }

void getSystemEntropy(Seed &vec, int count) {
    vec.resize(count);
    ojr_get_system_entropy(vec.data(), count);
}

void Generator::_init(int id) {
    if (0 == id) id = 1;
    ojr_algorithm *ap = _ojr_algorithms[id - 1];

    this->cg = new ojr_generator;
    _ojr_init(this->cg);
    _ojr_set_algorithm(this->cg, id);
    _ojr_set_state(this->cg, new uint32_t[ap->statesize], ap->statesize);
    _ojr_set_buffer(this->cg, new uint32_t[ap->bufsize], ap->bufsize);
    _ojr_call_open(this->cg);
}

Generator::Generator(int id) { Generator::_init(id); }
Generator::Generator(const char *name) { Generator::_init(oj::algorithmID(name)); }
Generator::Generator() { Generator::_init(1); }

Generator::~Generator() {
    _ojr_call_close(this->cg);
    if (this->cg->seed) delete this->cg->seed;
    delete this->cg->buf;
    delete this->cg->state;
}

int Generator::_copy_seed(uint32_t *sp, int size) {
    int first = (NULL == this->cg->seed);
    _ojr_set_seed(this->cg, sp, size);
    _ojr_call_seed(this->cg);

    if (first) this->cg->_status = 0xb1e55ed2;
    else this->cg->_status = 0xb1e55ed3;
    return size;
}

int Generator::seed(Seed v) {
    int s = v.size();
    uint32_t *sp = new uint32_t[s];
    memmove(sp, v.data(), 4 * s);
    return Generator::_copy_seed(sp, s);
}

int Generator::seed() {
    int s = this->cg->ap->seedsize;
    uint32_t *sp = new uint32_t[s];
    ojr_get_system_entropy(sp, s);
    return Generator::_copy_seed(sp, s);
}

int Generator::seed(uint32_t val) {
    uint32_t *sp = new uint32_t[1];
    *sp = val;
    return Generator::_copy_seed(sp, 1);
}

void Generator::reseed() {
    uint32_t val = 0xFFFFFFFF;
    ojr_get_system_entropy(&val, 1);
    _ojr_call_reseed(this->cg, val);
    this->cg->_status = 0xb1e55ed3;
}

void Generator::getSeed(Seed &os) {
    if (0 == this->cg->seed) return;
    os.assign(this->cg->seed, this->cg->seed + this->cg->seedsize);
}
int Generator::getAlgorithm() { return _ojr_get_algorithm(this->cg); }

uint16_t Generator::next16() { return ojr_next16(this->cg); }
uint32_t Generator::next32() { return ojr_next32(this->cg); }
uint64_t Generator::next64() { return ojr_next64(this->cg); }

double Generator::nextDouble() { return ojr_next_double(this->cg); }
double Generator::nextSignedDouble() { return ojr_next_signed_double(this->cg); }
double Generator::nextGaussian() { return ojr_next_gaussian(this->cg); }

int Generator::rand(int limit) { return ojr_rand(this->cg, limit); }
void Generator::discard(int count) { ojr_discard(this->cg, count); }

} /* namespace */
