/**
 *  \file parallel-mergesort.cc
 *
 *  \brief Implement your parallel mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "sort.hh"

void
parallelSort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */
  if (n < 2) {return;}

  parallelSort(N/2, A);

  parallelSort( N/2 , A + (N/2));

  //some parallel merge
}

/* eof */
