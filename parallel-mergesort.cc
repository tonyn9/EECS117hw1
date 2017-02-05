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
/*
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

void parallelMergeSort( keytype* A, keytype* tmp, int start, int end, int base);
void mergeParallel (keytype* A, int p1, int r1, int p2, int r2, keytype* temp, int p3, int base );
void serialMergeSort(keytype* A, int start, int end, keytype* tmp);
void mergeSerial( keytype* A, int start, int middle, int end, keytype* tmp);
int binary_search (keytype* A, int left, int right, keytype key);
void exchange (int& a, int& b);
static int compare (const void* a, const void* b);


// 'Main' sorting function, calls parallel mergesort on array A of size N
// returns A, sorted
void
parallelSort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */

  // make a temperorary array to pass to new function
  keytype* temp_in = newKeys(N);


#pragma omp parallel
{
	int numThreads = omp_get_num_threads();

	#pragma omp master
	{
		//because i was running single threaded for the longest time
		printf("number of threads used: %d\n", numThreads);
	}

	#pragma omp single
	{
		parallelMergeSort( A, temp_in, 0, N-1, N/numThreads);
	}
	
}

  

  // copy values of temp into A
  //memcpy (A, temp_in, N * sizeof (keytype));

  // free temp_in
  free (temp_in);
}

//Recursive MergeSort Algorithm designed to add threads with each 
//recursive call
void parallelMergeSort( keytype* A, keytype* tmp, int start, int end, int base){

  if ((end - start + 1) <= base) {
	  serialMergeSort(A, start, end, tmp);
	  return;}
int middle = start + (end - start)/2;

  #pragma omp task
  {
  	parallelMergeSort(A, tmp, start, middle, base);
  }
  parallelMergeSort(A, tmp, middle + 1, end, base);
  
  //parallelMergeSort(N/2, A + N/2 + 1, tmp, base);
  
  #pragma omp taskwait

  //printf("Ns are: %d %d \n", N/2, N-(N/2));
  //mergeSerial(A, start, middle, end, tmp);
  
  mergeParallel(A, start,middle, middle + 1, end, tmp, start, base);
  memcpy (A + start, tmp + start, (end-start+1) * sizeof(keytype));
}

// Merge two ranges of source array T[ p1 .. r1 ] and T[ p2 .. r2 ] 
// into destination array A starting at index p3.
void mergeParallel (keytype* A, int p1, int r1, int p2, int r2, keytype* temp, int p3, int base ){
	
	int length1 = r1 - p1 + 1;
	int length2 = r2 - p2 + 1;

	if(length1 + length2 <= base){
		int i = p1;
		int j = p2;
		int k = p3;

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
	
	
	if ( length1 < length2 ){
		exchange(p1, p2);
		exchange(r1, r2);
		exchange(length1, length2);
	}

	if(length1 == 0){
		return;
	}
	int q1 = (p1 + r1)/2;
	int q2 = binary_search(A, p2, r2, A[q1] );
	int q3 = p3 + (q1 - p1) + (q2 - p2);

	temp[q3] = A[q1];

	#pragma omp task
	{
		
		mergeParallel(A, p1, q1 - 1, p2, q2 - 1, temp, p3, base);
	}
		mergeParallel(A, q1 + 1, r1, q2, r2, temp, q3 + 1, base);
	
	#pragma omp taskwait


}

void exchange (int& a, int& b){
	int c = a;
	a = b;
	b = c;
}

//merge one array
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
