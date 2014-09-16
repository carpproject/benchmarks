/* Template file containing DATATYPE-agnostic PENCIL function definitions. */

void FNAME_CSR(int nRows, int nNonzero,
        int rowDelims[restrict const static nRows+1],
        int columns[restrict const static nNonzero],
        DATATYPE mat[restrict const static nNonzero],
        DATATYPE vec[restrict const static nRows],
        DATATYPE out[restrict const static nRows]) {
#pragma scop
    __pencil_assume(nRows > 0);
    __pencil_assume(nNonzero > 0);

#pragma pencil independent
    for (int i = 0; i < nRows; i++) {
        int start = rowDelims[i];
        int end = rowDelims[i+1];
        DATATYPE accum = 0.0;

        for (int j = start; j < end; j++) {
            int col = columns[j];

            accum += mat[j] * vec[col];
        }
        out[i] = accum;
    }
#pragma endscop
}

/* Vector variant for CSR is missing. */

void FNAME_ELLPACKR(int nRows, int maxRowLength,
        int rowLengths[restrict const static nRows],
        int columns[restrict const static maxRowLength][nRows],
        DATATYPE mat[restrict const static maxRowLength][nRows],
        DATATYPE vec[restrict const static nRows],
        DATATYPE out[restrict const static nRows]) {
#pragma scop
    __pencil_assume(nRows > 0);
    __pencil_assume(maxRowLength > 0);

#pragma pencil independent
    for (int i = 0; i < nRows; i++) {
        int rowLength = rowLengths[i];
        DATATYPE accum = 0.0;

        for (int j = 0; j < rowLength; j++) {
            int col = columns[j][i];

            accum += mat[j][i] * vec[col];
        }
        out[i] = accum;
    }
#pragma endscop
}
