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
#include <inttypes.h>
#include <math.h>
#include "lego.h"
#include "parse.h"

void validate_header(struct lego lego, int verbose) {
	int accum = 0, i = 0, l = 0;
	char *signature, *reserved;

	signature = lego.signature;
	reserved = lego.reserved;

	if (strcmp(signature, "LEG0") != 0) {
		printf("Invalid signature in header\n");
		exit(1);
	}

	for (i = 0, l = 5; i < l; i++) {
		accum += reserved[i];
	}

	if (accum != 0) {
		printf("Invalid reserved bytes in header\n");
		exit(1);
	}

	if (!lego.version) {
		printf("Missing header version\n");
		exit(1);
	}

	if (!lego.width || !lego.height) {
		printf("Missing image dimensions\n");
		exit(1);
	}

	if (verbose) {
		printf("Valid header, Version %g\n", lego.version);
		printf("Dimensions: %d x %d\n\n", lego.width, lego.height);
	}
}

int validate_hash(char *data, int length, char *hash_str) {
	int rc = 1;
	int old_byte_00 = 0, old_byte_01 = 0, old_byte_02 = 0, old_byte_03 = 0;
	int new_byte_00 = 0, new_byte_01 = 0, new_byte_02 = 0, new_byte_03 = 0;
	uint32_t new_hash = hash(data, length, 0);

	old_byte_00 = hash_str[0] & 0xFF;
	old_byte_01 = hash_str[1] & 0xFF;
	old_byte_02 = hash_str[2] & 0xFF;
	old_byte_03 = hash_str[3] & 0xFF;

	new_byte_00 = (new_hash >> 24) & 0xFF;
	new_byte_01 = (new_hash >> 16) & 0xFF;
	new_byte_02 = (new_hash >> 8) & 0xFF;
	new_byte_03 = new_hash & 0xFF;

	if (old_byte_00 != new_byte_00 || old_byte_01 != new_byte_01 || old_byte_02 != new_byte_02 || old_byte_03 != new_byte_03) {
		printf("Expected: 0x%02x 0x%02x 0x%02x 0x%02x\n", new_byte_00, new_byte_01, new_byte_02, new_byte_03);
		printf("Found:    0x%02x 0x%02x 0x%02x 0x%02x\n", old_byte_00, old_byte_01, old_byte_02, old_byte_03);

		rc = 0;
	}

	return rc;
}

struct brick_def get_brick_by_id(int id) {
	int i = 0, l = 0;
	struct brick_def brick;

	for (i = 0, l = brick_def_pos; i < l; i++) {
		if (bricks[i].id == id) {
			brick = bricks[i];
		}
	}

	return brick;
}

int get_brick_by_size(int width, int height) {
	int index = -1, i = 0, l = 0;

	for (i = 0, l = brick_def_pos; i < l; i++) {
		if (bricks[i].width == width && bricks[i].height == height) {
			index = i;
			i = l;
		}
	}

	return index;
}

struct color_def get_color_by_id(int id) {
	int i = 0, l = 0;
	struct color_def color;

	for (i = 0, l = color_def_pos; i < l; i++) {
		if (colors[i].id == id) {
			color = colors[i];
		}
	}

	return color;
}

double color_distance(int r1, int g1, int b1, int r2, int g2, int b2) {
	return sqrt(pow(r1 - r2, 2) + pow(g1 - g2, 2) + pow(b1 - b2, 2));
}

int get_closest_color(int r, int g, int b) {
	int id = -1, i = 0, l = 0;
	double lowest_distance = 1000.0, new_distance = 0.0;

	for (i = 0, l = color_def_pos; i < l; i++) {
		new_distance = color_distance(r, g, b, colors[i].red, colors[i].green, colors[i].blue);

		if (new_distance < lowest_distance) {
			lowest_distance = new_distance;
			id = colors[i].id;
		}
	}

	return id;
}

