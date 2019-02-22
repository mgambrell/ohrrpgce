/*
Rotozoomer and zoomer for 32bit or 8bit surfaces
Adapted from SDL2_rotozoom.h from sdl2_gfx, version 1.0.4 - zlib licensed.
http://www.ferzkopp.net/wordpress/2016/01/02/sdl_gfx-sdl2_gfx/
https://sourceforge.net/projects/sdl2gfx/

Copyright (C) 2012-2014  Andreas Schiffler

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.

Andreas Schiffler -- aschiffler at ferzkopp dot net

*/

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "misc.h"
#include "rotozoom.h"

/* A 32 bit RGBA pixel. */
typedef struct tColorRGBA {
	uint8_t r, g, b, a;
} tColorRGBA;

/* A 8 bit paletted pixel. */
typedef struct tColorY {
	uint8_t y;
} tColorY;

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))

/*
Number of guard rows added to destination surfaces.

This is a simple but effective workaround for observed issues.
These rows allocate extra memory and are then hidden from the surface.
Rows are added to the end of destination surfaces when they are allocated.
This catches any potential overflows which seem to happen with
just the right src image dimensions and scale/rotation and can lead
to a situation where the program can segfault.
*/
#define GUARD_ROWS 2

/*
Lower limit of absolute zoom factor or rotation degrees.
*/
#define VALUE_LIMIT	0.001



