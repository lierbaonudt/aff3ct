#ifndef NOISE_GENERATOR_HPP_
#define NOISE_GENERATOR_HPP_

#include <vector>

namespace aff3ct
{
namespace tools
{
template <typename R = float>
class Noise_generator
{
public:
	Noise_generator()
	{
	}

	virtual ~Noise_generator()
	{
	}

	template <class A = std::allocator<R>>
	void generate(std::vector<R,A> &noise, const R sigma, const R mu = 0.0)
	{
		this->generate(noise.data(), (unsigned)noise.size(), sigma, mu);
	}

	virtual void set_seed(const int seed) = 0;
	virtual void generate(R *noise, const unsigned length, const R sigma, const R mu = 0.0) = 0;
};

}
}

#endif /* NOISE_GENERATOR_HPP_ */