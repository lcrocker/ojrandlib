/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * C++ Wrapper.
 */

#include <cstdlib>
#include "ojrandlib.h"

namespace oj {

int Generator::algorithmCount() { return ojr_algorithm_count(); }
int Generator::algorithmID(const char *name) { return ojr_algorithm_id(name); }
char *Generator::algorithmName(int id) { return ojr_algorithm_name(id); }
int Generator::algorithmSeedsize(int id) { return ojr_algorithm_seedsize(id); }
int Generator::algorithmStatesize(int id) { return ojr_algorithm_statesize(id); }

Generator::Generator() { this->cg = ojr_new(NULL); }
Generator::Generator(int id) { this->cg = ojr_new(id); }
Generator::Generator(const char *name) {
	this->cg = ojr_new(Generator::algorithmID(name));
}
Generator::~Generator() { ojr_close(this->cg); }

int Generator::seed() { return ojr_seed(this->cg, NULL, 0); }
int Generator::seed(uint32_t val) {	return ojr_seed(this->cg, &val, 1); }
int Generator::seed(Seed v) { return ojr_seed(this->cg, v.data(), v.size()); }

int Generator::reseed() { return ojr_reseed(this->cg, NULL, 0); }
int Generator::reseed(uint32_t val) {	return ojr_reseed(this->cg, &val, 1); }
int Generator::reseed(Seed v) { return ojr_reseed(this->cg, v.data(), v.size()); }

int Generator::seedSize() { return this->cg->seedsize; }
void Generator::getSeed(Seed &os) {
	if (0 == this->cg->seed) return;
	os.assign(this->cg->seed, this->cg->seed + this->cg->seedsize);
}
int Generator::getAlgorithm() { return ojr_get_algorithm(this->cg); }

uint16_t Generator::next16() { return ojr_next16(this->cg); }
uint32_t Generator::next32() { return ojr_next32(this->cg); }
uint64_t Generator::next64() { return ojr_next64(this->cg); }

double Generator::nextDouble() { return ojr_next_double(this->cg); }
double Generator::nextSignedDouble() { return ojr_next_signed_double(this->cg); }
double Generator::nextGaussian() { return ojr_next_gaussian(this->cg); }

int Generator::rand(int limit) { return ojr_rand(this->cg, limit); }

} /* namespace */