/*
Internal 32 bit Zoomer with optional anti-aliasing by bilinear interpolation.

Zooms 32 bit RGBA/ABGR 'src' surface to 'dst' surface.
Assumes src and dst surfaces are of 32 bit depth.
Assumes dst surface was allocated with the correct dimensions.

`src`    : The surface to zoom (input).
`dst`    : The zoomed surface (output).
`flipx`  : Flag indicating if the image should be horizontally flipped.
`flipy`  : Flag indicating if the image should be vertically flipped.
`smooth` : Whether to antialias.

Returns 0 for success or -1 for error.
*/
int _zoomSurfaceRGBA(SDL_Surface * src, SDL_Surface * dst, boolint flipx, boolint flipy, boolint smooth)
{
	int x, y, sx, sy, ssx, ssy, *sax, *say, *csax, *csay, *salast, csx, csy, ex, ey, cx, cy, sstep, sstepx, sstepy;
	tColorRGBA *c00, *c01, *c10, *c11;
	tColorRGBA *sp, *csp, *dp;
	int spixelgap, spixelw, spixelh, dgap, t1, t2;

	/* Allocate memory for row/column increments */
	if ((sax = (int *) malloc((dst->w + 1) * sizeof(uint32_t))) == NULL) {
		return -1;
	}
	if ((say = (int *) malloc((dst->h + 1) * sizeof(uint32_t))) == NULL) {
		free(sax);
		return -1;
	}

	/* Precalculate row increments */
	spixelw = (src->w - 1);
	spixelh = (src->h - 1);
	if (smooth) {
		sx = (int) (65536.0 * (float) spixelw / (float) (dst->w - 1));
		sy = (int) (65536.0 * (float) spixelh / (float) (dst->h - 1));
	} else {
		sx = (int) (65536.0 * (float) (src->w) / (float) (dst->w));
		sy = (int) (65536.0 * (float) (src->h) / (float) (dst->h));
	}

	/* Maximum scaled source size */
	ssx = (src->w << 16) - 1;
	ssy = (src->h << 16) - 1;

	/* Precalculate horizontal row increments */
	csx = 0;
	csax = sax;
	for (x = 0; x <= dst->w; x++) {
		*csax = csx;
		csax++;
		csx += sx;

		/* Guard from overflows */
		if (csx > ssx) {
			csx = ssx;
		}
	}

	/* Precalculate vertical row increments */
	csy = 0;
	csay = say;
	for (y = 0; y <= dst->h; y++) {
		*csay = csy;
		csay++;
		csy += sy;

		/* Guard from overflows */
		if (csy > ssy) {
			csy = ssy;
		}
	}

	sp = (tColorRGBA *) src->pixels;
	dp = (tColorRGBA *) dst->pixels;
	dgap = dst->pitch - dst->w * 4;
	spixelgap = src->pitch/4;

	if (flipx) sp += spixelw;
	if (flipy) sp += (spixelgap * spixelh);

	if (smooth) {
		/*
		* Interpolating Zoom
		*/
		csay = say;
		for (y = 0; y < dst->h; y++) {
			csp = sp;
			csax = sax;
			for (x = 0; x < dst->w; x++) {
				/* Setup color source pointers */
				ex = (*csax & 0xffff);
				ey = (*csay & 0xffff);
				cx = (*csax >> 16);
				cy = (*csay >> 16);
				sstepx = cx < spixelw;
				sstepy = cy < spixelh;
				c00 = sp;
				c01 = sp;
				c10 = sp;
				if (sstepy) {
					if (flipy) {
						c10 -= spixelgap;
					} else {
						c10 += spixelgap;
					}
				}
				c11 = c10;
				if (sstepx) {
					if (flipx) {
						c01--;
						c11--;
					} else {
						c01++;
						c11++;
					}
				}

				/* Draw and interpolate colors */
				t1 = ((((c01->r - c00->r) * ex) >> 16) + c00->r) & 0xff;
				t2 = ((((c11->r - c10->r) * ex) >> 16) + c10->r) & 0xff;
				dp->r = (((t2 - t1) * ey) >> 16) + t1;
				t1 = ((((c01->g - c00->g) * ex) >> 16) + c00->g) & 0xff;
				t2 = ((((c11->g - c10->g) * ex) >> 16) + c10->g) & 0xff;
				dp->g = (((t2 - t1) * ey) >> 16) + t1;
				t1 = ((((c01->b - c00->b) * ex) >> 16) + c00->b) & 0xff;
				t2 = ((((c11->b - c10->b) * ex) >> 16) + c10->b) & 0xff;
				dp->b = (((t2 - t1) * ey) >> 16) + t1;
				t1 = ((((c01->a - c00->a) * ex) >> 16) + c00->a) & 0xff;
				t2 = ((((c11->a - c10->a) * ex) >> 16) + c10->a) & 0xff;
				dp->a = (((t2 - t1) * ey) >> 16) + t1;

				/* Advance source pointer x */
				salast = csax;
				csax++;
				sstep = (*csax >> 16) - (*salast >> 16);
				if (flipx) {
					sp -= sstep;
				} else {
					sp += sstep;
				}

				/* Advance destination pointer x */
				dp++;
			}

			/* Advance source pointer y */
			salast = csay;
			csay++;
			sstep = (*csay >> 16) - (*salast >> 16);
			sstep *= spixelgap;
			if (flipy) {
				sp = csp - sstep;
			} else {
				sp = csp + sstep;
			}

			/* Advance destination pointer y */
			dp = (tColorRGBA *) ((uint8_t *) dp + dgap);
		}

	} else {
		/*
		* Non-Interpolating Zoom
		*/
		csay = say;
		for (y = 0; y < dst->h; y++) {
			csp = sp;
			csax = sax;
			for (x = 0; x < dst->w; x++) {
				/* Draw */
				*dp = *sp;

				/* Advance source pointer x */
				salast = csax;
				csax++;
				sstep = (*csax >> 16) - (*salast >> 16);
				if (flipx) sstep = -sstep;
				sp += sstep;

				/* Advance destination pointer x */
				dp++;
			}
			/* Advance source pointer y */
			salast = csay;
			csay++;
			sstep = (*csay >> 16) - (*salast >> 16);
			sstep *= spixelgap;
			if (flipy) sstep = -sstep;
			sp = csp + sstep;

			/* Advance destination pointer y */
			dp = (tColorRGBA *) ((uint8_t *) dp + dgap);
		}
	}

	free(sax);
	free(say);
	return 0;
}

