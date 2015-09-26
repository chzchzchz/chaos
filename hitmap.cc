#include <string.h>
#include "hitmap.h"

HitMap::HitMap(const struct xfm &xfm_, unsigned x, unsigned y)
	: hm_xfm(xfm_)
	, dimx(x)
	, dimy(y)
{
	init_img();
}

HitMap::HitMap(unsigned x, unsigned y)
	: dimx(x)
	, dimy(y)
{
	init_img();
}

HitMap::~HitMap()
{
	for (unsigned i = 0; i < dimy; i++) delete img[i];
	delete [] img;
}

void HitMap::init_img(void)
{
	img = new unsigned char*[dimy];
	for (unsigned i = 0; i < dimy; i++) {
		img[i] = new unsigned char[dimx];
		memset(img[i], 0, dimx);
	}
}

void HitMap::inc(double x, double y)
{
	unsigned x_img = (unsigned int)((x + hm_xfm.xlateX) * hm_xfm.scaleX);
	unsigned y_img = (unsigned int)((y + hm_xfm.xlateY) * hm_xfm.scaleY);

	/* out of bounds? */
	if (x_img >= dimx || y_img >= dimy)
		return;

	if (img[y_img][x_img] < 0xff)
		img[y_img][x_img]++;
}

unsigned HitMap::get_fill_count(void) const
{
	unsigned ret = 0;
	for (unsigned y_idx = 0; y_idx < dimy; y_idx++)
		for(unsigned x_idx = 0; x_idx < dimx; x_idx++)
			if (img[y_idx][x_idx])
				ret++;
	return ret;
}
