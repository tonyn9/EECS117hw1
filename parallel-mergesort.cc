/**
 *  \file parallel-mergesort.cc
 *
 *  \brief Implement your parallel mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#include <string.h>
#include "sort.hh"


void parallelMergeSort( keytype* A, keytype* tmp, int start, int end, int base);
void mergeParallel (keytype* A, int p1, int r1, int p2, int r2, keytype* temp, int p3, int base );
void serialMergeSort(keytype* A, int start, int end, keytype* tmp);
void mergeSerial( keytype* A, int start, int middle, int end, keytype* tmp);
int binary_search (keytype* A, int left, int right, keytype key);
void exchange (int& a, int& b);


// 'Main' sorting function, calls parallel mergesort on array A of size N
// returns A, sorted
void
parallelSort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */

  // make a temperorary array to pass to new function
  keytype* temp_in = newKeys(N);

	//start parallel blocks
	#pragma omp parallel
	{
		int numThreads = omp_get_num_threads();

	/*	#pragma omp master
		{
			//because i was running single threaded for the longest time
			printf("number of threads used: %d\n", numThreads);
		}*/

		#pragma omp single
		{
			parallelMergeSort( A, temp_in, 0, N-1, N/numThreads);
		}
		
	}

  // free temp_in
  free (temp_in);
}

//Recursive MergeSort Algorithm designed to add threads with each 
//recursive call
//Based on CLSR algorithm 3rd edition
void parallelMergeSort( keytype* A, keytype* tmp, int start, int end, int base){

	// at a certain point, extra threads will slow down the system.
	// use sequential algorithm at that point 
	if ((end - start + 1) <= base) {
		serialMergeSort(A, start, end, tmp); // faster than qsort actually
		return;}
	int middle = start + (end - start)/2;

	//start parallel merge sort, thread branch with first call only
	#pragma omp task
	{
		parallelMergeSort(A, tmp, start, middle, base);
	}
	parallelMergeSort(A, tmp, middle + 1, end, base); 
	#pragma omp taskwait

	//printf("Ns are: %d %d \n", N/2, N-(N/2));
	//mergeSerial(A, start, middle, end, tmp);
	
	// start parallel merge
	mergeParallel(A, start,middle, middle + 1, end, tmp, start, base);
	memcpy (A + start, tmp + start, (end-start+1) * sizeof(keytype));
}

// Merge two ranges of source array T[ p1 .. r1 ] and T[ p2 .. r2 ] 
// into destination array A starting at index p3.
// Based on CLSR algorithm 3rd edition
void mergeParallel (keytype* A, int p1, int r1, int p2, int r2, keytype* temp, int p3, int base ){
	
	int length1 = r1 - p1 + 1;
	int length2 = r2 - p2 + 1;

	// prevent the program from creating too many threads 
	// that will make the sorting algorithm slower
	if(length1 + length2 <= base){
		int i = p1;
		int j = p2;
		int k = p3;

		// compares the value in array 
		// and puts it into temp array
		while ( i <= r1 && j <= r2){
			if(A[i] <= A[j]){
				temp[k] = A[i];
				i++;
			} else{
				temp[k] = A[j];
				j++;
			}
			k++;
		}

		// finishes up if one part of array
		// is finished
		while (i <= r1){
			temp[k] = A[i];
			i++;
			k++;
		}
		while(j <= r2){
			temp[k] = A[j];
			j++;
			k++;
		}
		return;
	}
	
	// assumes Na >= Nb from lecture
	if ( length1 < length2 ){
		exchange(p1, p2);
		exchange(r1, r2);
		exchange(length1, length2);
	}

	if(length1 == 0){
		return;
	}

	// splits second array so that one array will have
	// values lower than key and the other will have values
	// higher than key.
	// put key in temp array
	int q1 = (p1 + r1)/2;
	int q2 = binary_search(A, p2, r2, A[q1] );
	int q3 = p3 + (q1 - p1) + (q2 - p2);

	temp[q3] = A[q1];

	// similar to the sort, one call will create a thread
	// and the next call continues this thread
	#pragma omp task
	{
		mergeParallel(A, p1, q1 - 1, p2, q2 - 1, temp, p3, base);
	}
		mergeParallel(A, q1 + 1, r1, q2, r2, temp, q3 + 1, base);
	
	#pragma omp taskwait


}

// echange algorithm for switching the larger array
void exchange (int& a, int& b){
	int c = a;
	a = b;
	b = c;
}

//merge one array when dealing with base case
//faster than qsort
void serialMergeSort(keytype* A, int start, int end, keytype* tmp){
	
	if(start < end){
		int mid = (start+end)/2;
		serialMergeSort(A, start, mid, tmp);
		serialMergeSort(A, mid + 1, end, tmp);

		mergeSerial(A, start, mid, end, tmp);
	}
}

//does a serial merge, usefull for single threads 
//or when at a certain size
void mergeSerial( keytype* A, int start, int middle, int end, keytype* tmp){
	int i = start;
	int j = middle + 1;
	int ti = start;

	while ( i <= middle && j <= end){
		if (A[i] <= A[j]){
			tmp[ti] = A[i];
			ti++;
			i++;
		}else{
			tmp[ti] = A[j];
			ti++;
			j++;
		}
	}
	while(i<=middle){
		tmp[ti] = A[i];
		ti++;
		i++;
	}
	while(j<=end){
		tmp[ti] = A[j];
		ti++; 
		j++;
	}
	memcpy(A + start, tmp + start, (end - start + 1) * sizeof (keytype));
}

//finds a good middle of an array
int binary_search (keytype* A, int left, int right, keytype key){
	int low = left;
	int high;
	if (left > (right +1)){
		high = left;
	}
	else{
		high = right + 1;
	}
	while (low < high){
		int mid = (low + high)/2;
		if (key <= A[mid]){
			high = mid;
		}else{
			low = mid + 1;
		}
	}
	return high;
}


/* eof */
