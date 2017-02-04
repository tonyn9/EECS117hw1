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
//What I did so far.

/*
// Swap two integers a and b
void intExchange(int& a, int& b)
{
	int t = a;
	a = b;
	b = t;
}

// Perform binary search for key x in array A[left, right]
int binarySearch(keytype x, keytype* A, int left, int right)
{

	int low = left;
	int high;
	if (left > (right + 1)){
		high = left;
	}
	else{
		high = right + 1;
	}

	while (low < high){
		int mid = (low + high) / 2;
		if (x <= A[mid]){
			high = mid;
		}
		else{
			low = mid + 1;
		}
	}

	return high;
};


// Recursive parallel merging algorithm, borrowed from CLRS Intoduction to Algorithms, 3rd ed.
// merges TMP[p1...r1] and TMP[p2...r2] into OUT[p3...r3] where r3 = p3 + (r1 - p1) + (r2 - p2) + 1
void parallelMerge(keytype* TMP, int p1, int r1, int p2, int r2, keytype* OUT, int p3){

	int n1 = r1 - p1 + 1;
	int n2 = r2 - p2 + 1;

	if (n1 < n2){
		intExchange(p1, p2);
		intExchange(r1, r2);
		intExchange(n1, n2);
	}

	///////////////////////////////////
	// use a serial in-place merge when problem size becomes small enough
	//   to reduce overhead
	if (n1 < 100000){
		int i, j, k;
		i = p1;
		j = p2;
		k = p3;
		while ((i <= r1) && (j <= r2))
		{
			if (TMP[i] < TMP[j])
				OUT[k++] = TMP[i++];
			else
				OUT[k++] = TMP[j++];
		}

		while (i <= r1){
			OUT[k] = TMP[i];
			i++; k++;
		}

		while (j <= r2){
			OUT[k] = TMP[j];
			j++; k++;
		}

		return;
	}

	// no need to merge, both empty
	if (n1 == 0)
	{
		return;
	}

	else
	{

		int q1 = (p1 + r1) / 2;
		int q2 = binarySearch(TMP[q1], TMP, p2, r2);
		int q3 = p3 + (q1 - p1) + (q2 - p2);
		OUT[q3] = TMP[q1];

		#pragma omp task default(none) shared(TMP, OUT, p1, p2, p3, q1, q2, q3, r1, r2)
			parallelMerge(TMP, p1, q1 - 1, p2, q2 - 1, OUT, p3);
			parallelMerge(TMP, q1 + 1, r1, q2, r2, OUT, q3 + 1);
		#pragma omp taskwait
	}
}

// Recursive parallel mergesort algorithm, borrowed from CLRS Intoduction to Algorithms, 3rd ed.
void parallelMergeSort(keytype* A, int p, int r, keytype* OUT, int s, int numThreads)
	{

	int n = r - p + 1;
	//printf("threads is %d\n", numThreads);
	///////////////////////////////////
	// use sequential sort when problem size becomes small enough, or only on one processor
	if (n < 1000000 || (numThreads < 2) ){
		sequentialSort(n, OUT + s);
	}
	else{

		keytype* TMP = newCopy(r + 1, A);

		int q = (p + r) / 2;

		#pragma omp task default(none) shared(A, TMP, p, r, q, numThreads)
			parallelMergeSort(A, p, q, TMP, p, numThreads / 2);
			parallelMergeSort(A, q + 1, r, TMP, q + 1, numThreads - numThreads / 2);
		#pragma omp taskwait

		parallelMerge(TMP, p, q, q + 1, r, OUT, s);
		free(TMP);

	}
}

// 'Main' sorting function, calls parallel mergesort on array A of size N
// returns A, sorted
void parallelSort(int N, keytype* A)
{
	Lucky you, you get to start from scratch

	keytype* OUT = (keytype *)malloc(N * sizeof(keytype));
	memcpy(OUT, A, N * sizeof(keytype));

	int numThreads = omp_get_num_procs();

	#pragma omp parallel
	#pragma omp single nowait
		parallelMergeSort(A, 0, N - 1, OUT, 0, numThreads);

	memcpy(A, OUT, N * sizeof(keytype));

	free(OUT);

	return;

*/

void parallelMergeSort(int N, keytype* A, keytype* tmp);
void mergeParallel (int N, keytype* A, keytype* tmp);
void serialMergeSort(int N, keytype* A, keytype* tmp);
void mergeSerial(int N, keytype* A, keytype* tmp);

static int compare (const void* a, const void* b);

// 'Main' sorting function, calls parallel mergesort on array A of size N
// returns A, sorted
void
parallelSort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */

  // make a temperorary array to pass to new function
  keytype* temp_in = newKeys(N);
  parallelMergeSort(N, A, temp_in);

  // copy values of temp into A
  //memcpy (A, temp_in, N * sizeof (keytype));

  // free temp_in
  free (temp_in);
}

//Recursive MergeSort Algorithm designed to add threads with each 
//recursive call
void parallelMergeSort(int N, keytype* A, keytype* tmp){

  if (N < 10000) {
	  qsort (A, N, sizeof (keytype), compare)
	  return;}

  #pragma omp task firstprivate (N, A, tmp)
  parallelMergeSort(N/2, A, tmp);

  #pragma omp task firstprivate (N, A, tmp)
  parallelMergeSort( N/2 + 1, A + (N/2), tmp);

  #pragma omp taskwait

  mergeSerial(N, A, tmp);
}

//tbd
void mergeParallel (int N, keytype* A, keytype* tmp){

}

void serialMergeSort(int N, keytype* A, keytype* tmp){
	if(N <  2){
		return;
	}

}

//does a serial merge, usefull for single threads 
//or when at a certain size
void mergeSerial(int N, keytype* A, keytype* tmp){
	int i = 0;
	int j = N/2;
	int ti = 0;

	while ( i < N/2 && j < N){
		if (A[i] < A[j]){
			tmp[ti] = A[i];
			ti++;
			i++;
		}else{
			tmp[ti] = A[j];
			ti++;
			j++;
		}
	}
	while(i<N/2){
		tmp[ti] = A[i];
		ti++;
		i++;
	}
	while(j<N){
		tmp[ti] = A[j];
		ti++; 
		j++;
	}
	memcpy(A, tmp, N * sizeof (keytype));
}

static int compare (const void* a, const void* b)
{
  keytype ka = *(const keytype *)a;
  keytype kb = *(const keytype *)b;
  if (ka < kb)
    return -1;
  else if (ka == kb)
    return 0;
  else
    return 1;
}

/* eof */