void parseINFO(struct INFO *INF, struct block *bloc) {
	struct INFO INFO = *INF;
	struct block block = *bloc;
	int length = 0, i = 0, j = 0, k = 0;

	while (i < block.length) {
		if (block.data[i] == 'N' && block.data[i + 1] == 'M') {
			i += 2;

			length = block.data[i++];
			INFO.name = malloc(sizeof(char) * length + 1);

			for (j = 0, k = length; j < k; j++) {
				INFO.name[j] = block.data[j + i];
			}

			INFO.name[j] = '\0';
			i += length;
			continue;
		}

		if (block.data[i] == 'V' && block.data[i + 1] == 'R') {
			i += 2;

			length = block.data[i++];
			INFO.version = malloc(sizeof(char) * length + 1);

			for (j = 0, k = length; j < k; j++) {
				INFO.version[j] = block.data[j + i];
			}

			INFO.version[j] = '\0';
			i += length;
			continue;
		}

		if (block.data[i] == 'D' && block.data[i + 1] == 'S') {
			i += 2;

			length = block.data[i++];
			INFO.description = malloc(sizeof(char) * length + 1);

			for (j = 0, k = length; j < k; j++) {
				INFO.description[j] = block.data[j + i];
			}

			INFO.description[j] = '\0';
			i += length;
			continue;
		}

		if (block.data[i] == 'A' && block.data[i + 1] == 'N') {
			i += 2;

			length = block.data[i++];
			INFO.author.name = malloc(sizeof(char) * length + 1);

			for (j = 0, k = length; j < k; j++) {
				INFO.author.name[j] = block.data[j + i];
			}

			INFO.author.name[j] = '\0';
			i += length;
			continue;
		}

		if (block.data[i] == 'A' && block.data[i + 1] == 'E') {
			i += 2;

			length = block.data[i++];
			INFO.author.email = malloc(sizeof(char) * length + 1);

			for (j = 0, k = length; j < k; j++) {
				INFO.author.email[j] = block.data[j + i];
			}

			INFO.author.email[j] = '\0';
			i += length;
			continue;
		}

		if (block.data[i] == 'A' && block.data[i + 1] == 'U') {
			i += 2;

			length = block.data[i++];
			INFO.author.url = malloc(sizeof(char) * length + 1);

			for (j = 0, k = length; j < k; j++) {
				INFO.author.url[j] = block.data[j + i];
			}

			INFO.author.url[j] = '\0';
			i += length;
			continue;
		}
	}

	INFO.length = length;

	*INF = INFO;
	*bloc = block;
}

void dumpINFO(struct INFO INFO) {
	printf("INFO Block:\n\n");

	if (INFO.name) {
		printf("Name:        %s\n", INFO.name);
	}

	if (INFO.version) {
		printf("Version:     %s\n", INFO.version);
	}

	if (INFO.description) {
		printf("Description: %s\n", INFO.description);
	}

	if (INFO.author.name || INFO.author.email || INFO.author.url) {
		printf("Author:\n");

		if (INFO.author.name) {
			printf("  Name:      %s\n", INFO.author.name);
		}

		if (INFO.author.email) {
			printf("  Email:     %s\n", INFO.author.email);
		}

		if (INFO.author.url) {
			printf("  URL:       %s\n", INFO.author.url);
		}
	}

	printf("\n");
}

void parseBRKS(struct BRKS *BRK, struct block *bloc) {
	struct BRKS BRKS = *BRK;
	struct block block = *bloc;
	int counter = 0, scrubber = 0, length = block.length;
	char *data = block.data;

	while (scrubber < length) {
		BRKS.bricks[counter].id = data[scrubber + 0];
		BRKS.bricks[counter].color = data[scrubber + 1];
		BRKS.bricks[counter].rotation = data[scrubber + 2];

		scrubber += 3;
		counter++;
	}

	BRKS.count = counter;

	*BRK = BRKS;
	*bloc = block;
}

void dumpBRKS(struct BRKS BRKS) {
	int i = 0, l = 0;
	struct brick_def brick;
	struct color_def color;

	printf("BRKS Block:\n\n");

	for (i = 0, l = BRKS.count; i < l; i++) {
		brick = get_brick_by_id(BRKS.bricks[i].id);
		color = get_color_by_id(BRKS.bricks[i].color);

		printf("Brick %d\n", (i + 1));
		printf("  Brick:    %d (%s)\n", brick.id, brick.name);
		printf("  Color:    %d (%s, #%02x%02x%02x)\n", color.id, color.name, color.red, color.green, color.blue);
		printf("  Rotation: %d (%s)\n\n", BRKS.bricks[i].rotation, rotations[BRKS.bricks[i].rotation]);
	}
}

