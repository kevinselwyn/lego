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
#include <math.h>
#include "lego.h"

int prompt(char **variable, char *msg, size_t size) {
	char *pos = NULL;

	*variable = malloc(sizeof(char) * (size + 1));

	printf("%s: ", msg);
	if (fgets(*variable, size, stdin) == NULL) {
		printf("Read error\n");
		return 0;
	}

	if ((pos = strchr(*variable, '\n')) != NULL) {
		*pos = '\0';
	}

	return strlen(*variable);
}

struct INFO setINFO() {
	int length = 0;
	struct INFO INFO;

	INFO.name_length = prompt(&INFO.name, "Image name", 256);
	INFO.version_length = prompt(&INFO.version, "Version", 256);
	INFO.description_length = prompt(&INFO.description, "Description", 256);
	printf("Author\n");
	INFO.author.name_length = prompt(&INFO.author.name, "  Name", 256);
	INFO.author.email_length = prompt(&INFO.author.email, "  Email", 256);
	INFO.author.url_length = prompt(&INFO.author.url, "  URL", 256);

	length += INFO.name_length + INFO.version_length + INFO.description_length;
	length += INFO.author.name_length + INFO.author.email_length + INFO.author.url_length;

	INFO.length = length;

	return INFO;
}

void writeSubBlock(char **data, char *signature, int length, char *str) {
	char *ch = NULL;

	if (!length) {
		return;
	}

	ch = malloc(sizeof(char) * 1 + 1);

	strcat(*data, signature);
	ch[0] = length;
	strcat(*data, ch);
	strcat(*data, str);

	free(ch);
}

int INFO2data(char **data, struct INFO INFO) {
	int length = 0;

	*data = "";
	length = INFO.length;

	length += (INFO.name_length) ? 3 : 0;
	length += (INFO.version_length) ? 3 : 0;
	length += (INFO.description_length) ? 3 : 0;
	length += (INFO.author.name_length) ? 3 : 0;
	length += (INFO.author.email_length) ? 3 : 0;
	length += (INFO.author.url_length) ? 3 : 0;

	*data = malloc(sizeof(char) * length + 1);

	writeSubBlock(&*data, "NM", INFO.name_length, INFO.name);
	writeSubBlock(&*data, "VR", INFO.version_length, INFO.version);
	writeSubBlock(&*data, "DS", INFO.description_length, INFO.description);
	writeSubBlock(&*data, "AN", INFO.author.name_length, INFO.author.name);
	writeSubBlock(&*data, "AE", INFO.author.email_length, INFO.author.email);
	writeSubBlock(&*data, "AU", INFO.author.url_length, INFO.author.url);

	return length;
}

int writeBlock(FILE **output_file, struct block block) {
	int rc = 0, write_length = 8 + block.length + 4, i = 0, l = 0;
	unsigned char *output = malloc(sizeof(char) * write_length);
	uint32_t hashed = hash(block.data, block.length, 0);

	for (i = 0, l = 4; i < l; i++) {
		output[i] = block.signature[i];
	}

	output[4] = (block.length >> 24) & 0xFF;
	output[5] = (block.length >> 16) & 0xFF;
	output[6] = (block.length >> 8) & 0xFF;
	output[7] = block.length & 0xFF;

	for (i = 0, l = block.length; i < l; i++) {
		output[i + 8] = block.data[i];
	}

	i += 8;

	output[i++] = (hashed >> 24) & 0xFF;
	output[i++] = (hashed >> 16) & 0xFF;
	output[i++] = (hashed >> 8) & 0xFF;
	output[i++] = hashed & 0xFF;

	if (fwrite(output, 1, 8 + block.length + 4, *output_file) != write_length) {
		printf("Could not write %s block\n", block.signature);

		rc = 1;
		goto cleanup;
	}

cleanup:
	if (output) {
		free(output);
	}

	return rc;
}

