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

#ifndef LEGO_H
#define LEGO_H

#include <stdint.h>

#define MAX_BLOCKS 10
#define MAX_BRICKS 100000
#define MAX_COLORS_DEF 256
#define MAX_BRICKS_DEF 256
#define MAX_WIDTH 1000
#define MAX_HEIGHT 1000

#define TOPLEFT 0
#define TOPRIGHT 1
#define BOTTOMRIGHT 2
#define BOTTOMLEFT 3

#define ROTATION_0 0
#define ROTATION_90 1
#define ROTATION_180 2
#define ROTATION_270 3

/* uiversal */
struct block{
	int length;
	char *signature, *data, *hash;
};

struct brick{
	int id, color, rotation;
};

struct BRKS{
	int count;
	struct brick bricks[MAX_BRICKS];
};

struct author{
	int name_length, email_length, url_length;
	char *name, *email, *url;
};

struct INFO{
	int length, name_length, version_length, description_length;
	char *name, *version, *description;
	struct author author;
};

struct color_def{
	int id, red, green, blue;
	char *name;
};

struct NCLR{
	int count;
	struct color_def colors[MAX_COLORS_DEF];
};

struct brick_def{
	int id, width, height;
	char *name;
};

struct NBRK{
	int count;
	struct brick_def bricks[MAX_BRICKS_DEF];
};

struct lego{
	int width, height;
	double version;
	char *header, *signature, *reserved;
	struct block blocks[MAX_BLOCKS];
	struct INFO INFO;
	struct BRKS BRKS;
	struct NCLR NCLR;
	struct NBRK NBRK;
};

int canvas[MAX_WIDTH][MAX_HEIGHT];

/* usage.c */
char *exec;
void usage(char *highlight);
void version();
void warranty();
void conditions();
void help();

/* license.c */
void gpl(int index);

struct gpl_index {
	int start, length;
};

/* hash.c */
uint32_t hash(char *data, uint32_t length, uint32_t seed);

/* parse.c */
void validate_header(struct lego lego, int verbose);
struct brick_def get_brick_by_id(int id);
struct color_def get_color_by_id(int id);
void parseINFO(struct INFO *INF, struct block *bloc);
void dumpINFO(struct INFO INFO);
void parseBRKS(struct BRKS *BRK, struct block *bloc);
void dumpBRKS(struct BRKS BRKS);
void parseNCLR(struct NCLR *NCL, struct block *bloc);
void dumpNCLR(struct NCLR NCLR);
void parseNBRK(struct NBRK *NBR, struct block *bloc);
void dumpNBRK(struct NBRK NBRK);
void parse(struct lego *leg, char *filename, int verbose, char *which);
void dumpColors();
void dumpBricks();
void listBricks(struct BRKS BRKS);
int get_closest_color(int r, int g, int b);
int get_brick_by_size(int width, int height);

/* encode.c */
int encode(char *input, char *output);

/* decode.c */
int decode(char *input, char *output, struct lego lego);

/* image.c */
struct image{
	unsigned char *data;
	int width, height;
};

struct image read_png(char *filename);
int write_png(struct image image, char *filename);
struct image read_jpeg(char *filename);
int write_jpeg(struct image image, char *filename);

#endif /* LEGO_H */