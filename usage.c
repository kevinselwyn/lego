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

#if !defined(__APPLE__)
#include <malloc.h>
#endif

#define VERSION "1.0.1"

char *exec = "lego";

void version() {
	printf("LEGO® Enhanced Graphics Object, v%s\n", VERSION);
}

void license() {
	printf("This program comes with ABSOLUTELY NO WARRANTY; for details run `%s -w`.\n", exec);
	printf("This is free software, and you are welcome to redistribute it\n");
	printf("under certain conditions; run `%s -c` for details.\n\n", exec);
}

void fairplay() {
	printf("Read more about Lego® Fair Play at:\n<http://aboutus.lego.com/en-us/legal-notice/fair-play>\n\n");
}

void help() {
	usage(NULL);
}

struct command{
	char *name, *description, *usage;
};

struct command commands[6] = {
	{ "encode", "Encodes a PNG into a LEGO file", "<infile.png> (<outfile.lego>)" },
	{ "decode", "Decodes a LEGO file into a PNG", "<infile.lego> (<outfile.png>)" },
	{ "info", "Prints the basic info about a LEGO file", "<infile.lego> (<INFO|BRKS|NCLR|NBRK>)" },
	{ "list", "Tallies the number and types of bricks used", "<infile.lego>" },
	{ "colors", "Lists all colors available", "(<infile.lego>)" },
	{ "bricks", "Lists all bricks available", "(<infile.lego>)" }
};

void usage(char *highlight) {
	int indent = (int)strlen(exec) + 7, i = 0, l = 0;
	char *color = "\x1B[31m", *normal = "\x1B[0m";

	version();
	printf("Copyright (C) 2014, Kevin Selwyn <kevinselwyn@gmail.com>\n\n");
	license();
	fairplay();

	if (!highlight) {
		printf("Usage: %s [--version,-v] [--help,-h]\n", exec);
		printf("%*s [--warranty,-w] [--conditions,-c]\n", indent, "");
		printf("%*s <command> [<args>]\n\n", indent, "");
		printf("The commands are:\n");
	} else {
		for (i = 0, l = 6; i < l; i++) {
			if (highlight && strcmp(highlight, commands[i].name) == 0) {
				printf("Usage: %s %s %s\n\n", exec, commands[i].name, commands[i].usage);
			}
		}
	}

	for (i = 0, l = 6; i < l; i++) {
		indent = 6 - (int)strlen(commands[i].name);

		if (highlight && strcmp(highlight, commands[i].name) == 0) {
			printf("%s   %s%*s  %s%s\n", color, commands[i].name, indent, "", commands[i].description, normal);
		} else {
			printf("   %s%*s  %s\n", commands[i].name, indent, "", commands[i].description);
		}
	}

	printf("\n");
}