/*
Internal 8 bit Zoomer without smoothing.

Zooms 8bit palette/Y 'src' surface to 'dst' surface.
Assumes src and dst surfaces are of 8 bit depth.
Assumes dst surface was allocated with the correct dimensions.

`src`   : The surface to zoom (input).
`dst`   : The zoomed surface (output).
`flipx` : Flag indicating if the image should be horizontally flipped.
`flipy` : Flag indicating if the image should be vertically flipped.

Returns 0 for success or -1 for error.
*/
int _zoomSurfaceY(SDL_Surface * src, SDL_Surface * dst, boolint flipx, boolint flipy)
{
	int x, y;
	uint32_t *sax, *say, *csax, *csay;
	int csx, csy;
	uint8_t *sp, *dp, *csp;
	int dgap;

	/* Allocate memory for row increments */
	if ((sax = (uint32_t *) malloc((dst->w + 1) * sizeof(uint32_t))) == NULL) {
		return -1;
	}
	if ((say = (uint32_t *) malloc((dst->h + 1) * sizeof(uint32_t))) == NULL) {
		free(sax);
		return -1;
	}

	/* Pointer setup */
	sp = csp = (uint8_t *) src->pixels;
	dp = (uint8_t *) dst->pixels;
	dgap = dst->pitch - dst->w;

	if (flipx) csp += (src->w-1);
	if (flipy) csp  = ( (uint8_t*)csp + src->pitch*(src->h-1) );

	/* Precalculate row increments */
	csx = 0;
	csax = sax;
	for (x = 0; x < dst->w; x++) {
		csx += src->w;
		*csax = 0;
		while (csx >= dst->w) {
			csx -= dst->w;
			(*csax)++;
		}
		(*csax) = (*csax) * (flipx ? -1 : 1);
		csax++;
	}
	csy = 0;
	csay = say;
	for (y = 0; y < dst->h; y++) {
		csy += src->h;
		*csay = 0;
		while (csy >= dst->h) {
			csy -= dst->h;
			(*csay)++;
		}
		(*csay) = (*csay) * (flipy ? -1 : 1);
		csay++;
	}

	/* Draw */
	csay = say;
	for (y = 0; y < dst->h; y++) {
		csax = sax;
		sp = csp;
		for (x = 0; x < dst->w; x++) {
			/* Draw */
			*dp = *sp;
			/* Advance source pointers */
			sp += (*csax);
			csax++;
			/* Advance destination pointer */
			dp++;
		}
		/* Advance source pointer (for row) */
		csp += ((*csay) * src->pitch);
		csay++;

		/* Advance destination pointers */
		dp += dgap;
	}

	free(sax);
	free(say);
	return 0;
}

