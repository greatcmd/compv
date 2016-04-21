/* Copyright (C) 2016 Doubango Telecom <https://www.doubango.org>
*
* This file is part of Open Source ComputerVision (a.k.a CompV) project.
* Source code hosted at https://github.com/DoubangoTelecom/compv
* Website hosted at http://compv.org
*
* CompV is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* CompV is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with CompV.
*/

/* @description
* This class computes image moments
* Some literature:
* Measuring Corner Properties: http://users.cs.cf.ac.uk/Paul.Rosin/corner2.pdf
*/
#include "compv/image/compv_image_moments.h"
#include "compv/compv_mathutils.h"
#include "compv/compv_debug.h"

COMPV_NAMESPACE_BEGIN()

double CompVImageMoments::cirPQ(const uint8_t* ptr, int patch_diameter, int center_x, int center_y, int img_width, int img_stride, int img_height, int p, int q)
{
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED(); // use cir00, cir01, cirM01M10...
	double mpq = 0;
	int patch_radius = (patch_diameter >> 1), img_y, i, j, dX, minI, maxI, minJ, maxJ;
	int patch_radius_pow2 = (patch_radius * patch_radius);
	double jq;
	const uint8_t* img_ptr;

	// Compute minJ and maxJ
	minJ = -patch_radius;
	if ((center_y + minJ) < 0) {
		minJ = -center_y;
	}
	maxJ = +patch_radius;
	if ((center_y + maxJ) >= img_height) {
		maxJ = (img_height - center_y - 1);
		maxJ = COMPV_MATH_CLIP3(minJ, patch_radius, maxJ);
	}

	for (j = minJ, img_y = (center_y + j); j <= maxJ; ++j, ++img_y) {
		jq = pow(j, q); // y**q

		// Pythagorean theorem: x = sqrt(r**2 - y**2)
		dX = ((int)sqrt(patch_radius_pow2 - (j * j))); // TODO: Compute once

		// Compute minI and maxI
		minI = -dX;
		if ((center_x + minI) < 0) {
			minI = -center_x;
		}
		maxI = +dX;
		if ((center_x + maxI) >= img_width) {
			maxI = (img_width - center_x - 1);
			maxI = COMPV_MATH_CLIP3(minI, dX, maxI);
		}

		img_ptr = &ptr[(img_y * img_stride) + (center_x + minI)];
		for (i = minI; i <= maxI; ++i, ++img_ptr) {
			mpq += pow(i, p) * jq * *img_ptr; // i^p * j^q * I(x, y)
		}
	}
	return (mpq);
}

void CompVImageMoments::cirM01M10(const uint8_t* ptr, int patch_diameter, int center_x, int center_y, int img_width, int img_stride, int img_height, double* m01, double* m10)
{
	double s01 = 0, s10 = 0;
	int patch_radius = (patch_diameter >> 1), img_y, i, j, dX, minI, maxI, minJ, maxJ;
	int patch_radius_pow2 = (patch_radius * patch_radius);
	const uint8_t* img_ptr;

	// Compute minJ and maxJ
	minJ = -patch_radius;
	if ((center_y + minJ) < 0) {
		minJ = -center_y;
	}
	maxJ = +patch_radius;
	if ((center_y + maxJ) >= img_height) {
		maxJ = (img_height - center_y - 1);
		maxJ = COMPV_MATH_CLIP3(minJ, patch_radius, maxJ);
	}

	for (j = minJ, img_y = (center_y + j); j <= maxJ; ++j, ++img_y) {
		// Pythagorean theorem: x = sqrt(r**2 - y**2)
		dX = ((int)sqrt(patch_radius_pow2 - (j * j))); // TODO: Compute once

		// Compute minI and maxI
		minI = -dX;
		if ((center_x + minI) < 0) {
			minI = -center_x;
		}
		maxI = +dX;
		if ((center_x + maxI) >= img_width) {
			maxI = (img_width - center_x - 1);
			maxI = COMPV_MATH_CLIP3(minI, dX, maxI);
		}

		img_ptr = &ptr[(img_y * img_stride) + (center_x + minI)];
		for (i = minI; i <= maxI; ++i, ++img_ptr) {
			s10 += (i * *img_ptr); // i^p * j^q * I(x, y) = i^1 * j^0 * I(x, y) = i * I(x, y)
			s01 += j * *img_ptr; // i^p * j^q * I(x, y) = i^0 * j^1 * I(x, y) = j * I(x, y)
		}
	}
	*m01 = s01;
	*m10 = s10;
}

COMPV_NAMESPACE_END()