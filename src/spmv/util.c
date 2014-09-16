#ifndef SPMV_UTIL_H_
#define SPMV_UTIL_H_

#include <stdlib.h>
#include <assert.h>

#define PAD_FACTOR 16

void initRandomMatrix(int *cols, int *rowDelimiters, const int n, const int dim);

// Original version by: Kyle Spafford
void initRandomMatrix(int *cols, int *rowDelimiters, const int n, const int dim)
{
    int nnzAssigned = 0;

    // Figure out the probability that a nonzero should be assigned to a given
    // spot in the matrix
    double prob = (double)n / ((double)dim * (double)dim);

    // Seed random number generator
    srand48(8675309L);

    // Randomly decide whether entry i,j gets a value, but ensure n values
    // are assigned
    int fillRemaining = 0;
    int i,j;
    for (i = 0; i < dim; i++)
    {
        rowDelimiters[i] = nnzAssigned;
        for (j = 0; j < dim; j++)
        {
            int numEntriesLeft = (dim * dim) - ((i * dim) + j);
            int needToAssign   = n - nnzAssigned;
            if (numEntriesLeft <= needToAssign) {
                fillRemaining = 1;
            }
            if ((nnzAssigned < n && drand48() <= prob) || fillRemaining)
            {
                // Assign (i,j) a value
                cols[nnzAssigned] = j;
                nnzAssigned++;
            }
        }
    }
    // Observe the convention to put the number of non zeroes at the end of the
    // row delimiters array
    rowDelimiters[dim] = n;
    assert(nnzAssigned == n);
}

// Original version by: Lukasz Wesolowski
void convertToColMajor(DATATYPE *A, int *cols, int dim, int *rowDelimiters,
                       DATATYPE *newA, int *newcols, int *rl, int maxrl,
                       int padded)
{
    int pad = 0;
    if (padded && dim % PAD_FACTOR != 0)
    {
        pad = PAD_FACTOR - dim % PAD_FACTOR;
    }

    int newIndex = 0;
    int i,j,p;
    for (j=0; j<maxrl; j++)
    {
        for (i=0; i<dim; i++)
        {
            if (rowDelimiters[i] + j < rowDelimiters[i+1])
            {
                newA[newIndex] = A[rowDelimiters[i]+j];
                newcols[newIndex] = cols[rowDelimiters[i]+j];
            }
            else
            {
                newA[newIndex] = 0;
            }
            newIndex++;
        }
        if (padded)
        {
            for (p=0; p<pad; p++)
            {
                newA[newIndex] = 0;
                newIndex++;
            }
        }
    }
}

#endif