/*
Internal 32 bit rotozoomer with optional anti-aliasing.

Rotates and zooms 32 bit RGBA/ABGR 'src' surface to 'dst' surface based on the control
parameters by scanning the destination surface and applying optionally anti-aliasing
by bilinear interpolation.
Assumes src and dst surfaces are of 32 bit depth.
Assumes dst surface was allocated with the correct dimensions.

`src`    : Source surface.
`dst`    : Destination surface.
`cx`     : Horizontal center coordinate.
`cy`     : Vertical center coordinate.
`isin`   : Integer version of sine of angle.
`icos`   : Integer version of cosine of angle.
`flipx`  : Flag indicating horizontal mirroring should be applied.
`flipy`  : Flag indicating vertical mirroring should be applied.
`smooth` : Whether to antialias.
*/
void _transformSurfaceRGBA(SDL_Surface * src, SDL_Surface * dst, int cx, int cy, int isin, int icos, boolint flipx, boolint flipy, boolint smooth)
{
	int x, y, t1, t2, dx, dy, xd, yd, sdx, sdy, ax, ay, ex, ey, sw, sh;
	tColorRGBA c00, c01, c10, c11, cswap;
	tColorRGBA *pc, *sp;
	int gap;

	/* Variable setup */
	xd = ((src->w - dst->w) << 15);
	yd = ((src->h - dst->h) << 15);
	ax = (cx << 16) - (icos * cx);
	ay = (cy << 16) - (isin * cx);
	sw = src->w - 1;
	sh = src->h - 1;
	pc = (tColorRGBA*) dst->pixels;
	gap = dst->pitch - dst->w * 4;

	if (smooth) {
		/*
		* Interpolating version
		*/
		for (y = 0; y < dst->h; y++) {
			dy = cy - y;
			sdx = (ax + (isin * dy)) + xd;
			sdy = (ay - (icos * dy)) + yd;
			for (x = 0; x < dst->w; x++) {
				dx = (sdx >> 16);
				dy = (sdy >> 16);
				if (flipx) dx = sw - dx;
				if (flipy) dy = sh - dy;
				if ((dx > -1) && (dy > -1) && (dx < (src->w-1)) && (dy < (src->h-1))) {
					sp = (tColorRGBA *)src->pixels;
					sp += ((src->pitch/4) * dy);
					sp += dx;
					c00 = *sp;
					sp += 1;
					c01 = *sp;
					sp += (src->pitch/4);
					c11 = *sp;
					sp -= 1;
					c10 = *sp;
					if (flipx) {
						cswap = c00; c00=c01; c01=cswap;
						cswap = c10; c10=c11; c11=cswap;
					}
					if (flipy) {
						cswap = c00; c00=c10; c10=cswap;
						cswap = c01; c01=c11; c11=cswap;
					}

					/* Interpolate colors */
					ex = (sdx & 0xffff);
					ey = (sdy & 0xffff);
					t1 = ((((c01.r - c00.r) * ex) >> 16) + c00.r) & 0xff;
					t2 = ((((c11.r - c10.r) * ex) >> 16) + c10.r) & 0xff;
					pc->r = (((t2 - t1) * ey) >> 16) + t1;
					t1 = ((((c01.g - c00.g) * ex) >> 16) + c00.g) & 0xff;
					t2 = ((((c11.g - c10.g) * ex) >> 16) + c10.g) & 0xff;
					pc->g = (((t2 - t1) * ey) >> 16) + t1;
					t1 = ((((c01.b - c00.b) * ex) >> 16) + c00.b) & 0xff;
					t2 = ((((c11.b - c10.b) * ex) >> 16) + c10.b) & 0xff;
					pc->b = (((t2 - t1) * ey) >> 16) + t1;
					t1 = ((((c01.a - c00.a) * ex) >> 16) + c00.a) & 0xff;
					t2 = ((((c11.a - c10.a) * ex) >> 16) + c10.a) & 0xff;
					pc->a = (((t2 - t1) * ey) >> 16) + t1;
				}
				sdx += icos;
				sdy += isin;
				pc++;
			}
			pc = (tColorRGBA *) ((uint8_t *) pc + gap);
		}

	} else {
		/*
		* Non-interpolating version
		*/
		for (y = 0; y < dst->h; y++) {
			dy = cy - y;
			sdx = (ax + (isin * dy)) + xd;
			sdy = (ay - (icos * dy)) + yd;
			for (x = 0; x < dst->w; x++) {
				dx = (short) (sdx >> 16);
				dy = (short) (sdy >> 16);
				if (flipx) dx = (src->w-1)-dx;
				if (flipy) dy = (src->h-1)-dy;
				if ((dx >= 0) && (dy >= 0) && (dx < src->w) && (dy < src->h)) {
					sp = (tColorRGBA *) ((uint8_t *) src->pixels + src->pitch * dy);
					sp += dx;
					*pc = *sp;
				}
				sdx += icos;
				sdy += isin;
				pc++;
			}
			pc = (tColorRGBA *) ((uint8_t *) pc + gap);
		}
	}
}


