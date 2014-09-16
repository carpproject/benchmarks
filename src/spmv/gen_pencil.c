/* PENCIL implementation of single and double-precision SpMV */

#define DATATYPE float
#define FNAME_CSR pencil_csr_scalar_sp
#define FNAME_ELLPACKR pencil_ellpackr_sp
#include "spmv.h"
#undef DATATYPE
#undef FNAME_CSR
#undef FNAME_ELLPACKR

#define DATATYPE double
#define FNAME_CSR pencil_csr_scalar_dp
#define FNAME_ELLPACKR pencil_ellpackr_dp
#include "spmv.h"
#undef DATATYPE
#undef FNAME_CSR
#undef FNAME_ELLPACKR
