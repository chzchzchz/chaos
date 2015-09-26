#ifndef XFM_H
#define XFM_H

// describes how to scale/translate x,y coords
struct xfm {
	xfm()
	: scaleX(1.0)
	, scaleY(1.0)
	, xlateX(0.0)
	, xlateY(0.0)
	{}

	double	scaleX;
	double	scaleY;
	double	xlateX;
	double	xlateY;
};

#endif