/*
Rotates and zooms 8 bit palette/Y 'src' surface to 'dst' surface without smoothing.

Rotates and zooms 8 bit RGBA/ABGR 'src' surface to 'dst' surface based on the control
parameters by scanning the destination surface.
Assumes src and dst surfaces are of 8 bit depth.
Assumes dst surface was allocated with the correct dimensions.

`src`   : Source surface.
`dst`   : Destination surface.
`cx`    : Horizontal center coordinate.
`cy`    : Vertical center coordinate.
`isin`  : Integer version of sine of angle.
`icos`  : Integer version of cosine of angle.
`flipx` : Flag indicating horizontal mirroring should be applied.
`flipy` : Flag indicating vertical mirroring should be applied.
*/
void _transformSurfaceY(SDL_Surface * src, SDL_Surface * dst, int cx, int cy, int isin, int icos, boolint flipx, boolint flipy)
{
	int x, y, dx, dy, xd, yd, sdx, sdy, ax, ay;
	tColorY *pc, *sp;
	int gap;

	/* Variable setup */
	xd = ((src->w - dst->w) << 15);
	yd = ((src->h - dst->h) << 15);
	ax = (cx << 16) - (icos * cx);
	ay = (cy << 16) - (isin * cx);
	pc = (tColorY*) dst->pixels;
	gap = dst->pitch - dst->w;
	/* Clear surface to colorkey (zero) */
	memset(pc, 0, dst->pitch * dst->h);

	/* Iterate through destination surface */
	for (y = 0; y < dst->h; y++) {
		dy = cy - y;
		sdx = (ax + (isin * dy)) + xd;
		sdy = (ay - (icos * dy)) + yd;
		for (x = 0; x < dst->w; x++) {
			dx = (short) (sdx >> 16);
			dy = (short) (sdy >> 16);
			if (flipx) dx = (src->w-1)-dx;
			if (flipy) dy = (src->h-1)-dy;
			if ((dx >= 0) && (dy >= 0) && (dx < src->w) && (dy < src->h)) {
				sp = (tColorY *) (src->pixels);
				sp += (src->pitch * dy + dx);
				*pc = *sp;
			}
			sdx += icos;
			sdy += isin;
			pc++;
		}
		pc += gap;
	}
}

/*
Rotates a 8/16/24/32 bit surface in increments of 90 degrees.

Specialized 90 degree rotator which rotates a 'src' surface in 90 degree
increments clockwise returning a new surface. Faster than rotozoomer since
no scanning or interpolation takes place. Input surface must be 8/16/24/32 bit.
(code contributed by J. Schiller, improved by C. Allport and A. Schiffler)

`src`               : Source surface to rotate.
`numClockwiseTurns` : Number of clockwise 90 degree turns to apply to the source.

Returns the new, rotated surface; or NULL for surfaces with incorrect input format.
*/
SDL_Surface* rotateSurface90Degrees(SDL_Surface* src, int numClockwiseTurns)
{
	int row, col, newWidth, newHeight;
	int bpp, bpr;
	SDL_Surface* dst;
	uint8_t* srcBuf;
	uint8_t* dstBuf;
	int normalizedClockwiseTurns;

	/* Has to be a valid surface pointer and be a Nbit surface where n is divisible by 8 */
	if (!src ||
	    !src->format) {
		debug(errShowBug, "NULL source surface or source surface format");
	    return NULL;
	}

	/* normalize numClockwiseTurns */
	normalizedClockwiseTurns = (numClockwiseTurns % 4);
	if (normalizedClockwiseTurns < 0) {
		normalizedClockwiseTurns += 4;
	}

	/* If turns are even, our new width/height will be the same as the source surface */
	if (normalizedClockwiseTurns % 2) {
		newWidth = src->h;
		newHeight = src->w;
	} else {
		newWidth = src->w;
		newHeight = src->h;
	}

	dst = SDL_CreateRGBSurface( src->flags, newWidth, newHeight, src->format->BitsPerPixel,
		src->format->Rmask,
		src->format->Gmask,
		src->format->Bmask,
		src->format->Amask);
	if (!dst) {
		debug(errShowBug, "Could not create destination surface");
		return NULL;
	}

	/* Calculate byte-per-pixel */
	bpp = src->format->BitsPerPixel / 8;

	switch(normalizedClockwiseTurns) {
	case 0: /* Make a copy of the surface */
		{
			if (src->pitch == dst->pitch) {
				/* If the pitch is the same for both surfaces, the memory can be copied all at once. */
				memcpy(dst->pixels, src->pixels, (src->h * src->pitch));
			}
			else
			{
				/* If the pitch differs, copy each row separately */
				srcBuf = (uint8_t*)(src->pixels);
				dstBuf = (uint8_t*)(dst->pixels);
				bpr = src->w * bpp;
				for (row = 0; row < src->h; row++) {
					memcpy(dstBuf, srcBuf, bpr);
					srcBuf += src->pitch;
					dstBuf += dst->pitch;
				}
			}
		}
		break;

		/* rotate clockwise */
	case 1: /* rotated 90 degrees clockwise */
		{
			for (row = 0; row < src->h; ++row) {
				srcBuf = (uint8_t*)(src->pixels) + (row * src->pitch);
				dstBuf = (uint8_t*)(dst->pixels) + (dst->w - row - 1) * bpp;
				for (col = 0; col < src->w; ++col) {
					memcpy (dstBuf, srcBuf, bpp);
					srcBuf += bpp;
					dstBuf += dst->pitch;
				}
			}
		}
		break;

	case 2: /* rotated 180 degrees clockwise */
		{
			for (row = 0; row < src->h; ++row) {
				srcBuf = (uint8_t*)(src->pixels) + (row * src->pitch);
				dstBuf = (uint8_t*)(dst->pixels) + ((dst->h - row - 1) * dst->pitch) + (dst->w - 1) * bpp;
				for (col = 0; col < src->w; ++col) {
					memcpy (dstBuf, srcBuf, bpp);
					srcBuf += bpp;
					dstBuf -= bpp;
				}
			}
		}
		break;

	case 3: /* rotated 270 degrees clockwise */
		{
			for (row = 0; row < src->h; ++row) {
				srcBuf = (uint8_t*)(src->pixels) + (row * src->pitch);
				dstBuf = (uint8_t*)(dst->pixels) + (row * bpp) + ((dst->h - 1) * dst->pitch);
				for (col = 0; col < src->w; ++col) {
					memcpy (dstBuf, srcBuf, bpp);
					srcBuf += bpp;
					dstBuf -= dst->pitch;
				}
			}
		}
		break;
	}
	/* end switch */

	return dst;
}

