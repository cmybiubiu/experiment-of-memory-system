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

//TODO: parallelize the code and optimize performance


// Compute the historic average grade for a given course. Updates the average value in the record
void compute_average(course_record *course) {
	assert(course != NULL);
	assert(course->grades != NULL);

	course_record *course_local = (course_record *)course;

	double average = 0.0;
	int course_count = course_local->grades_count;

	grade_record *course_grades = course_local->grades;
	//course_grades = malloc(sizeof(grade_record)*course_count);
	printf("size of course grades %ld", sizeof(grade_record));

	for (int i = 0; i < course->grades_count; i++) {
		average += (double) (course_grades[i].grade);
	}
	course->average = average/course_count;
	//free(course_grades);
	pthread_exit(NULL);
}

// Compute the historic average grades for all the courses
void compute_averages(course_record *courses, int courses_count) {
	assert(courses != NULL);

	pthread_t thread_id[courses_count];
	int ret, rc;


	for (int i = 0; i < courses_count; i++) {
		//multi thread for each course

		ret = pthread_create(&thread_id[i],NULL, (void *)compute_average, (void *)&(courses[i]) );//input: &id, Null,
		if (ret != 0){
			printf("create pthread error!\n");
			return;
		}
	}

	for (int i =0; i < courses_count; i++){

		rc = pthread_join(thread_id[i], NULL);
		if (rc != 0){
			printf("pthread join error!\n");
			return;
		}
	}
}


int main(int argc, char *argv[])
{
	course_record *courses;
	int courses_count;
	// Load data from file; "part2data" is the default file path if not specified
	if (load_data((argc > 1) ? argv[1] : "part2data", &courses, &courses_count) < 0) return 1;

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	compute_averages(courses, courses_count);
	clock_gettime(CLOCK_MONOTONIC, &end);

	for (int i = 0; i < courses_count; i++) {
		printf("%s: %f\n", courses[i].name, courses[i].average);
	}

	printf("%f\n", timespec_to_msec(difftimespec(end, start)));

	free_data(courses, courses_count);
	return 0;
}