void addNCLR(struct NCLR NCLR) {
	int i = 0, l = 0;

	for (i = 0, l = NCLR.count; i < l; i++) {
		colors[color_def_pos].id = NCLR.colors[i].id;
		colors[color_def_pos].red = NCLR.colors[i].red;
		colors[color_def_pos].green = NCLR.colors[i].green;
		colors[color_def_pos].blue = NCLR.colors[i].blue;
		colors[color_def_pos].name = NCLR.colors[i].name;

		if (NCLR.colors[i].id > color_def_pos) {
			color_def_pos = NCLR.colors[i].id;
		}

		color_def_pos++;
	}
}

void parseNCLR(struct NCLR *NCL, struct block *bloc) {
	struct NCLR NCLR = *NCL;
	struct block block = *bloc;
	int counter = 0, scrubber = 0, length = block.length, name_length = 0, i = 0, l = 0;
	char *data = block.data;

	while (scrubber < length) {
		NCLR.colors[counter].id = data[scrubber + 0];
		NCLR.colors[counter].red = data[scrubber + 1];
		if (data[scrubber + 1] < 0) {
			NCLR.colors[counter].red += 256;
		}
		NCLR.colors[counter].green = data[scrubber + 2];
		if (data[scrubber + 2] < 0) {
			NCLR.colors[counter].green += 256;
		}
		NCLR.colors[counter].blue = data[scrubber + 3];
		if (data[scrubber + 3] < 0) {
			NCLR.colors[counter].blue += 256;
		}

		scrubber += 4;
		name_length = data[scrubber++];

		NCLR.colors[counter].name = malloc(sizeof(char) * name_length + 1);
		for (i = 0, l = name_length; i < l; i++) {
			NCLR.colors[counter].name[i] = data[scrubber++];
		}
		NCLR.colors[counter].name[i] = '\0';

		counter++;
	}

	NCLR.count = counter;

	addNCLR(NCLR);

	*NCL = NCLR;
	*bloc = block;
}

void dumpNCLR(struct NCLR NCLR) {
	int i = 0, l = 0;

	printf("NCLR Block:\n\n");

	for (i = 0, l = NCLR.count; i < l; i++) {
		printf("Color %d\n", (i + 1));
		printf("  Color ID:    %d\n", NCLR.colors[i].id);
		printf("  Color Red:   %d\n", NCLR.colors[i].red);
		printf("  Color Green: %d\n", NCLR.colors[i].green);
		printf("  Color Blue:  %d\n", NCLR.colors[i].blue);
		printf("  Color Name:  %s\n\n", NCLR.colors[i].name);
	}
}

void dumpColors() {
	int i = 0;

	for (i = 0; i < color_def_pos; i++) {
		printf("Color %d\n", (i + 1));
		printf("  Color ID:    %d\n", colors[i].id);
		printf("  Color Red:   %d\n", colors[i].red);
		printf("  Color Green: %d\n", colors[i].green);
		printf("  Color Blue:  %d\n", colors[i].blue);
		printf("  Color Name:  %s\n\n", colors[i].name);
	}
}

void addNBRK(struct NBRK NBRK) {
	int i = 0, l = 0;

	for (i = 0, l = NBRK.count; i < l; i++) {
		bricks[brick_def_pos].id = NBRK.bricks[i].id;
		bricks[brick_def_pos].width = NBRK.bricks[i].width;
		bricks[brick_def_pos].height = NBRK.bricks[i].height;
		bricks[brick_def_pos].name = NBRK.bricks[i].name;

		if (NBRK.bricks[i].id > brick_def_pos) {
			brick_def_pos = NBRK.bricks[i].id;
		}

		brick_def_pos++;
	}
}

void parseNBRK(struct NBRK *NBR, struct block *bloc) {
	struct NBRK NBRK = *NBR;
	struct block block = *bloc;
	int counter = 0, scrubber = 0, length = block.length, name_length = 0, i = 0, l = 0;
	char *data = block.data;

	while (scrubber < length) {
		NBRK.bricks[counter].id = data[scrubber++];
		NBRK.bricks[counter].width = data[scrubber++];
		NBRK.bricks[counter].height = data[scrubber++];

		name_length = data[scrubber++];

		NBRK.bricks[counter].name = malloc(sizeof(char) * name_length + 1);
		for (i = 0, l = 7; i < l; i++) {
			NBRK.bricks[counter].name[i] = data[scrubber++];
		}
		NBRK.bricks[counter].name[i] = '\0';

		counter++;
	}

	NBRK.count = counter;

	addNBRK(NBRK);

	*NBR = NBRK;
	*bloc = block;
}

