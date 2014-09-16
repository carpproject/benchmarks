#include <float.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pencil_runtime.h"
#include "measure-time.h"

#ifdef DATATYPE_SP
#define DATATYPE float
#define FNAME_CSR_SCALAR pencil_csr_scalar_sp
#define FNAME_ELLPACKR pencil_ellpackr_sp
#define FNAME_CSR_SCALAR_REF ref_csr_scalar_sp
#define FNAME_ELLPACKR_REF ref_ellpackr_sp
#define IS_EQUAL(a,b) (fabsf((a) - (b)) <= FLT_EPSILON * fabsf(a))
#elif DATATYPE_DP
#define DATATYPE double
#define FNAME_CSR_SCALAR pencil_csr_scalar_dp
#define FNAME_ELLPACKR pencil_ellpackr_dp
#define FNAME_CSR_SCALAR_REF ref_csr_scalar_dp
#define FNAME_ELLPACKR_REF ref_ellpackr_dp
#define IS_EQUAL(a,b) (fabs((a) - (b)) <= DBL_EPSILON * fabs(a))
#else
#error Please define either DATATYPE_SP or DATATYPE_DP
#endif

void initRandomMatrix(int *cols, int *rowDelimiters, const int n, const int dim);
void convertToColMajor(DATATYPE *A, int *cols, int dim, int *rowDelimiters,
                       DATATYPE *newA, int *newcols, int *rl, int maxrl,
                       int padded);

enum Benchmark {
    bm_csr,
    bm_ellpackr
};

struct Options {
    /* Number of passes. */
    int nPasses;

    /* Matrix size. */
    int size;

    /* Benchmark version to run. */
    enum Benchmark benchmark;
};

static struct Options options;

/* Print vector to stdout.
 */
void printVector(int n, DATATYPE *array) {
    for (int i = 0; i < n; i++) {
        printf("%f\n", array[i]);
    }
}

/* Compare two vectors. */
void validateVector(const int dim, DATATYPE expected[restrict const static dim], DATATYPE actual[restrict const static dim]) {
    for (int i = 0; i < dim; i++) {
        if (!IS_EQUAL(expected[i], actual[i])) {
            fprintf(stderr, "Validation error at element %d: ", i);
            fprintf(stderr, "expected %f", expected[i]);
            fprintf(stderr, "   found %f", actual[i]);
            fprintf(stderr, "\n");
        }
    }
}

/* Fill an array with random values.
 */
void fillArray(int n, DATATYPE *array) {
    for (int i = 0; i < n; i++) {
        array[i] = (DATATYPE)(10.0 * rand() / (RAND_MAX + 1.0f));
    }
}

/* Generate a square matrix in CSR format with the given number of rows and 1% nonzero entries.
 */
void generateCsrMatrix(int nRows, int *nNonzero, int **rowDelims, int **columns, DATATYPE **values) {
    *nNonzero = nRows * nRows / 100;    // As with SHOC, make 1% of matrix entries nonzero
    *values = pencil_alloc(*nNonzero * sizeof(DATATYPE));
    *rowDelims = pencil_alloc((nRows+1) * sizeof(int));
    *columns = pencil_alloc(*nNonzero * sizeof(int));
    fillArray(*nNonzero, *values);
    initRandomMatrix(*columns, *rowDelims, *nNonzero, nRows);
}

/* Compute row lengths for a CSR matrix and return the length of the widest row.
 */
int getRowLengths(int nRows, int rowDelims[restrict const static nRows+1], int rowLengths[restrict const static nRows]) {
    int maxLength = 0;
    for (int i = 0; i < nRows; i++) {
        rowLengths[i] = rowDelims[i+1] - rowDelims[i];
        if (rowLengths[i] > maxLength) {
            maxLength = rowLengths[i];
        }
    }
    return maxLength;
}

/* Generate an ELLPACK-R matrix in column-major order.
 *
 * First generates a CSR matrix and then transforms it to a column-major
 * ELLPACK-R matrix.
 */
void generateEllpackrColMajMatrix(int nRows, int *nNonzero, int *maxRowLength, int **rowLengths, int **columns, DATATYPE **values) {
    int *csrRowDelims;
    int *csrColumns;
    DATATYPE *csrValues;
    generateCsrMatrix(nRows, nNonzero, &csrRowDelims, &csrColumns, &csrValues);

    *rowLengths = pencil_alloc(nRows * sizeof(int));
    *maxRowLength = getRowLengths(nRows, csrRowDelims, *rowLengths);

    *columns = pencil_alloc(nRows * *maxRowLength * sizeof(int));
    *values = pencil_alloc(nRows * *maxRowLength * sizeof(DATATYPE));
    convertToColMajor(csrValues, csrColumns, nRows, csrRowDelims, *values, *columns, *rowLengths, *maxRowLength, 0);

    pencil_free(csrRowDelims);
    pencil_free(csrColumns);
    pencil_free(csrValues);
}

