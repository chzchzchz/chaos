#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <sys/time.h>

struct xfm {
	double	scaleX;
	double	scaleY;
	double	xlateX;
	double	xlateY;
};

struct img {
	unsigned char	**img;
	unsigned int	dimx;
	unsigned int	dimy;
	struct xfm	xfm;
};

#define VECTOR_LEN	6


static double eval(double x, double y, double* v)
{
	return v[0] + v[1]*x + v[2]*y + v[3]*x*x + v[4]*x*y + v[5]*y*y;
}

#define MIN_FILL		0.15
#define SEARCH_DIM_X		100
#define SEARCH_DIM_Y		100
#define SEARCH_ITERS		10000
#define SEARCH_VMIN		-2.0
#define SEARCH_VMAX		2.0

#define maxmin(z)				\
	do {					\
		if (z##_new > z##_max)		\
			z##_max = z##_new;	\
		else if (z##_new < z##_min)	\
			z##_min = z##_new;	\
	} while(0)

static int tryVectorsUsage(double *a, double *b, int iters, struct img* img)
{
	double	x, y;
	int	x_idx, y_idx;
	unsigned int fill_count;
	int	i;

	for (i = 0; i < img->dimy; i++) {
		memset(img->img[i], 0, img->dimx);
	}

	x = 0; y = 0;
	for (i = 0; i < iters; i++) {
		double x_new, y_new;
		unsigned int  x_img, y_img;
		x_new = eval(x, y, a);
		y_new = eval(x, y, b);

		x_img = (unsigned int)((x_new+img->xfm.xlateX)*img->xfm.scaleX);
		y_img = (unsigned int)((y_new+img->xfm.xlateY)*img->xfm.scaleY);

		assert(x_img < img->dimx);
		assert(y_img < img->dimy);

		if (img->img[y_img][x_img] < 0xff)
			img->img[y_img][x_img]++;

		x = x_new;
		y = y_new;
	}

	fill_count = 0;
	for (y_idx = 0; y_idx < img->dimy; y_idx++)
		for(x_idx = 0; x_idx < img->dimx; x_idx++)
			if (img->img[y_idx][x_idx])
				fill_count++;

	if (fill_count < MIN_FILL*img->dimx*img->dimy)
		return 0;

	return 1;
}


static int tryVectorsBounds(double* a, double* b, int iters, struct img* img);


static int tryVectors(double* a, double* b, int iters, struct img* img)
{
	if (!tryVectorsBounds(a, b, iters, img))
		return 0;

	return tryVectorsUsage(a, b, iters, img);
}

static int tryVectorsBounds(double* a, double* b, int iters, struct img* img)
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
		return 0;
	if ((x_min - x_min) == x_min || (y_min - y_min) == y_min || 
	    x_min != x_min || y_min != y_min)
		return 0;

	if (x_max - x_min == x_max || x_max - x_min == -x_min ||
	    y_max - y_min == y_max || y_max - y_min == -y_min)
		return 0;

	img->xfm.xlateX = -x_min;
	img->xfm.xlateY = -y_min;
	img->xfm.scaleX = ((double)img->dimx - 1) / (x_max - x_min);
	img->xfm.scaleY = ((double)img->dimy - 1) / (y_max - y_min);

	return 1;
}

static struct img* img_create(int x, int y)
{
	struct img*	img;
	int		i;

	assert (x > 0 && y > 0);

	img = malloc(sizeof(struct img));
	img->dimx = x;
	img->dimy = y;
	img->img = malloc(sizeof(unsigned char*) * y);
	for (i = 0; i < y; i++)
		img->img[i] = malloc(sizeof(unsigned char) * x);

	return img;
}

static void img_redim(struct img* img, int x, int y)
{
	int	i;

	assert (x > 0 && y > 0);

	for (i = 0; i < img->dimy; i++)
		free(img->img[i]);
	free(img->img);

	img->xfm.scaleX /= ((double)img->dimx);
	img->xfm.scaleY /= ((double)img->dimy);
	img->xfm.scaleX *= ((double)x - 1);
	img->xfm.scaleY *= ((double)y - 1);

	img->dimx = x;
	img->dimy = y;

	img->img = malloc(sizeof(unsigned char*) * y);
	for (i = 0; i < y; i++)
		img->img[i] = malloc(sizeof(unsigned char) * x);
}

static void genVector(double* v, double min, double max)
{
	int	i;

	for (i = 0; i < VECTOR_LEN; i++) {
		double	rval;
		rval = (double)rand() / (double)RAND_MAX;
		rval *= (max - min);
		rval += min;
		v[i] = rval;
	}
}

static void img_destroy(struct img* img)
{
	int	i;
	for (i = 0; i < img->dimy; i++)
		free(img->img[i]);
	free(img->img);
	free(img);
}

/* return number of attempts */
static int findVectors(struct img* search_img, double *v1, double *v2)
{
	int	attempts = 0;
	do { 
		attempts++;
		genVector(v1, SEARCH_VMIN, SEARCH_VMAX);
		genVector(v2, SEARCH_VMIN, SEARCH_VMAX);
	} while (!tryVectors(v1, v2, SEARCH_ITERS, search_img));

	return attempts;
}

static void img_gen(double* a, double* b, struct img* img, int iters)
{
	double	x,y;
	int	i;

	for (i = 0; i < img->dimy; i++) {
		memset(img->img[i], 0, img->dimx);
	}

	x = 0; y = 0;
	for (i = 0; i < iters; i++) {
		double x_new, y_new;
		unsigned int  x_img, y_img;
		x_new = eval(x, y, a);
		y_new = eval(x, y, b);

		x_img = (unsigned int)((x_new+img->xfm.xlateX)*img->xfm.scaleX);
		y_img = (unsigned int)((y_new+img->xfm.xlateY)*img->xfm.scaleY);

		x = x_new;
		y = y_new;

		if (x_img >= img->dimx || y_img >= img->dimy)
			continue;

		if (img->img[y_img][x_img] < 0xff)
			img->img[y_img][x_img]++;
	}
}

struct jpeg_info {
	JSAMPLE	*buf;
	int	h;
	int	w;
};

/*
 * Sample routine for JPEG compression.  We assume that the target file name
 * and a compression quality factor are passed in.
 */
static void write_jpeg(struct jpeg_info* info, char * filename, int quality)
{
	struct jpeg_compress_struct	cinfo;
	struct jpeg_error_mgr		jerr;
	FILE*				outfile;
	JSAMPROW			row_pointer[1];
	int				row_stride;

	cinfo.err = jpeg_std_error(&jerr);
//	if ((outfile = fopen(filename, "wb")) == NULL) {
//		fprintf(stderr, "can't open %s\n", filename);
//		exit(1);
//	}
	outfile = stdout;

	jpeg_create_compress(&cinfo);

	cinfo.image_width = info->w;
	cinfo.image_height = info->h;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_stdio_dest(&cinfo, outfile);
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);

	jpeg_start_compress(&cinfo, TRUE);
	row_stride = info->w * 3;	/* JSAMPLEs per row in img_buf */

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & info->buf[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
//	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
}

void img_write(struct img* img, char* fname)
{
	struct jpeg_info	jinfo;
	int			x, y;
	jinfo.h = img->dimy;
	jinfo.w = img->dimx;
	jinfo.buf = malloc(3 * img->dimx * img->dimy);

	for (y = 0; y < img->dimy; y++) {
		int	off;
		off = y*img->dimx*3;
		for (x = 0; x < img->dimx; x++) {
			unsigned int	v;
			v = img->img[y][x];
//			v *= 3;
			v /= 3;
			v = 0xff - v;
			jinfo.buf[off + x*3] = v;
			jinfo.buf[off + x*3 + 1] = v;
			jinfo.buf[off + x*3 + 2] = v;
		}
	}

	write_jpeg(&jinfo, fname, 90);

	free(jinfo.buf);
}


int main(int argc, char* argv[])
{
	struct img	*img;
	double		v1[VECTOR_LEN], v2[VECTOR_LEN];
	int		attempts;
	int		in_x, in_y;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s dim_x dim_y\n", argv[0]);
		return 1;
	}

	in_x = atoi(argv[1]);
	in_y = atoi(argv[2]);

	srand(time(0));
	img = img_create(SEARCH_DIM_X, SEARCH_DIM_Y);

	attempts = findVectors(img, v1, v2);
	fprintf(stderr, "attempts: %d\n", attempts);

	img_redim(img, in_x, in_y);
	img_gen(v1, v2, img,  in_x*in_y*20);
	img_write(img, "hello.jpg");
	img_destroy(img);

	return 0;
}