void dumpNBRK(struct NBRK NBRK) {
	int i = 0, l = 0;

	printf("NBRK Block:\n\n");

	for (i = 0, l = NBRK.count; i < l; i++) {
		printf("Brick %d\n", (i + 1));
		printf("  Brick ID: %d\n", NBRK.bricks[i].id);
		printf("  Brick width: %d\n", NBRK.bricks[i].width);
		printf("  Brick height: %d\n", NBRK.bricks[i].height);
		printf("  Brick name: %s\n\n", NBRK.bricks[i].name);
	}
}

void dumpBricks() {
	int i = 0;

	for (i = 0; i < brick_def_pos; i++) {
		printf("Brick %d\n", (i + 1));
		printf("  Brick ID: %d\n", bricks[i].id);
		printf("  Brick width: %d\n", bricks[i].width);
		printf("  Brick height: %d\n", bricks[i].height);
		printf("  Brick name: %s\n\n", bricks[i].name);
	}
}

void listBricks(struct BRKS BRKS) {
	int id = 0, length = 0, in_array = -1, i = 0, j = 0, k = 0, l = 0;
	struct color_def color;
	struct brick_def brick;

	printf("Total bricks: %d\n", BRKS.count);

	for (i = 0, j = BRKS.count; i < j; i++) {
		id = (BRKS.bricks[i].id * 256) + BRKS.bricks[i].color;

		brick = get_brick_by_id(BRKS.bricks[i].id);
		color = get_color_by_id(BRKS.bricks[i].color);

		in_array = -1;
		for (k = 0, l = combo_pos; k < l; k++) {
			if (combos[k].id == id) {
				in_array = k;
			}
		}

		if (in_array == -1) {
			length = strlen(brick.name) + strlen(color.name) + 2;

			combos[combo_pos].name = malloc(sizeof(char) * length + 1);
			strcpy(combos[combo_pos].name, brick.name);
			strcat(combos[combo_pos].name, ", ");
			strcat(combos[combo_pos].name, color.name);

			combos[combo_pos].id = id;
			combos[combo_pos].total = 1;

			combo_pos++;
		} else {
			combos[in_array].total++;
		}
	}

	printf("Individual bricks: %d\n", combo_pos);

	printf("Brick list:\n");

	for (i = 0, l = combo_pos; i < l; i++) {
		printf("\t(%d) %s\n", combos[i].total, combos[i].name);
		free(combos[i].name);
	}
}