int encode(char *input, char *output) {
	int rc = 0, i = 0, l = 0, x = 0, y = 0;
	int r = 0, g = 0, b = 0, counter = 0, brick_width = 0, total_width = 0;
	char *ext = NULL;
	FILE *output_file = NULL;
	struct lego lego;
	struct INFO INFO;
	struct block blockINFO;
	struct BRKS BRKS;
	struct block blockBRKS;
	struct image image;
	struct brick brick;

	if (!input) {
		printf("No input filename provided\n");

		rc = 1;
		goto cleanup;
	}

	if (strstr(input, ".jpg") || strstr(input, ".jpeg")) {
		image = read_jpeg(input);
	} else if (strstr(input, ".png")) {
		image = read_png(input);
	} else {
		printf("Unsupported file format %s\n", output);

		rc = 1;
		goto cleanup;
	}

	if (!output) {
		output = malloc(sizeof(char) * strlen(input) + 2);
		strcpy(output, input);
		ext = strrchr(output, '.');
		output[strlen(output) - strlen(ext)] = '\0';
		strcat(output, ".lego");

		output_file = fopen(output, "w+");
		free(output);
	} else {
		output_file = fopen(output, "w+");
	}

	if (!output_file) {
		printf("Could not open %s\n", output);

		rc = 1;
		goto cleanup;
	}

	lego.header = NULL;
	lego.signature = "LEG0";
	lego.version = 1.0;
	lego.width = image.width;
	lego.height = image.height;

	lego.header = malloc(sizeof(char) * (16 + 1));
	for (i = 0, l = 4; i < l; i++) {
		lego.header[i] = lego.signature[i];
	}

	lego.header[4] = (int)floor(lego.version);
	lego.header[5] = 0;

	for (i = 0, l = 6; i < l; i++) {
		lego.header[6 + i] = 0; // lego.header[6 - 11] RESERVED
	}

	lego.header[12] = (lego.width >> 8) & 0xFF;
	lego.header[13] = lego.width & 0xFF;
	lego.header[14] = (lego.height >> 8) & 0xFF;
	lego.header[15] = lego.height & 0xFF;

	if (fwrite(lego.header, 1, 16, output_file) != 16) {
		printf("Could not write header to %s\n", output);

		rc = 1;
		goto cleanup;
	}

	INFO = setINFO();
	blockINFO.signature = "INFO";
	blockINFO.length = INFO2data(&blockINFO.data, INFO);

	if (writeBlock(&output_file, blockINFO)) {
		printf("Could not write INFO block\n");

		rc = 1;
		goto cleanup;
	}

	for (i = 0, l = image.width * image.height * 3, counter = 0, x = 0, y = 0; i < l; i += 3, counter++, x++) {
		r = image.data[i];
		g = image.data[i + 1];
		b = image.data[i + 2];

		if (counter != 0 && counter % image.width == 0) {
			x = 0;
			y++;
		}

		canvas[x][y] = get_closest_color(r, g, b);
	}

	for (y = 0, counter = 0; y < image.height; y++) {
		x = 0;
		total_width = 0;

		while (total_width < image.width) {
			brick_width = 1;

			brick.color = canvas[x][y];

			while (canvas[x][y] == canvas[x + 1][y]) {
				x++;
				brick_width += 1;
			}

			x++;

			brick.id = get_brick_by_size(brick_width, 1);

			while (brick.id == -1) {
				x--;
				brick_width--;

				brick.id = get_brick_by_size(brick_width, 1);

				if (x < 0) {
					break;
				}
			}

			total_width += brick_width;

			brick.rotation = 0;

			BRKS.bricks[counter++] = brick;

			if (counter >= MAX_BRICKS) {
				printf("Too many bricks\n");

				rc = 1;
				goto cleanup;
			}
		}
	}

	BRKS.count = counter;

	blockBRKS.length = BRKS.count * 3;
	blockBRKS.data = malloc(sizeof(char) * blockBRKS.length + 1);
	blockBRKS.signature = "BRKS";

	for (i = 0, l = BRKS.count, counter = 0; i < l; i++) {
		blockBRKS.data[counter++] = BRKS.bricks[i].id;
		blockBRKS.data[counter++] = BRKS.bricks[i].color;
		blockBRKS.data[counter++] = BRKS.bricks[i].rotation;
	}

	if (writeBlock(&output_file, blockBRKS)) {
		printf("Could not write BRKS block\n");

		rc = 1;
		goto cleanup;
	}

cleanup:

	if (blockBRKS.data) {
		free(blockBRKS.data);
	}

	if (output_file) {
		fclose(output_file);
	}

	return rc;
}