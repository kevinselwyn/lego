/*
 * LEGO® Enhanced Graphics Object, v1.0.1
 * Copyright (C) 2014, Kevin Selwyn <kevinselwyn@gmail.com>
 *
 *
 * This file is part of LEGO® Enhanced Graphics Object.
 *
 * LEGO® Enhanced Graphics Object is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * LEGO® Enhanced Graphics Object is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * LEGO® Enhanced Graphics Object.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * LEGO® is a trademark of the LEGO Group of companies which does not sponsor,
 * authorize or endorse this project.
 */

#include <stdio.h>
#include <stdlib.h>
#include "lego.h"

uint32_t hash(char *data, uint32_t length, uint32_t seed) {
	int i = 0, l = length / 4;
	const uint32_t *chunks = NULL;
	const uint8_t *tail = NULL;
	uint32_t c1 = 0xCC9E2D51, c2 = 0x1B873593;
	uint32_t r1 = 15, r2 = 13;
	uint32_t m = 5, n = 0xE6546B64, h = 0, k = 0;
	uint8_t *d = (uint8_t *) data;

	h = seed;
	chunks = (const uint32_t *) (d + l * 4);
	tail = (const uint8_t *) (d + l * 4);

	for (i = -l; i != 0; ++i) {
		k = chunks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		h ^= k;
		h = (h << r2) | (h >> (32 - r2));
		h = h * m + n;
	}

	k = 0;

	switch (length & 3) {
	case 3:
		k ^= (tail[2] << 16);
		break;
	case 2:
		k ^= (tail[1] << 8);
		break;
	case 1:
		k ^= tail[0];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;
		h ^= k;
		break;
	default:
		break;
	}

	h ^= length;
	h ^= (h >> 16);
	h *= 0x85EBCA6B;
	h ^= (h >> 13);
	h *= 0xC2B2AE35;
	h ^= (h >> 16);

	return h;
}