void parse(struct lego *leg, char *filename, int verbose, char *which) {
	struct lego lego = *leg;
	int rc = 0, filesize = 0, scrubber = 0, counter = 0, i = 0, l = 0;
	unsigned char *filedata = NULL;
	FILE *file = NULL;

	file = fopen(filename, "rb");

	if (!file) {
		printf("Could not open %s\n", filename);

		rc = 1;
		goto cleanup;
	}

	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (!filesize) {
		printf("%s is empty\n", filename);

		rc = 1;
		goto cleanup;
	}

	filedata = malloc(sizeof(char) * filesize + 1);

	if (!filedata) {
		printf("Memory error\n");

		rc = 1;
		goto cleanup;
	}

	if (fread(filedata, 1, filesize, file) != (unsigned)filesize) {
		printf("Could not read %s\n", filename);

		rc = 1;
		goto cleanup;
	}

	lego.header = malloc(sizeof(char) * 16 + 1);
	for (i = 0, l = 16; i < l; i++) {
		lego.header[i] = filedata[i];
	}
	lego.header[i] = '\0';

	lego.signature = malloc(sizeof(char) * 4 + 1);
	for (i = 0, l = 4; i < l; i++) {
		lego.signature[i] = lego.header[i];
	}
	lego.signature[i] = '\0';

	lego.version = filedata[4];

	if (filedata[5] < 10) {
		lego.version += (double)filedata[5] / 10;
	} else if (filedata[5] < 100) {
		lego.version += (double)filedata[5] / 100;
	} else {
		lego.version += (double)filedata[5] / 1000;
	}

	lego.reserved = malloc(sizeof(char) * 6 + 1);
	for (i = 0, l = 5; i < l; i++) {
		lego.reserved[i] = lego.header[6 + i];
	}
	lego.reserved[i] = '\0';

	lego.width = (filedata[12] * 256) + filedata[13];
	lego.height = (filedata[14] * 256) + filedata[15];

	validate_header(lego, verbose);

	scrubber += 16;
	counter = 0;

	while (scrubber < filesize) {
		lego.blocks[counter].signature = malloc(sizeof(char) * 4 + 1);
		for (i = 0, l = 4; i < l; i++) {
			lego.blocks[counter].signature[i] = filedata[scrubber + i];
		}
		lego.blocks[counter].signature[i] = '\0';

		scrubber += 4;

		lego.blocks[counter].length = 0;

		for (i = 0, l = 4; i < l; i++) {
			lego.blocks[counter].length += (unsigned int)filedata[scrubber + i] * pow(256, 3 - i);
		}

		scrubber += 4;

		lego.blocks[counter].data = malloc(sizeof(char) * lego.blocks[counter].length + 1);
		for (i = 0, l = lego.blocks[counter].length; i < l; i++) {
			lego.blocks[counter].data[i] = filedata[scrubber + i];
		}
		lego.blocks[counter].data[i] = '\0';

		scrubber += lego.blocks[counter].length;

		lego.blocks[counter].hash = malloc(sizeof(char) * 4 + 1);
		for (i = 0, l = 4; i < l; i++) {
			lego.blocks[counter].hash[i] = filedata[scrubber + i];
		}
		lego.blocks[counter].hash[i] = '\0';

		scrubber += 4;

		if (!validate_hash(lego.blocks[counter].data, lego.blocks[counter].length, lego.blocks[counter].hash)) {
			printf("Corrupted %s block\n", lego.blocks[counter].signature);

			rc = 1;
			goto cleanup;
		}

		counter += 1;
	}

	for (i = 0, l = counter; i < l; i++) {
		if (strcmp(lego.blocks[i].signature, "INFO") == 0) {
			if (which && strcmp(which, "INFO") != 0) {
				continue;
			}

			parseINFO(&lego.INFO, &lego.blocks[i]);
			if (verbose) {
				if (lego.INFO.length) {
					dumpINFO(lego.INFO);
				} else {
					printf("Empty INFO block\n");
				}
			}

			if (which && strcmp(which, "INFO") == 0) {
				goto cleanup;
			}
		} else if (strcmp(lego.blocks[i].signature, "BRKS") == 0) {
			if (which && strcmp(which, "BRKS") != 0) {
				continue;
			}

			parseBRKS(&lego.BRKS, &lego.blocks[i]);
			if (verbose) {
				if (lego.BRKS.count) {
					dumpBRKS(lego.BRKS);
				} else {
					printf("Empty BRKS block\n");
				}
			}

			if (which && strcmp(which, "BRKS") == 0) {
				goto cleanup;
			}
		} else if (strcmp(lego.blocks[i].signature, "NCLR") == 0) {
			if (which && strcmp(which, "NCLR") != 0) {
				continue;
			}

			parseNCLR(&lego.NCLR, &lego.blocks[i]);
			if (verbose) {
				if (lego.NCLR.count) {
					dumpNCLR(lego.NCLR);
				} else {
					printf("Empty NCLR block\n");
				}
			}

			if (which && strcmp(which, "NCLR") == 0) {
				goto cleanup;
			}
		} else if (strcmp(lego.blocks[i].signature, "NBRK") == 0) {
			if (which && strcmp(which, "NBRK") != 0) {
				continue;
			}

			parseNBRK(&lego.NBRK, &lego.blocks[i]);
			if (verbose) {
				if (lego.NBRK.count) {
					dumpNBRK(lego.NBRK);
				} else {
					printf("Empty NBRK block\n");
				}
			}

			if (which && strcmp(which, "NBRK") == 0) {
				goto cleanup;
			}
		}

		free(lego.blocks[i].signature);
		free(lego.blocks[i].data);
		free(lego.blocks[i].hash);
	}

cleanup:
	if (file) {
		fclose(file);
	}

	if (filedata) {
		free(filedata);
	}

	if (rc) {
		exit(rc);
	}

	*leg = lego;
}