/* Test SpMV on a CSR matrix.
 */
void testCsr(int nRows) {
    int *rowDelims;
    int *columns;
    DATATYPE *values;
    int nNonzero;
    generateCsrMatrix(nRows, &nNonzero, &rowDelims, &columns, &values);

    // Get an input vector with random values
    DATATYPE *vector = pencil_alloc(nRows * sizeof(DATATYPE));
    fillArray(nRows, vector);

    // Fill reference output with random values
    DATATYPE *refOut = malloc(nRows * sizeof(DATATYPE));
    fillArray(nRows, refOut);

    // Fill PPCG output with random values
    DATATYPE *ppcgOut = pencil_alloc(nRows * sizeof(DATATYPE));
    fillArray(nRows, ppcgOut);

    FNAME_CSR_SCALAR_REF(nRows, nNonzero, rowDelims, columns, values, vector, refOut);

    START_MEASURE_TIME
    for (int i = 0; i < options.nPasses; i++) {
        FNAME_CSR_SCALAR(nRows, nNonzero, rowDelims, columns, values, vector, ppcgOut);
    }
    STOP_MEASURE_TIME

    validateVector(nRows, refOut, ppcgOut);
    PRINT_TIME

    pencil_free(values);
    pencil_free(rowDelims);
    pencil_free(columns);
    pencil_free(vector);
    free(refOut);
    pencil_free(ppcgOut);
}

/* Test SpMV on an ELLPACK-R matrix.
 */
void testEllpackR(int nRows) {
    int nNonzero;
    int maxRowLength;
    int *rowLengths;
    int *columns;
    DATATYPE *values;
    generateEllpackrColMajMatrix(nRows, &nNonzero, &maxRowLength, &rowLengths, &columns, &values);

    // Get an input vector with random values
    DATATYPE *vector = pencil_alloc(nRows * sizeof(DATATYPE));
    fillArray(nRows, vector);

    // Fill reference output with random values
    DATATYPE *refOut = malloc(nRows * sizeof(DATATYPE));
    fillArray(nRows, refOut);

    // Fill PPCG output with random values
    DATATYPE *ppcgOut = pencil_alloc(nRows * sizeof(DATATYPE));
    fillArray(nRows, ppcgOut);

    FNAME_ELLPACKR_REF(nRows, maxRowLength, rowLengths, columns, values, vector, refOut);

    START_MEASURE_TIME
    for (int i = 0; i < options.nPasses; i++) {
        FNAME_ELLPACKR(nRows, maxRowLength, rowLengths, columns, values, vector, ppcgOut);
    }
    STOP_MEASURE_TIME

    validateVector(nRows, refOut, ppcgOut);
    PRINT_TIME

    pencil_free(values);
    pencil_free(rowLengths);
    pencil_free(columns);
    pencil_free(vector);
    free(refOut);
    pencil_free(ppcgOut);
}

void printUsage(const char *name) {
    fprintf(stderr, "Usage: %s [-i iters] [-n passes] [-s size] csr|ellpackr\n", name);
}

void parseCommandLine(int argc, char *argv[]) {
    options.nPasses = 1;
    options.size = 16384;

    int opt;
    while ((opt = getopt(argc, argv, "i:n:s:")) != -1) {
        switch (opt) {
        case 'i':
            fprintf(stderr, "Varying the number of kernel invocations is not supported.\n");
            exit(EXIT_FAILURE);
            break;
        case 'n':
            options.nPasses = atoi(optarg);
            break;
        case 's':
            options.size = atoi(optarg);
            break;
        default:
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Insufficient number of arguments.\n");
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[optind], "csr") == 0) {
        options.benchmark = bm_csr;
    }
    else if (strcmp(argv[optind], "ellpackr") == 0) {
        options.benchmark = bm_ellpackr;
    }
    else {
        fprintf(stderr, "Invalid benchmark '%s'\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    parseCommandLine(argc, argv);

    pencil_init();

    if (options.benchmark == bm_csr) {
        testCsr(options.size);
    }
    else if (options.benchmark == bm_ellpackr) {
        testEllpackR(options.size);
    }

    pencil_shutdown();

    return EXIT_SUCCESS;
}
