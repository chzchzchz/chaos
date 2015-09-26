#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <jpeglib.h>
#include <setjmp.h>

#include "attractor.h"

struct jpeg_info {
	jpeg_info(unsigned x, unsigned y)
		: h(y)
		, w(x)
		, buf((JSAMPLE*)(malloc(3 * x * y)))
	{}

	~jpeg_info() { free(buf); }

	unsigned h;
	unsigned w;
	JSAMPLE	*buf;
};

/*
 * Sample routine for JPEG compression.  We assume that the target file name
 * and a compression quality factor are passed in.
 */
static void write_jpeg(struct jpeg_info* info, const char * filename, int quality)
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

static void img_write(const HitMap& hm, const char* fname)
{
	struct jpeg_info	jinfo(hm.get_dim_x(), hm.get_dim_y());

	for (unsigned y = 0; y < jinfo.h; y++) {
		const unsigned char	*row = hm.get_row(y);
		unsigned		off = y * jinfo.w * 3;

		for (unsigned x = 0; x < jinfo.w; x++) {
			unsigned int	v;
			v = row[x];
//			v *= 3;
			v /= 3;
			v = 0xff - v;
			jinfo.buf[off + x*3] = v;
			jinfo.buf[off + x*3 + 1] = v;
			jinfo.buf[off + x*3 + 2] = v;
		}
	}

	write_jpeg(&jinfo, fname, 90);
}

int main(int argc, char* argv[])
{
	int		in_x, in_y;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s dim_x dim_y\n", argv[0]);
		return 1;
	}

	in_x = atoi(argv[1]);
	in_y = atoi(argv[2]);

	srand(time(0));

	Attractor	attractor;
	struct xfm	img_xfm(attractor.get_xfm());

	img_xfm.scaleX /= ((double)SEARCH_DIM_X);
	img_xfm.scaleY /= ((double)SEARCH_DIM_Y);
	img_xfm.scaleX *= ((double)in_x - 1);
	img_xfm.scaleY *= ((double)in_y - 1);

	HitMap		hm(img_xfm, in_x, in_y);

	attractor.apply(hm, in_x * in_y * 20);

	img_write(hm, "hello.jpg");

	return 0;
}
