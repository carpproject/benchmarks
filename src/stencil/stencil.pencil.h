void FNAME(unsigned int nIter, unsigned int nRows, unsigned int nPaddedColumns,
           unsigned int nColumns,
           FTYPE data[const restrict static 2][nRows][nPaddedColumns],
           FTYPE center, FTYPE cardinal, FTYPE diagonal)
{
#pragma scop
    __pencil_assume (nRows >= 3);
    __pencil_assume (nColumns >= 3);
    __pencil_assume (nIter > 0);
    __pencil_assume (nPaddedColumns >= nColumns);
    for (unsigned int iter = 0; iter < nIter; ++iter)
    {
        unsigned int old_idx = iter % 2;
        unsigned int new_idx = (iter + 1) % 2;
#pragma pencil independent
        for (unsigned int i = 1; i < nRows - 1; ++i)
        {
            for (unsigned int j = 1; j < nColumns - 1; ++j)
            {
                FTYPE old_center = data[old_idx][i][j];
                FTYPE old_NSEW_values =
                  data[old_idx][i - 1][j] +
                  data[old_idx][i + 1][j] +
                  data[old_idx][i][j - 1] +
                  data[old_idx][i][j + 1];
                FTYPE old_diag_values =
                  data[old_idx][i - 1][j - 1] +
                  data[old_idx][i - 1][j + 1] +
                  data[old_idx][i + 1][j - 1] +
                  data[old_idx][i + 1][j + 1];
                data[new_idx][i][j] =
                  center * old_center +
                  cardinal * old_NSEW_values +
                  diagonal * old_diag_values;
            }
        }
    }
#pragma endscop
}
