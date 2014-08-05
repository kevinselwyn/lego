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
#include <png.h>
#include <jpeglib.h>
#include "lego.h"

#if !defined(__APPLE__)
#include <malloc.h>
#endif

struct image read_png(char *filename) {
	int rc = 0, i = 0, x = 0, y = 0, width = 0, height = 0;
	char *header = NULL;
	unsigned char *data = NULL;
	FILE *file = NULL;
	png_byte color_type, bit_depth;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers;
	struct image image;

	file = fopen(filename, "rb");

	if (!file) {
		printf("Could not open %s\n", filename);

		rc = 1;
		goto cleanup;
	}

	header = malloc(sizeof(char) * (8 + 1));

	if (fread(header, 1, 8, file) != 8) {
		printf("Could not read %s\n", filename);

		rc = 1;
		goto cleanup;
	}

	if (png_sig_cmp(header, 0, 8)) {
		printf("%s is not a PNG\n", filename);

		rc = 1;
		goto cleanup;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		printf("png_create_read_struct failed\n");

		rc = 1;
		goto cleanup;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		printf("png_create_info_struct failed\n");

		rc = 1;
		goto cleanup;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error initializing read\n");

		rc = 1;
		goto cleanup;
	}

	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	if (color_type != 2 || bit_depth != 8) {
		printf("PNGs must be 8-bit RGB\n");

		rc = 1;
		goto cleanup;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Could not read PNG\n");

		rc = 1;    
		goto cleanup;
	}

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);

	for (y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
	}

	png_read_image(png_ptr, row_pointers);

	data = malloc(sizeof(char) * (width * height * 3) + 1);

	for (y = 0, i = 0; y < height; y++) {
		png_byte *row = row_pointers[y];

		for (x = 0; x < width; x++) {
			png_byte *rgb = &(row[x * 3]);

			data[i++] = rgb[0];
			data[i++] = rgb[1];
			data[i++] = rgb[2];
		}
	}

cleanup:
	if (header) {
		free(header);
	}

	if (file) {
		fclose(file);
	}

	if (rc) {
		exit(1);
	}

	image.data = data;
	image.width = width;
	image.height = height;

	return image;
}

int write_png(struct image image, char *filename) {
	int rc = 0, i = 0, x = 0, y = 0, width = 0, height = 0;
	unsigned char *data = NULL;
	FILE *file = NULL;
	png_byte color_type, bit_depth;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers;

	file = fopen(filename, "wb");

	if (!file) {
		printf("Could not open %s\n", filename);

		rc = 1;
		goto cleanup;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		printf("png_create_write_struct failed\n");

		rc = 1;
		goto cleanup;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		printf("png_create_info_struct failed\n");

		rc = 1;
		goto cleanup;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error initializing write\n");

		rc = 1;
		goto cleanup;
	}

	png_init_io(png_ptr, file);

	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error writing header\n");

		rc = 1;
		goto cleanup;
	}

	data = image.data;
	width = image.width;
	height = image.height;
	color_type = 2;
	bit_depth = 8;

	png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);

	for (y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
	}

	for (y = 0, i = 0; y < height; y++) {
		png_byte *row = row_pointers[y];

		for (x = 0; x < width; x++) {
			png_byte *rgb = &(row[x * 3]);

			rgb[0] = data[i++];
			rgb[1] = data[i++];
			rgb[2] = data[i++];
		}
	}

	png_write_image(png_ptr, row_pointers);

	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error ending write\n");

		rc = 1;
		goto cleanup;
	}

	png_write_end(png_ptr, NULL);

cleanup:
	if (file) {
		fclose(file);
	}

	return rc;
}

struct image read_jpeg(char *filename) {
	int rc = 0, i = 0, x = 0, y = 0, width = 0, height = 0, row_stride = 0;
	unsigned char *data = NULL, r = NULL, g = NULL, b = NULL;
	FILE *file = NULL;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY pJpegBuffer;
	struct image image;

	file = fopen(filename, "rb");

	if (!file) {
		printf("Could not open %s\n", filename);

		rc = 1;
		goto cleanup;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, file);

	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	width = cinfo.output_width;
	height = cinfo.output_height;
	row_stride = width * cinfo.output_components;

	pJpegBuffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	i = 0;
	data = malloc(sizeof(char) * (width * height * 3) + 1);

	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, pJpegBuffer, 1);

		for (x = 0; x < width; x++) {
			r = pJpegBuffer[0][cinfo.output_components * x];

			if (cinfo.output_components > 2) {
				g = pJpegBuffer[0][cinfo.output_components * x + 1];
				b = pJpegBuffer[0][cinfo.output_components * x + 2];
			} else {
				g = r;
				b = r;
			}

			data[i++] = r;
			data[i++] = g;
			data[i++] = b;
		}

		y++;
	}

cleanup:
	if (file) {
		fclose(file);
	}

	if (rc) {
		exit(1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	image.data = data;
	image.width = width;
	image.height = height;

	return image;
}

int write_jpeg(struct image image, char *filename) {
	int rc = 0, width = 0, height = 0, row_stride = 0;
	unsigned char *data = NULL;
	FILE *file = NULL;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];

	file = fopen(filename, "wb");

	if (!file) {
		printf("Could not open %s\n", filename);

		rc = 1;
		goto cleanup;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, file);

	data = image.data;
	width = image.width;
	height = image.height;

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 100, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = cinfo.input_components * cinfo.image_width;

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = (JSAMPROW)&data[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, (JSAMPARRAY)&row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);

cleanup:
	if (file) {
		fclose(file);
	}

	jpeg_destroy_compress(&cinfo);

	return rc;
}