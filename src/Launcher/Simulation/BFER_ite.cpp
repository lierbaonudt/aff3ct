#include "Simulation/BFER/Iterative/BFER_ite.hpp"

#include <thread>
#include <string>
#include <iostream>

#include "BFER_ite.hpp"

using namespace aff3ct;
using namespace aff3ct::launcher;

template <typename B, typename R, typename Q>
BFER_ite<B,R,Q>
::BFER_ite(const int argc, const char **argv, std::ostream &stream)
: Launcher(argc, argv, stream), codec(nullptr), params(new factory::BFER_ite::parameters())
{
	Launcher::params = params;
}

template <typename B, typename R, typename Q>
BFER_ite<B,R,Q>
::~BFER_ite()
{
	if (codec != nullptr)
		delete codec;
}

template <typename B, typename R, typename Q>
void BFER_ite<B,R,Q>
::build_args()
{
	Launcher::build_args();

	factory::BFER_ite::build_args(this->req_args, this->opt_args);
	factory::Source             ::build_args(this->req_args, this->opt_args);
	factory::CRC                ::build_args(this->req_args, this->opt_args);
	factory::Interleaver        ::build_args(this->req_args, this->opt_args);
	factory::Modem              ::build_args(this->req_args, this->opt_args);
	factory::Channel            ::build_args(this->req_args, this->opt_args);
	if (std::is_integral<Q>())
		factory::Quantizer      ::build_args(this->req_args, this->opt_args);
	factory::Monitor            ::build_args(this->req_args, this->opt_args);
	factory::Terminal_BFER      ::build_args(this->req_args, this->opt_args);

	if (this->req_args.find({"enc-info-bits", "K"}) != this->req_args.end() ||
	    this->req_args.find({"pct-info-bits", "K"}) != this->req_args.end())
		this->req_args.erase({"src-info-bits", "K"});
	this->opt_args.erase({"src-seed",      "S"});
	this->req_args.erase({"crc-info-bits", "K"});
	this->opt_args.erase({"crc-fra",       "F"});
	this->req_args.erase({"itl-size"          });
	this->opt_args.erase({"itl-fra",       "F"});
	this->opt_args.erase({"itl-seed",      "S"});
	this->req_args.erase({"mdm-fra-size",  "N"});
	this->opt_args.erase({"mdm-fra",       "F"});
	this->opt_args.erase({"mdm-sigma"        });
	this->req_args.erase({"chn-fra-size",  "N"});
	this->opt_args.erase({"chn-fra",       "F"});
	this->opt_args.erase({"chn-sigma"         });
	this->opt_args.erase({"chn-seed",      "S"});
	this->opt_args.erase({"chn-add-users"     });
	this->opt_args.erase({"chn-complex"       });
	this->req_args.erase({"qnt-size",      "N"});
	this->opt_args.erase({"qnt-fra",       "F"});
	this->opt_args.erase({"qnt-sigma"         });
	this->req_args.erase({"mnt-size",      "K"});
	this->opt_args.erase({"mnt-fra",       "F"});
	this->req_args.erase({"ter-info-bits","K"});
	this->opt_args.erase({"ter-cw-size",  "N"});
}

