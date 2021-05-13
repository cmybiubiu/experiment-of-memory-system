// ------------
// This code is provided solely for the personal and private use of
// students taking the CSC367 course at the University of Toronto.
// Copying for purposes other than this use is expressly prohibited.
// All forms of distribution of this code, whether as given or with
// any changes, are expressly prohibited.
//
// Authors: Bogdan Simion, Maryam Dehnavi, Alexey Khrabrov
//
// All of the files in this directory and all subdirectories are:
// Copyright (c) 2019 Bogdan Simion and Maryam Dehnavi
// -------------

#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <math.h>

#include "time_util.h"

#define PARTA_TEST_TIMES (50)
#define PARTB_TEST_TIMES (500)

#define KB (1024)
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)

static int sizes_a[] = { 1 * MB, 2 * MB, 4 * MB, 8 * MB, 16 * MB, 32 * MB, 64 * MB, 128 * MB, 256 * MB, 512 * MB, 1 * GB };
static int sizes_b[] = { 4 * KB, 8 * KB, 16 * KB, 32 * KB, 64 * KB, 128 * KB, 256 * KB, 512 * KB, 1 * MB, 2 * MB, 4 * MB, 8 * MB, 16 * MB, 32 * MB, 64 * MB, 128 * MB };
static int sizes_level[] = { 32 * KB, 256 * KB, 16 * MB, 128 * MB };


/* Output data for part A */
int dump_output_to_file_a(double *output, char *filename) {
	FILE *f = fopen(filename, "w");
	if (f == NULL) {
		printf("could not open output file\n");
		return -1;
	}
	for (int i = 0; i < sizeof(sizes_a)/sizeof(int); i++){
		// Output format: MB, s, MB/s
		int size_MB = sizes_a[i]/(1024 * 1024);
		fprintf(f, "%d %f %f\n", size_MB, output[i] / (PARTA_TEST_TIMES * 1000000000.), size_MB * PARTA_TEST_TIMES * 1000000000. / output[i]);
	}
	fclose(f);
	return 0;
}


/* An experiment to measure memory bandwidth */
char part_a() {
	struct timespec start;
	struct timespec end;

	// data[i] refers to the time spent on writing to a block memory of size sizes_a[i]
	double *data = malloc(sizeof(sizes_a) / sizeof(int) * sizeof(double));
	// initialize data
	for (int i = 0; i < sizeof(sizes_a) / sizeof(int); i ++) {
		data[i] = 0;
	}

	// A block of memory
	char* buffer = malloc(1 * GB);
	memset(buffer, 'Z', 1 * GB);

	// Run tests PARTA_TEST_TIMES times
	for(int t = 0; t < PARTA_TEST_TIMES; t ++) {
		// Run test for each size in sizes a
		for (int i = 0; i < sizeof(sizes_a) / sizeof(int); i ++) {
			char c = 'A' + i;
			clock_gettime(CLOCK_MONOTONIC, &start);
			// write to a block memory
			memset(buffer, c, sizes_a[i]);
			clock_gettime(CLOCK_MONOTONIC, &end);
			data[i] += timespec_to_nsec(difftimespec(end, start));

			// Reset the buffer
			memset(buffer, 'Z', 1 * GB);
			// To go around compiler optimization
			char random_letter = 'A' + (random() % 26);
			buffer[i] = random_letter;
		}
	}
	// To go around compiler optimization
	char p = 'p';
	for (int i = 0; i < 40 * MB; i ++) {
		p = buffer[i];
	}

	dump_output_to_file_a(data, "data_a.txt");
	
	// Clean up
	free(buffer);
	free(data);
	return p;
}


/* Output data for part B: levels */
int dump_output_to_file_b(double *output, char *filename) {
	FILE *f = fopen(filename, "w");
	if (f == NULL) {
		printf("could not open output file\n");
		return -1;
	}
	for (int i = 0; i < sizeof(sizes_b)/sizeof(int); i++){
		// Output units: KB ns
		fprintf(f, "%d %f\n", sizes_b[i]/1024, output[i] / PARTB_TEST_TIMES);
	}
	fclose(f);
	return 0;
}


/* An experiment to determine the number of levels in the CPU cache hierarchy, and to measure cache
 * sizes for each level, as well as the main memory.
 */