/* Correct x/y zoom to positive non-zero values. (Negative zoom means the image is flipped).

`zoomx`     : Pointer to the horizontal zoom factor.
`zoomy`     : Pointer to the vertical zoom factor.
*/
void _normaliseZoom(double *zoomx, double *zoomy)
{
	*zoomx = fabs(*zoomx);
	*zoomy = fabs(*zoomy);
	if (*zoomx < VALUE_LIMIT) *zoomx = VALUE_LIMIT;
	if (*zoomy < VALUE_LIMIT) *zoomy = VALUE_LIMIT;
}

/*
Internal target surface sizing function for rotozooms with trig result return.

`width`      : The source surface width.
`height`     : The source surface height.
`angle`      : The angle to rotate in degrees.
`zoomx`      : The horizontal scaling factor.
`zoomy`      : The vertical scaling factor.
`dstwidth`   : The calculated width of the destination surface.
`dstheight`  : The calculated height of the destination surface.
`canglezoom` : The sine of the angle adjusted by the zoom factor.
`sanglezoom` : The cosine of the angle adjusted by the zoom factor.
*/
void _rotozoomSurfaceSizeTrig(int width, int height, double angle, double zoomx, double zoomy,
	int *dstwidth, int *dstheight,
	double *canglezoom, double *sanglezoom)
{
	_normaliseZoom(&zoomx, &zoomy);

	if (fabs(angle) <= VALUE_LIMIT) {
		/* No rotation */
		*dstwidth = (int) floor(((double) width * zoomx) + 0.5);
		*dstwidth = MAX(*dstwidth, 1);
		*dstheight = (int) floor(((double) height * zoomy) + 0.5);
		*dstheight = MAX(*dstheight, 1);
		return;
	}

	double x, y, cx, cy, sx, sy;
	double radangle;
	int dstwidthhalf, dstheighthalf;

	/* Determine destination width and height by rotating a centered source box */
	radangle = angle * (M_PI / 180.0);
	*sanglezoom = sin(radangle);
	*canglezoom = cos(radangle);
	*sanglezoom *= zoomx;
	*canglezoom *= zoomy;
	x = (double)(width / 2);
	y = (double)(height / 2);
	cx = *canglezoom * x;
	cy = *canglezoom * y;
	sx = *sanglezoom * x;
	sy = *sanglezoom * y;

	dstwidthhalf = MAX((int)
		ceil(MAX(MAX(MAX(fabs(cx + sy), fabs(cx - sy)), fabs(-cx + sy)), fabs(-cx - sy))), 1);
	dstheighthalf = MAX((int)
		ceil(MAX(MAX(MAX(fabs(sx + cy), fabs(sx - cy)), fabs(-sx + cy)), fabs(-sx - cy))), 1);
	*dstwidth = 2 * dstwidthhalf;
	*dstheight = 2 * dstheighthalf;
}


