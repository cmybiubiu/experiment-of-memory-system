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

#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "data.h"
#include "time_util.h"

__int32_t threads;
course_record *all_courses;
__int32_t chunk_size;
__int32_t courses_count;

//TODO: parallelize the code and optimize performance
// Compute the historic average grade for a given course. Updates the average value in the record
void compute_average(int nth_course)
{
	assert(&all_courses[nth_course] != NULL);
	assert(all_courses[nth_course].grades != NULL);
	int grade_count = all_courses[nth_course].grades_count;
	double average = 0.0;

	for (int i = 0; i < grade_count; i++) {
		average += all_courses[nth_course].grades[i].grade;
	}
	all_courses[nth_course].average = average/grade_count;

}

//Parallel sharded implementation of calculation of average
void compute_average_sharded(void * ith_thread){
	int chunk_offset = (*(int*)ith_thread) *chunk_size;

	for (int i = chunk_offset; i<chunk_offset+chunk_size && i<courses_count; i++ ){
		compute_average(i);
	}
}

// Compute the historic average grades for all the courses
void compute_averages(course_record *courses, int courses_count) {
	assert(courses != NULL);

	int threads = (int) courses_count/2 ;

	pthread_t thread_id[threads];
	int idx[threads];
	int ret, rc;

	for (int i = 0; i < threads; i++) {
		//assign 2 courses's calculation to 1 thread
		idx[i] = i;
		ret = pthread_create(&thread_id[i],NULL, (void*)compute_average_sharded, (void*)&idx[i] );
		if (ret){
			printf("create pthread error!\n");
			return;
		}
	}

	for (int i =0; i < threads; i++){

		rc = pthread_join(thread_id[i], NULL);
		if (rc){
			printf("pthread join error!\n");
			return;
		}
	}
}


int main(int argc, char *argv[])
{
	chunk_size = 2;
	// Load data from file; "part2data" is the default file path if not specified
	if (load_data((argc > 1) ? argv[1] : "part2data", &all_courses, &courses_count) < 0) return 1;

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	compute_averages(all_courses, courses_count);
	clock_gettime(CLOCK_MONOTONIC, &end);

	for (int i = 0; i < courses_count; i++) {
		printf("%s: %f\n", all_courses[i].name, all_courses[i].average);
	}

	printf("%f\n", timespec_to_msec(difftimespec(end, start)));

	free_data(all_courses, courses_count);
	return 0;
}
