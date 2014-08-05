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

struct action {
	char *name;
	void (*func)();
	int rc;
};

struct action actions[8] = {
	{ "--version", version },
	{ "-v", version },
	{ "--help", help },
	{ "-h", help },
	{ "--warranty", warranty },
	{ "-w", warranty },
	{ "--conditions", conditions },
	{ "-c", conditions }
};

int main(int argc, char *argv[]) {
	int rc = 0, i = 0, l = 0;
	char *action = NULL, *input = NULL, *output = NULL;
	struct lego lego;

	exec = argv[0];

	if (argc < 2) {
		usage(NULL);

		rc = 1;
		goto cleanup;
	}

	action = argv[1];
	input = argv[2];
	output = argv[3];

	for (i = 0, l = 8; i < l; i++) {
		if (strcmp(action, actions[i].name) == 0) {
			actions[i].func();

			rc = 1;
			goto cleanup;
		}
	}

	if (strcmp(action, "--license") == 0 || strcmp(action, "-l") == 0) {
		if (argc < 3) {
			gpl(-1);
		} else {
			gpl(atoi(input));
		}

		rc = 1;
		goto cleanup;
	} else if (strcmp(action, "encode") == 0) {
		if (argc < 3) {
			usage("encode");

			rc = 1;
			goto cleanup;
		}

		encode(input, output);
	} else if (strcmp(action, "decode") == 0) {
		if (argc < 3) {
			usage("decode");

			rc = 1;
			goto cleanup;
		}

		parse(&lego, input, 0, NULL);
		decode(input, output, lego);
	} else if (strcmp(action, "info") == 0) {
		if (argc < 3) {
			usage("info");

			rc = 1;
			goto cleanup;
		}

		parse(&lego, input, 1, output);
	} else if (strcmp(action, "list") == 0) {
		if (argc < 3) {
			usage("list");

			rc = 1;
			goto cleanup;
		}

		parse(&lego, input, 0, NULL);
		listBricks(lego.BRKS);
	} else if (strcmp(action, "colors") == 0) {
		if (argc > 2) {
			parse(&lego, input, 0, NULL);
		}

		dumpColors();
	} else if (strcmp(action, "bricks") == 0) {
		if (argc > 2) {
			parse(&lego, input, 0, NULL);
		}

		dumpBricks();
	} else {
		printf("Invalid action: %s\n", action);

		rc = 1;
		goto cleanup;
	}

cleanup:
	if (lego.header) {
		free(lego.header);
	}

	if (lego.signature) {
		free(lego.signature);
	}

	if (lego.reserved) {
		free(lego.reserved);
	}

	return rc;
}
