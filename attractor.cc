#include <stdlib.h>
#include "attractor.h"

static double eval(double x, double y, const std::vector<double>& v)
{
	return v[0] + v[1]*x + v[2]*y + v[3]*x*x + v[4]*x*y + v[5]*y*y;
}

#define maxmin(z)				\
	do {					\
		if (z##_new > z##_max)		\
			z##_max = z##_new;	\
		else if (z##_new < z##_min)	\
			z##_min = z##_new;	\
	} while(0)

Attractor::Attractor()
{
	find_vecs();
}

bool Attractor::try_vecs_usage(unsigned iters) const
{
	HitMap	hm(search_xfm, SEARCH_DIM_X, SEARCH_DIM_Y);

	apply(hm, SEARCH_ITERS);

	if (hm.get_fill_count() < MIN_FILL * SEARCH_DIM_X * SEARCH_DIM_Y)
		return false;

	return true;
}

bool Attractor::try_vecs(unsigned iters)
{
	if (!try_vec_bounds(iters))
		return false;

	return try_vecs_usage(iters);
}

bool Attractor::try_vec_bounds(unsigned iters)
{
	double	x_min, y_min, x_max, y_max, x, y;
	int	i;

	x = 0; y = 0;

	x_min = x_max = x;
	y_min = y_max = y;
	for (i = 0; i < iters; i++) {
		double x_new, y_new;
		x_new = eval(x, y, a);
		y_new = eval(x, y, b);
		maxmin(x);
		maxmin(y);
		x = x_new;
		y = y_new;
	}

	if ((x_max - x_max) == x_max || (y_max - y_max) == y_max || 
	    x_max != x_max || y_max != y_max)
		return false;
	if ((x_min - x_min) == x_min || (y_min - y_min) == y_min || 
	    x_min != x_min || y_min != y_min)
		return false;

	if (x_max - x_min == x_max || x_max - x_min == -x_min ||
	    y_max - y_min == y_max || y_max - y_min == -y_min)
		return false;

	search_xfm.xlateX = -x_min;
	search_xfm.xlateY = -y_min;
	search_xfm.scaleX = ((double)SEARCH_DIM_X - 1) / (x_max - x_min);
	search_xfm.scaleY = ((double)SEARCH_DIM_Y - 1) / (y_max - y_min);

	return true;
}

static void gen_vec(std::vector<double>& v, double min, double max)
{
	v.resize(VECTOR_LEN);
	for (unsigned i = 0; i < v.size(); i++) {
		double	rval;
		rval = (double)rand() / (double)RAND_MAX;
		rval *= (max - min);
		rval += min;
		v[i] = rval;
	}
}

/* return number of attempts */
unsigned Attractor::find_vecs(void)
{
	unsigned attempts = 0;
	do { 
		attempts++;
		gen_vec(a, SEARCH_VMIN, SEARCH_VMAX);
		gen_vec(b, SEARCH_VMIN, SEARCH_VMAX);
	} while (!try_vecs(SEARCH_ITERS));

	return attempts;
}

void Attractor::apply(HitMap& hm, unsigned iters) const
{
	double	x = 0.0,
		y = 0.0;
	for (unsigned i = 0; i < iters; i++) {
		double	x_new = eval(x, y, a),
			y_new = eval(x, y, b);

		hm.inc(x_new, y_new);

		x = x_new;
		y = y_new;
	}
}