template <typename B, typename R, typename Q>
void BFER_ite<B,R,Q>
::store_args()
{
	Launcher::store_args();

	factory::BFER_ite::store_args(this->ar.get_args(), *params);

	factory::Source::store_args(this->ar.get_args(), *params->src);

	auto K = this->req_args.find({"src-info-bits", "K"}) != this->req_args.end() ? params->src->K : params->enc->K;
	auto N = this->req_args.find({"src-info-bits", "K"}) != this->req_args.end() ? params->src->K : params->pct->N;

	factory::CRC::store_args(this->ar.get_args(), *params->crc);

	params->crc->K = K - params->crc->size;
	params->src->K = params->src->K == 0 ? params->crc->K : params->src->K;

	params->itl->size = N;

	factory::Interleaver::store_args(this->ar.get_args(), *params->itl);

	params->mdm->N = N;

	factory::Modem::store_args(this->ar.get_args(), *params->mdm);

	params->chn->N         = params->mdm->N_mod;
	params->chn->complex   = params->mdm->complex;
	params->chn->add_users = params->mdm->type == "SCMA";

	factory::Channel::store_args(this->ar.get_args(), *params->chn);

	params->qnt->size = params->mdm->N_fil;

	if (std::is_integral<Q>())
		factory::Quantizer::store_args(this->ar.get_args(), *params->qnt);

	params->mnt->size = params->src->K;

	factory::Monitor::store_args(this->ar.get_args(), *params->mnt);

	params->ter->K = K;
	params->ter->N = N;

	factory::Terminal_BFER::store_args(this->ar.get_args(), *params->ter);

	if (!std::is_integral<Q>())
		params->qnt->type = "NO";

	if (params->coset)
		params->enc->type = "COSET";

	if (params->src->type == "AZCW" || params->enc->type == "AZCW")
	{
		params->src->type = "AZCW";
		params->enc->type = "AZCW";
	}

	params->crc->n_frames = params->src->n_frames;
	params->enc->n_frames = params->src->n_frames;
	params->itl->n_frames = params->src->n_frames;
	params->pct->n_frames = params->src->n_frames;
	params->mdm->n_frames = params->src->n_frames;
	params->chn->n_frames = params->src->n_frames;
	params->qnt->n_frames = params->src->n_frames;
	params->dec->n_frames = params->src->n_frames;
	params->mnt->n_frames = params->src->n_frames;
}

template <typename B, typename R, typename Q>
void BFER_ite<B,R,Q>
::group_args()
{
	Launcher::group_args();

	this->arg_group.push_back({factory::Simulation ::prefix, factory::Simulation ::name + " parameter(s)"});
	this->arg_group.push_back({factory::Source     ::prefix, factory::Source     ::name + " parameter(s)"});
	this->arg_group.push_back({factory::CRC        ::prefix, factory::CRC        ::name + " parameter(s)"});
	this->arg_group.push_back({factory::Encoder    ::prefix, factory::Encoder    ::name + " parameter(s)"});
	this->arg_group.push_back({factory::Interleaver::prefix, factory::Interleaver::name + " parameter(s)"});
	this->arg_group.push_back({factory::Puncturer  ::prefix, factory::Puncturer  ::name + " parameter(s)"});
	this->arg_group.push_back({factory::Modem      ::prefix, factory::Modem      ::name + " parameter(s)"});
	this->arg_group.push_back({factory::Channel    ::prefix, factory::Channel    ::name + " parameter(s)"});
	this->arg_group.push_back({factory::Quantizer  ::prefix, factory::Quantizer  ::name + " parameter(s)"});
	this->arg_group.push_back({factory::Decoder    ::prefix, factory::Decoder    ::name + " parameter(s)"});
	this->arg_group.push_back({factory::Monitor    ::prefix, factory::Monitor    ::name + " parameter(s)"});
	this->arg_group.push_back({factory::Terminal   ::prefix, factory::Terminal   ::name + " parameter(s)"});
}

template <typename B, typename R, typename Q>
void BFER_ite<B,R,Q>
::print_header()
{
	factory::BFER_ite::header(this->pl_sim, *params);
	factory::Source             ::header(this->pl_src, *params->src);
	factory::CRC                ::header(this->pl_crc, *params->crc);
	factory::Interleaver        ::header(this->pl_itl, *params->itl);
	factory::Modem              ::header(this->pl_mdm, *params->mdm);
	factory::Channel            ::header(this->pl_chn, *params->chn);
	factory::Quantizer          ::header(this->pl_qnt, *params->qnt);
	factory::Monitor            ::header(this->pl_mnt, *params->mnt);
	factory::Terminal_BFER      ::header(this->pl_ter, *params->ter);

	Launcher::print_header();
}

template <typename B, typename R, typename Q>
simulation::Simulation* BFER_ite<B,R,Q>
::build_simu()
{
	this->build_codec();
	return factory::BFER_ite::build<B,R,Q>(*params, *codec);
}

// ==================================================================================== explicit template instantiation 
#include "Tools/types.h"
#ifdef MULTI_PREC
template class aff3ct::launcher::BFER_ite<B_8,R_8,Q_8>;
template class aff3ct::launcher::BFER_ite<B_16,R_16,Q_16>;
template class aff3ct::launcher::BFER_ite<B_32,R_32,Q_32>;
template class aff3ct::launcher::BFER_ite<B_64,R_64,Q_64>;
#else
template class aff3ct::launcher::BFER_ite<B,R,Q>;
#endif
// ==================================================================================== explicit template instantiation
