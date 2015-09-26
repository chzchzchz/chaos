#ifndef ATTRACTOR_H
#define ATTRACTOR_H

#include <vector>

#include "xfm.h"
#include "hitmap.h"

#define MAP_DEGREE	3
#define SEARCH_DIM_X	100
#define SEARCH_DIM_Y	100
#define SEARCH_VMIN	-2.0
#define SEARCH_VMAX	2.0
#define MIN_FILL	0.15
#define SEARCH_ITERS	10000

class Attractor
{
public:
	Attractor();

	const xfm& get_xfm(void) const { return search_xfm; }
	void apply(HitMap& hm, unsigned iters) const;

private:
	bool try_vec_bounds(unsigned iters);
	bool try_vecs(unsigned iters);
	bool try_vecs_usage(unsigned iters) const;
	unsigned find_vecs(void);

	struct xfm 		search_xfm;
	std::vector<double>	a, b;
};

#endif
