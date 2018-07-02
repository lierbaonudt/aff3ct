#ifndef MONITOR_MI_BFER_HPP_
#define MONITOR_MI_BFER_HPP_

#ifdef ENABLE_MPI
#include <mpi.h>
#endif

#include "../MI/Monitor_MI.hpp"
#include "../BFER/Monitor_BFER.hpp"

namespace aff3ct
{
namespace module
{

template <typename B = int, typename R = float>
class Monitor_MI_BFER : public Monitor_MI<B,R>, public Monitor_BFER<B>
{
public:
	inline Task&   operator[](const mnt::tsk t) { return Module::operator[]((int)t); };
	using Monitor_MI<B,R>::operator[];
	using Monitor_BFER<B>::operator[];


	struct Values_t
	{
		typename Monitor_MI<B,R>::Values_t& mi_vals;
		typename Monitor_BFER<B>::Values_t& er_vals;

		Values_t(typename Monitor_MI<B,R>::Values_t& mi_vals, typename Monitor_BFER<B>::Values_t& er_vals)
		: mi_vals(mi_vals), er_vals(er_vals) { }

		static constexpr unsigned n_attributes = Monitor_MI<B,R>::Values_t::n_attributes + Monitor_BFER<B>::Values_t::n_attributes;

		Values_t& operator+=(const Values_t& o)
		{
			mi_vals += o.mi_vals;
			er_vals += o.er_vals;
			return *this;
		}

		Values_t& operator=(const Values_t& o)
		{
			mi_vals = o.mi_vals;
			er_vals = o.er_vals;
			return *this;
		}

		void reset()
		{
			mi_vals.reset();
			er_vals.reset();
		}

	#ifdef ENABLE_MPI
		static void create_MPI_struct(int          blen         [n_attributes],
		                              MPI_Aint     displacements[n_attributes],
		                              MPI_Datatype oldtypes     [n_attributes])
		{
			auto MI_blen            = &blen         [0];
			auto MI_displacements   = &displacements[0];
			auto MI_oldtypes        = &oldtypes     [0];
			auto BFER_blen          = &blen         [Monitor_MI<B,R>::Values_t::n_attributes];
			auto BFER_displacements = &displacements[Monitor_MI<B,R>::Values_t::n_attributes]; // TODO: warning might be wrong displacement next to MI attributes
			auto BFER_oldtypes      = &oldtypes     [Monitor_MI<B,R>::Values_t::n_attributes];

			Monitor_MI<B,R>::Values_t::create_MPI_struct(  MI_blen    MI_displacements,   MI_oldtypes);
			Monitor_BFER<B>::Values_t::create_MPI_struct(BFER_blen, BFER_displacements, BFER_oldtypes);
		}
	#endif
	};

protected:
	Values_t vals;

public:
	Monitor_MI_BFER(const int K, const int N,
	                const unsigned mi_max_n_cf,
	                const unsigned er_max_fe, const bool er_count_unknown_values = false,
	                const int n_frames = 1);

	Monitor_MI_BFER();
	Monitor_MI_BFER(const Monitor_MI_BFER<B,R>& mon, const int n_frames = -1);

	virtual ~Monitor_MI_BFER() = default;


	const Values_t& get_vals() const;

	virtual void add_handler_check(std::function<void(void)> callback);

	virtual void reset();
	virtual void clear_callbacks();

	virtual void collect(const Monitor& m, bool fully = false);
	virtual void collect(const Monitor_MI_BFER<B,R>& m, bool fully = false);

	Monitor_MI_BFER<B,R>& operator=(const Monitor_MI_BFER<B,R>& m);

protected:
	Monitor_MI_BFER(const bool create_task, const int K, const int N,
	                const unsigned mi_max_n_cf,
	                const unsigned er_max_fe, const bool er_count_unknown_values,
	                const int n_frames);
};
}
}

#endif /* MONITOR_MI_BFER_HPP_ */