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
#include <string.h>
#include "lego.h"

struct coords find_origin(int width, int height) {
	int x = 0, y = 0;
	struct coords coords = { 0, 0 };

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (canvas[x][y] == -1) {
				coords.x = x;
				coords.y = y;

				return coords;
			}
		}
	}

	return coords;
}

int decode(char *input, char *output, struct lego lego) {
	int rc = 0, x = 0, y = 0, i = 0, l = 0;
	int width = lego.width, height = lego.height;
	int brick_width = 1, brick_height = 1;
	char *ext = NULL;
	unsigned char *data = NULL;
	struct coords coords;
	struct BRKS BRKS = lego.BRKS;
	struct brick brick;
	struct brick_def brick_def;
	struct color_def color_def;
	struct image image;

	if (!input) {
		printf("No filename provided\n");

		rc = 1;
		goto cleanup;
	}

	if (!BRKS.count) {
		printf("No bricks to decode\n");

		rc = 1;
		goto cleanup;
	}

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			canvas[x][y] = -1;
		}
	}

	for (i = 0, l = BRKS.count; i < l; i++) {
		brick = BRKS.bricks[i];
		brick_def = get_brick_by_id(brick.id);

		brick_width = brick_def.width;
		brick_height = brick_def.height;

		if (brick.rotation == ROTATION_90 || brick.rotation == ROTATION_270) {
			brick_width = brick_def.height;
			brick_height = brick_def.width;
		}

		coords = find_origin(width, height);

		for (y = coords.y; y < coords.y + brick_height; y++) {
			for (x = coords.x; x < coords.x + brick_width; x++) {
				canvas[x][y] = brick.color;
			}
		}
	}

	data = malloc(sizeof(char) * (width * height * 3) + 1);

	for (y = 0, i = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			color_def = get_color_by_id(canvas[x][y]);

			data[i++] = color_def.red;
			data[i++] = color_def.green;
			data[i++] = color_def.blue;
		}
	}

	image.data = data;
	image.width = width;
	image.height = height;

	if (!output) {
		output = malloc(sizeof(char) * strlen(input) + 1);
		strcpy(output, input);
		ext = strrchr(output, '.');
		output[strlen(output) - strlen(ext)] = '\0';
		strcat(output, ".png");

		rc = write_png(image, output);
		free(output);

		goto cleanup;
	}

	if (strstr(output, ".jpg") || strstr(output, ".jpeg")) {
		rc = write_jpeg(image, output);
	} else if (strstr(output, ".png")) {
		rc = write_png(image, output);
	} else {
		printf("Unsupported file format %s\n", output);

		rc = 1;
		goto cleanup;
	}

cleanup:
	if (data) {
		free(data);
	}

	return rc;
}