long part_b_levels() {
	struct timespec start;
	struct timespec end;

	// data[i] refers to the time spent on ramdomly accessed memory of size sizes_b[i]
	double *data = malloc(sizeof(sizes_b) / sizeof(int) * sizeof(double));
	// initialize data
	for (int i = 0; i < sizeof(sizes_b) / sizeof(int); i ++) {
		data[i] = 0;
	}

	// Allocate a buffer of 128 MB
	int* buffer = malloc(128 * MB);
	// fill the buffer with some dummy numbers
	for (long k = 0; k < (128 * MB) / sizeof(int); k ++) {
		buffer[k] = 123;
	}

	// Perform test on each size in the sizes_b array
	for (long s = 0; s < sizeof(sizes_b) / sizeof(int); s ++) {
		// Each test runs PARTB_TEST_TIMES times
		for (int t = 0; t < PARTB_TEST_TIMES; t ++) {
			// Binary representation of num_of_int: 10000..., binary representation of num_of_int: 11111...
			long num_of_int = sizes_b[s] / sizeof(int) - 1;
			//Perform 'write' to discontinues locations and time this operation
			if (sizes_b[s] < 1 * MB) {
				clock_gettime(CLOCK_MONOTONIC, &start);
				for (int j = 0; j < 50000; j ++){
					// Stride is 128
					buffer[(j * 128) & num_of_int] = j;
				}
				clock_gettime(CLOCK_MONOTONIC, &end);
				data[s] += timespec_to_nsec(difftimespec(end, start)) / 50000;
			} else {
				clock_gettime(CLOCK_MONOTONIC, &start);
				for (int j = 0; j < 100000; j ++){
					// Stride is 512
					buffer[(j * 512) & num_of_int] = j;
				}
				clock_gettime(CLOCK_MONOTONIC, &end);
				data[s] += timespec_to_nsec(difftimespec(end, start)) / 100000;
			}
		}
	}
	dump_output_to_file_b(data, "data_b.txt");
	// Go around compiler optimization
	long p = 0;
	for (long k = 0; k < (128 * MB) / sizeof(int); k ++) {
		p = buffer[k];
	}
	// Clean up
	free(buffer);
	free(data);
	return p;
}

/* Output data for part B: latency */
int dump_output_to_file_b_latency(double *output, char *filename) {
	FILE *f = fopen(filename, "w");
	if (f == NULL) {
		printf("could not open output file\n");
		return -1;
	}
	for (int i = 0; i < sizeof(sizes_level)/sizeof(int); i++){
		// Output units: KB ns
		fprintf(f, "%d %f\n", sizes_level[i]/1024, output[i] / PARTB_TEST_TIMES);
	}
	fclose(f);
	return 0;
}


/* An experiment to determine the cache (write) latencies for each level, as well as the
 * write latency of main memory.
 */
char part_b_latency() {
	struct timespec start;
	struct timespec end;

	// data[i] refers to the time spent on ramdomly accessed memory of size sizes_level[i]
	double *data = malloc(sizeof(sizes_level) / sizeof(int) * sizeof(double));
	// initialize data
	for (int i = 0; i < sizeof(sizes_level) / sizeof(int); i ++) {
		data[i] = 0;
	}

	// Allocate a buffer of 128 MB
	char* buffer = malloc(128 * MB);
	// Initialize the buffer
	memset(buffer, 'Z', 128 * MB);

	// Perform test on each size in the sizes_level array
	for (long s = 0; s < sizeof(sizes_level) / sizeof(int); s ++) {
		// Binary representation of size: 10000...
		int size = sizes_level[s];
		// Binary representation of mod: 11111...
		int mod = sizes_level[s] - 1;
		// Each test runs PARTB_TEST_TIMES times
		for (int t = 0; t < PARTB_TEST_TIMES; t ++) {
			//Perform 'write' to discontinues locations and time this operation
			if (size < 1 * MB) {
				clock_gettime(CLOCK_MONOTONIC, &start);
				for (int j = 0; j < 10000; j ++){
					// Stride is 128
					buffer[(j * 128) & mod] = 'A';
				}
				clock_gettime(CLOCK_MONOTONIC, &end);
				data[s] += timespec_to_nsec(difftimespec(end, start)) / 10000;
			} else {
				clock_gettime(CLOCK_MONOTONIC, &start);
				for (int j = 0; j < 200000; j ++){
					// Stride is 512
					buffer[(j * 512) & mod] = 'B';
				}
				clock_gettime(CLOCK_MONOTONIC, &end);
				data[s] += timespec_to_nsec(difftimespec(end, start)) / 200000;
			}

			// Reset the buffer
			memset(buffer, 'Z', 128 * MB);
			// Go around compiler optimization
			char random_letter = 'A' + (random() % 26);
			buffer[t] = random_letter;
		}
	}
	dump_output_to_file_b_latency(data, "data_b_latency.txt");
	// Go around compiler optimization
	char p = 'p';
	for (int i = 0; i < 40 * MB; i ++) {
		p = buffer[i];
	}
	// Clean up
	free(buffer);
	free(data);
	return p;
}



int main(int argc, char *argv[])
{
	part_a();
	part_b_levels();
	part_b_latency();

	return 0;
}
