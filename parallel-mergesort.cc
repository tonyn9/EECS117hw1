/**
 *  \file parallel-mergesort.cc
 *
 *  \brief Implement your parallel mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "omp.h"

#include "sort.hh"

void parallelMergeSort(int N, keytype* A, keytype* tmp);
void merge (int N, keytype* A, keytype* tmp);

void
parallelSort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */

  // make a temperorary array to pass to new function
  keytype* temp_in = new newKeys(N);
  parallelMergeSort(N, A, temp_in);

  // copy values of temp into A
  //memcpy (A, temp_in, N * sizeof (keytype));

  // free temp_in 
  free (temp_in);
}


void parallelMergeSort(int N, keytype* A, keytype* tmp){

  if (n < 2) {return;}

  #pragma omp task firstprivate (N, A, tmp)
  parallelSortMergeSort(N/2, A, tmp);

  #pragma omp task firstprivate (N, A, tmp)
  parallelSortMergeSort( N/2 + 1, A + (N/2), tmp);

  #pragma omp taskwait

  merge(N, A, tmp);
}


void merge (int N, keytype* A, keytype* tmp){
  
}

/* eof */