/*
Returns the size of the resulting target surface for a rotozoomSurface() call.
The minimum size of the target surface is 1. The input factors can be positive or negative.

`width`     : The source surface width.
`height`    : The source surface height.
`angle`     : The angle to rotate in degrees.
`zoomx`     : The horizontal scaling factor.
`zoomy`     : The vertical scaling factor.
`dstwidth`  : The calculated width of the rotozoomed destination surface.
`dstheight` : The calculated height of the rotozoomed destination surface.
*/
void rotozoomSurfaceSize(int width, int height, double angle, double zoomx, double zoomy, int *dstwidth, int *dstheight)
{
	double dummy_sanglezoom, dummy_canglezoom;

	_rotozoomSurfaceSizeTrig(width, height, angle, zoomx, zoomy, dstwidth, dstheight, &dummy_sanglezoom, &dummy_canglezoom);
}

/*
Rotates and zooms a surface with different horizontal and vertival scaling factors and optional anti-aliasing.

Rotates and zooms a 32bit or 8bit 'src' surface to newly created 'dst' surface.
'angle' is the rotation in degrees, 'zoomx and 'zoomy' scaling factors. If 'smooth' is set
then the destination 32bit surface is anti-aliased. If the surface is not 8bit
or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.

`src`    : The surface to rotozoom.
`angle`  : The angle to rotate in degrees.
`zoomx`  : The horizontal scaling factor.
`zoomy`  : The vertical scaling factor.
`smooth` : Whether to antialias.

Returns the new rotozoomed surface.
*/
SDL_Surface *rotozoomSurface(SDL_Surface * src, double angle, double zoomx, double zoomy, boolint smooth)
{
	SDL_Surface *rz_dst = NULL;
	double zoominv;
	double sanglezoom, canglezoom, sanglezoominv, canglezoominv;
	int dstwidthhalf, dstwidth, dstheighthalf, dstheight;
	int is32bit;
	boolint flipx = (zoomx < 0.), flipy = (zoomy < 0.);
	_normaliseZoom(&zoomx, &zoomy);

	if (!src)
		return NULL;

	is32bit = (src->format->BitsPerPixel == 32);

	/* Determine target size */
	_rotozoomSurfaceSizeTrig(src->w, src->h, angle, zoomx, zoomy, &dstwidth, &dstheight, &canglezoom, &sanglezoom);

	/* Alloc space to completely contain the zoomed/rotated surface */
	if (is32bit) {
		rz_dst =
			SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight + GUARD_ROWS, 32,
			src->format->Rmask, src->format->Gmask,
			src->format->Bmask, src->format->Amask);
	} else {
		rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight + GUARD_ROWS, 8, 0, 0, 0, 0);
	}

	if (!rz_dst)
		return NULL;

	/* Adjust for guard rows */
	rz_dst->h = dstheight;

	/* Check if we have a rotozoom or just a zoom */
	if (fabs(angle) > VALUE_LIMIT) {
		/* ----------  angle != 0: full rotozoom ---------- */

		/* Calculate target factors from sin/cos and zoom */
		zoominv = 65536.0 / (zoomx * zoomx);
		sanglezoominv = sanglezoom;
		canglezoominv = canglezoom;
		sanglezoominv *= zoominv;
		canglezoominv *= zoominv;

		dstwidthhalf = dstwidth / 2;
		dstheighthalf = dstheight / 2;

		if (is32bit) {
			_transformSurfaceRGBA(src, rz_dst, dstwidthhalf, dstheighthalf,
				(int)sanglezoominv, (int)canglezoominv, flipx, flipy, smooth);
		} else {
			_transformSurfaceY(src, rz_dst, dstwidthhalf, dstheighthalf,
				(int)sanglezoominv, (int)canglezoominv, flipx, flipy);
		}

	} else {
		/* ------- angle == 0: Just a zoom  --------- */
		if (is32bit) {
			_zoomSurfaceRGBA(src, rz_dst, flipx, flipy, smooth);
		} else {
			_zoomSurfaceY(src, rz_dst, flipx, flipy);
		}
	}

	return rz_dst;
}