#include <cassert>
#include <vector>
#include <cmath>

#include "Encoder_repetition_sys.hpp"

template <typename B>
Encoder_repetition_sys<B>
::Encoder_repetition_sys(const int& K, const int& N, const bool buffered_encoding, const std::string name)
: Encoder_sys<B>(K, N, 1, name), rep_count((N/K) -1), buffered_encoding(buffered_encoding)
{
	assert(N % K == 0); // check if repetition count is consistent
}

template <typename B>
void Encoder_repetition_sys<B>
::encode_sys(const mipp::vector<B>& U_K, mipp::vector<B>& par)
{
	assert(this->n_frames == 1);
	assert(buffered_encoding);

	for (auto i = 0; i < rep_count; i++) // parity bits
		std::copy(U_K.begin(), U_K.end(), par.begin() + i * this->K);
}

template <typename B>
void Encoder_repetition_sys<B>
::encode(const mipp::vector<B>& U_K, mipp::vector<B>& X_N)
{
	assert(this->n_frames == 1);
	
	// repetition
	if (!buffered_encoding)
	{
		for (auto i = 0; i < this->K; i++)
		{
			const auto off1 = i * (rep_count +1);
			const auto off2 = off1 +1;

			const auto bit = U_K[i];

			X_N[off1] = bit; // systematic bit

			// parity bits
			for (auto j = 0; j < rep_count; j++)
				X_N[off2 +j] = bit;
		}
	}
	else
	{
		std::copy(U_K.begin(), U_K.end(), X_N.begin()); // systematic bits
		for (auto i = 1; i <= rep_count; i++) // parity bits
			std::copy(U_K.begin(), U_K.end(), X_N.begin() + i * this->K);
	}
}

// ==================================================================================== explicit template instantiation 
#include "Tools/types.h"
#ifdef MULTI_PREC
template class Encoder_repetition_sys<B_8>;
template class Encoder_repetition_sys<B_16>;
template class Encoder_repetition_sys<B_32>;
template class Encoder_repetition_sys<B_64>;
#else
template class Encoder_repetition_sys<B>;
#endif
// ==================================================================================== explicit template instantiation