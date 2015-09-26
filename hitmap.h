#ifndef HITMAP_H
#define HITMAP_H

#include "xfm.h"

class HitMap
{
public:
	HitMap(const struct xfm& xfm_, unsigned x, unsigned y);
	HitMap(unsigned x, unsigned y);
	~HitMap();

	void inc(double x, double y);

	unsigned get_dim_x(void) const { return dimx; }
	unsigned get_dim_y(void) const { return dimy; }
	const unsigned char *get_row(unsigned y) const { return img[y]; }
	unsigned get_fill_count(void) const;

private:
	void init_img(void);

	unsigned char	**img;
	unsigned int	dimx;
	unsigned int	dimy;
	struct xfm	hm_xfm;
};

#endif
