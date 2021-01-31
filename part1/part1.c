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
// #include <sys/sysinfo.h>
#include "time_util.h"
#include <math.h>


#define PARTB_TEST_TIMES (1024)

#define KB (1024)
#define MB (1024 * 1024)
#define L2BATCH (1000)
#define READ_TIMES 999999999
#define TEST_RANGE 24



static int sizes[] = { 1 * KB, 2 * KB, 4 * KB, 8 * KB, 12 * KB, 16 * KB, 24 * KB, 32 * KB, 64 * KB, 128 * KB, 256 * KB, 512 * KB, 1 * MB, 2 * MB, 4 * MB, 6 * MB, 8 * MB, 12 * MB, 16 * MB, 24 * MB, 32 * MB, 48 * MB, 64 * MB};

int dump_output_to_file(double *output, char *filename) {
	FILE *f = fopen(filename, "w");
	if (f == NULL) {
		printf("could not open output file\n");
		return -1;
	}
	for (int i = 0; i < sizeof(sizes) / sizeof(int); i++){
		fprintf(f, "%d KB %fns \n", sizes[i]/KB, output[i]/1000);
	}
	fclose(f);

	return 0;
}

long part_b() {
	struct timespec start;
	struct timespec end;

	// data[i] refers to the time spent on ramdomly access memory of size sizes[i]
	double *data = malloc(sizeof(sizes) / sizeof(int) * sizeof(double));
	long p = 0;

	for (int t = 0; t < 1000; t++){ //run experiment for 1000 times
		// Perform test on each size
		for (int i = 0; i < sizeof(sizes)/sizeof(int); i ++) { // i<sizeof(tests)/sizeof(int) - 1 ??????
			int size = sizes[i];
			long num_of_int = size / sizeof(int);  // tests[i]/sizeof(int) - 1;??????????
			int* buffer = malloc(size);
			// fill the buffer with some dummy numbers
			for (long k = 0; k < num_of_int; k ++) {
				buffer[k] = 3231;
			}

			// A list of random indices
			int ramdom_index[PARTB_TEST_TIMES];
			for (int j = 0; j < PARTB_TEST_TIMES; j ++) {
				ramdom_index[j] = (rand() % 1024) * (size / 1024);
			}

			// Perform 'write' to random locations PARTB_TEST_TIMES times and time this operation
			clock_gettime(CLOCK_MONOTONIC, &start);
			for (int t = 0; t < PARTB_TEST_TIMES; t++) {
				buffer[ramdom_index[t]] = ramdom_index[t] +1 ;
			}

			clock_gettime(CLOCK_MONOTONIC, &end);
			// Time spent on ramdomly access memory of size
			data[i] += timespec_to_nsec(difftimespec(end, start))/PARTB_TEST_TIMES;

			for (long k = 0; k < num_of_int; k ++) {
				p += buffer[k];
			}

//        printf("\n");
//            printf("%dth time, start_sec: %ld, end: %ld \n", i, start.tv_sec, end.tv_sec);
//            printf("%dth time, start_nsec: %ld, end: %ld \n", i, start.tv_nsec, end.tv_nsec);

			// Clean up
			free(buffer);
		}
	}
	dump_output_to_file(data, "data3.txt");

	return p;
}

int part_b_method2(double **data2, int times) {
	struct timespec start;
	struct timespec end;
	clockid_t clock = CLOCK_MONOTONIC;
	long p = 0;

	*data2 = malloc(sizeof(sizes) / sizeof(int)* sizeof(double));

	int *mem1 ;
	long back = 64 * MB / sizeof(int);
	for(int k = 0;k< sizeof(sizes) / sizeof(int) ; k++){
		(*data2)[k] = 0;
	}
	mem1 = malloc (back*sizeof(int));
	for(long k = 0;k<back;k++){
		mem1[k] = 3231;
	}


	for( int t = 0;t < times ;t++){

		for ( int i = 0;i<sizeof(sizes)/sizeof(int) ;i++){

			long n = sizes[i]/sizeof(int) - 1;
			clock_gettime(clock, &start);
			for (int j = 0;j<50000;j++){
				mem1[((j*256)&n)] = j+1;

			}
			clock_gettime(clock, &end);
			(*data2)[i] += timespec_to_nsec(difftimespec(end, start))/(50000);
			p += mem1[i];
		}
	}

//	for(long j = 0;j<back;j++){
//		p += mem1[j];
//	}
	dump_output_to_file(*data2, "data2.txt");

	return p;


}

long partb_method3(double **data, int times){
	struct timespec start;
	struct timespec end;
	clockid_t clock = CLOCK_MONOTONIC;

	*data = malloc(sizeof(sizes) / sizeof(int)* sizeof(double));

	int *mem1 ;
	long back = 64 * MB / sizeof(int);
	for(int k = 0;k< sizeof(sizes) / sizeof(int) ; k++){
		(*data)[k] = 0;
	}
	mem1 = malloc (back*sizeof(int));
	for(long k = 0;k<back;k++){
		mem1[k] = 3231;
	}


	for( int t = 0;t < times ;t++){

		for ( int i = 0;i<sizeof(sizes)/sizeof(int) - 1;i++){

			long n = sizes[i]/sizeof(int) - 1;
			clock_gettime(clock, &start);
			for (int j = 0;j<50000;j++){
				mem1[((j*256)&n)] = j+1;

			}
			clock_gettime(clock, &end);
			(*data)[i] += timespec_to_nsec(difftimespec(end, start))/(50000);
		}
	}

	dump_output_to_file(*data, "data.txt");

	long p = 0;
	for(long j = 0;j<back;j++){
		p += mem1[j];
	}
	return p;
}



int main(int argc, char *argv[])
{
	// Pin the thread to a single CPU to minimize effects of scheduling
	// Don't use CPU #0 if possible, it tends to be busier with servicing interrupts

	double *data;
//	double *data2;
	partb_method3(&data, L2BATCH);
	//part_b_method2(&data2, L2BATCH);
	part_b();
	free(data);
	//free(data2);
}
