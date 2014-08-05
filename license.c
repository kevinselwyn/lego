#include <stdio.h>
#include <stdlib.h>
#include "lego.h"

#if !defined(__APPLE__)
#include <malloc.h>
#endif

#define LICENSE "LICENSE.txt"

struct gpl_index gpl_toc[18] = {
	{ 3626, 1928 },
	{ 5556, 2130 },
	{ 7688, 1349 },
	{ 9039, 786 },
	{ 9827, 619 },
	{ 10448, 1874 },
	{ 12324, 5465 },
	{ 17791, 3242 },
	{ 21035, 1365 },
	{ 22402, 595 },
	{ 22999, 1393 },
	{ 24394, 3870 },
	{ 28266, 687 },
	{ 28955, 558 },
	{ 29515, 1259 },
	{ 30776, 581 },
	{ 31359, 636 },
	{ 31997, 424 }
};

void gpl(int index) {
	int rc = 0, start = 0, length = 0;
	char *text = NULL;
	FILE *license = fopen(LICENSE, "r");
	struct gpl_index toc;

	if (!license) {
		rc = 1;
		goto cleanup;
	}

	if (index == -1) {
		start = 3626;
		length = 28845;
	} else if (index < 0 || 17 < index) {
		printf("Invalid GPL index %d\n", index);

		goto cleanup;
	} else {
		toc = gpl_toc[index];
		start = toc.start;
		length = toc.length;
	}

	fseek(license, start, SEEK_SET);
	text = malloc(sizeof(char) * (length + 1));

	if (fread(text, 1, length, license) != length) {
		rc = 1;
		goto cleanup;
	}

	printf("\n%s\n\n", text);

cleanup:
	if (rc) {
		printf("Could not find %s\n\n", LICENSE);
		printf("Please view the license online at:\n<http://www.gnu.org/licenses/>\n");
	}

	if (text) {
		free(text);
	}

	if (license) {
		fclose(license);
	}
}

void warranty() {
	gpl(15);
}

void conditions() {
	gpl(2);
}