/* file: qr_dense_default_pcl_impl.i */
/*******************************************************************************
* Copyright 2014-2017 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/*
//++
//  Implementation of PCL's qr
//--
*/

#ifndef __QR_KERNEL_DEFAULT_PCL_IMPL_I__
#define __QR_KERNEL_DEFAULT_PCL_IMPL_I__

#define PCL_OK               0
#define PCL_MEMORY_ERROR     services::ErrorMemoryAllocationFailed
#define PCL_MKL_ERROR        services::UnknownError
#define PCL_PARAMETER_ERROR  services::ErrorIncorrectSizeOfArray

#define LAPACK_GEQRF Lapack<algorithmFPType, cpu>::xxgeqrf
#define LAPACK_ORGQR Lapack<algorithmFPType, cpu>::xxorgqr
#define LAPACK_ORMQR Lapack<algorithmFPType, cpu>::xxormqr
#define LAPACK_GESVD Lapack<algorithmFPType, cpu>::xxgesvd

#define _MIN_(a,b) (((a)<(b))?(a):(b))

//#define _DBG_

#if defined _DBG_
    #define PRINTF(func,var) printf("\t%s:\t%s = %.2lf\n", #func, #var, (double)(var));
    #define PRINTI(func,var) printf("\t%s:\t%s = %ld\n",   #func, #var, (long long)(var));
    #define PRINTS(func,var) printf("\t%s:\t%s\n", #func, var);
#else
    #define PRINTF(func,var)
    #define PRINTI(func,var)
    #define PRINTS(func,var)
#endif

#define NTHREADS 24
#define NTILES   10
/*
 * Allocate memory buffer for LAPACK routines.
 *
 * INPUTS
 * A:           Input matrix (nrows x ncols)
 * nrows:       number of rows in input matrix
 * ncols:       number of columns in input matrix
 *
 * OUTPUTS
 * lwork:       Allocated buffer size
 * work:        Allocated buffer pointer
 */
template <typename algorithmFPType, CpuType cpu>
static void work_alloc(   algorithmFPType*  A,
                   const size_t nrows,
                   const size_t ncols,
                   algorithmFPType*  tau,
                   size_t* lwork,
                   algorithmFPType** work )
{
    DAAL_INT mkl_m_q   = nrows;
    DAAL_INT mkl_n_q   = ncols;
    DAAL_INT mkl_lda_q = nrows;
    DAAL_INT mkl_lwork = -1;
    DAAL_INT mkl_info_q;
    algorithmFPType tmpwork_q;

    LAPACK_GEQRF(  mkl_m_q,
                   mkl_n_q,
                   A,
                   mkl_lda_q,
                   tau,
                   &tmpwork_q,
                   mkl_lwork,
                   &mkl_info_q );
    if( mkl_info_q != 0 )
    {
        *work = NULL;
    };

    *lwork = (size_t)tmpwork_q;
    *work  = service_scalable_malloc<algorithmFPType,cpu>( *lwork );

    return;

} /* work_alloc */


/*
 * Deallocate memory buffer for LAPACK routines.
 *
 * INPUTS
 * work:           Pointer to the buffer
 */
template <typename algorithmFPType, CpuType cpu>
static void work_free( algorithmFPType* work )
{
    if( work )
    {
        service_scalable_free<algorithmFPType,cpu>( work );
    }

    return;

}/* work_free */


/*
 * Factor matrix using TSQR. Input matrix is overwritten with packed Q factor.
 *
 * INPUTS
 * A:           Input matrix (nrows x ncols)
 * nrows:       number of rows in input matrix
 * ncols:       number of columns in input matrix
 * nthreads:    number of threads to use for the factorization
 * local_tiles: number of ncols x ncols blocks to assign to each thread.
 *              Should be roughly the per-thread L2 cache size.
 *
 * OUTPUTS
 * A:           Q factor is packed into the A matrix (in-place)
 * tau:         A number of tau vectors are returned. The size of this matrix
 *              is determined in tsqr.cc.
 */
template <typename algorithmFPType, CpuType cpu>
static void tsqr( algorithmFPType*  A,
          const size_t nrows,
          const size_t ncols,
          algorithmFPType*  tau,
          const size_t nthreads,
          const size_t local_tiles,
          size_t lwork,
          algorithmFPType* work,
          int* st )
{
    if( *st != PCL_OK )
    {
        return;
    }

    DAAL_INT mkl_m;
    DAAL_INT mkl_n;
    DAAL_INT mkl_lda;
    DAAL_INT mkl_info;
    DAAL_INT mkl_lwork;

    size_t rows_per_thread  = nrows / nthreads;
    size_t chunk_size       = ( local_tiles - 1 )*ncols;
    size_t tiles_per_thread = ( rows_per_thread + chunk_size - 1 ) / chunk_size;
    size_t Rda              = nthreads*ncols;

    if( nrows < Rda )
    {
        *st = PCL_PARAMETER_ERROR;
        return;
    }

    algorithmFPType* R = service_scalable_malloc<algorithmFPType,cpu>( nthreads * ncols * ncols );
    if( R == NULL )
    {
        *st = PCL_MEMORY_ERROR;
        return;
    }

    daal::threader_for( nthreads, nthreads, [&]( size_t tid )
    {
        DAAL_INT mkl_m_local;
        DAAL_INT mkl_n_local;
        DAAL_INT mkl_lda_local;
        DAAL_INT mkl_info_local;
        DAAL_INT mkl_lwork_local;

        algorithmFPType* R_local   = R   + tid*ncols;
        algorithmFPType* tau_local = tau + tid*tiles_per_thread*ncols + ncols;

        algorithmFPType* a_local   = service_scalable_malloc<algorithmFPType,cpu>( local_tiles * ncols * ncols );
        if( a_local == NULL )
        {
            *st = PCL_MEMORY_ERROR;
            return;
        }

        size_t start                     = tid * rows_per_thread;
        size_t end                       = (tid == (nthreads-1))?nrows:(start + rows_per_thread );
        algorithmFPType* A_local         = A + start*ncols;
        algorithmFPType * mkl_work_local = NULL;
        size_t lwork_local;

        work_alloc<algorithmFPType,cpu>( a_local,
                                         local_tiles*ncols,
                                         ncols,
                                         tau_local,
                                         &lwork_local,
                                         &mkl_work_local );
        if( mkl_work_local == NULL )
        {
            service_scalable_free<algorithmFPType,cpu>( a_local );
            *st = PCL_MEMORY_ERROR;
            return;
        }

        mkl_lwork_local = lwork_local;

        // First tile
        size_t first_height = _MIN_( end - start, chunk_size + ncols );
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < first_height; i++ )
            {
                a_local[i + j*local_tiles*ncols] = A_local[i*ncols + j];
            }
        } /* for(size_t j = 0; j < ncols; j++ )  */

        mkl_m_local   = first_height;
        mkl_n_local   = ncols;
        mkl_lda_local = local_tiles*ncols;

        LAPACK_GEQRF(  mkl_m_local,
                       mkl_n_local,
                       a_local,
                       mkl_lda_local,
                       tau_local,
                       mkl_work_local,
                       mkl_lwork_local,
                       &mkl_info_local );
        if( mkl_info_local != 0 )
        {
            *st = PCL_MKL_ERROR;
        }

        tau_local += ncols;

        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = j + 1; i < first_height; i++ )
            {
                A_local[i*ncols + j] = a_local[i + j*local_tiles*ncols];
            }

           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = j + 1; i < ncols; i++ )
            {
                a_local[i + j*local_tiles*ncols] = 0.0f;
            }
        } /* for(size_t j = 0; j < ncols; j++ )  */

        // The rest of the tiles
        size_t new_start = start + first_height;
        size_t ntiles    = (( end - new_start ) + chunk_size - 1 ) / chunk_size;

        for( size_t tile = 0; tile < ntiles; tile++ )
        {
            size_t row              = new_start + tile * chunk_size;
            size_t height           = _MIN_( end - row, chunk_size );
            algorithmFPType* A_tile = A + row * ncols;

            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = 0; i < height; i++ )
                {
                    a_local[i + local_tiles*ncols*j + ncols] = A_tile[i*ncols + j];
                }
            } /* for(size_t j = 0; j < ncols; j++ )  */

            mkl_m_local   = height+ncols;
            mkl_n_local   = ncols;
            mkl_lda_local = local_tiles*ncols;

            LAPACK_GEQRF(  mkl_m_local,
                           mkl_n_local,
                           a_local,
                           mkl_lda_local,
                           tau_local,
                           mkl_work_local,
                           mkl_lwork_local,
                           &mkl_info_local );
            if( mkl_info_local != 0 )
            {
                *st = PCL_MKL_ERROR;
            }

            tau_local += ncols;

            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = 0; i < height; i++ )
                {
                    A_tile[i*ncols + j] = a_local[i + local_tiles*ncols*j + ncols];
                }
            } /* for(size_t j = 0; j < ncols; j++ )  */

            // Reset area under upper triangle to 0. Just in case MKL set them.
            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_IVDEP
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = j + 1; i < ncols; i++ )
                {
                    a_local[i + j*local_tiles*ncols] = 0.0f;
                }
            } /* for(size_t j = 0; j < ncols; j++ ) */

        } /* for(size_t tile = 0; tile < ntiles; tile++) */

        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < j + 1; i++ )
            {
                R_local[i + Rda*j] = a_local[i + local_tiles*ncols*j];
            }

           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = j + 1; i < ncols; i++ )
            {
                R_local[i + Rda*j] = 0.0f;
            }
        } /* for(size_t j = 0; j < ncols; j++ )  */

        service_scalable_free<algorithmFPType,cpu>( a_local );
        work_free<algorithmFPType,cpu>( mkl_work_local );

    } ); /* daal::threader_for( nthreads, nthreads, [&](size_t tid) */

    mkl_m     = nthreads * ncols;
    mkl_n     = ncols;
    mkl_lda   = nthreads * ncols;
    mkl_lwork = lwork;

    LAPACK_GEQRF(  mkl_m,
                   mkl_n,
                   R,
                   mkl_lda,
                   tau,
                   work,
                   mkl_lwork,
                   &mkl_info );
    if( mkl_info != 0 )
    {
        *st = PCL_MKL_ERROR;
    }

    daal::threader_for( nthreads, nthreads, [&]( size_t tid )
    {
        algorithmFPType* A_local = A + tid * rows_per_thread * ncols;
        algorithmFPType* R_local = R + tid * ncols;

        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < j + 1; i++ )
            {
                A_local[i*ncols + j] = R_local[i + j*nthreads*ncols];
            }
        } /* for(size_t j = 0; j < ncols; j++ )  */

    } ); /* daal::threader_for( nthreads, nthreads, [&](size_t tid) */

    service_scalable_free<algorithmFPType,cpu>( R );

    return;

} /* void tsqr */


/*
 * Recover explicit Q matrix from factored TSQR matrix
 *
 * INPUTS
 * A:           TSQR factor matrix (returned from tsqr())
 * nrows:       number of rows in factor matrix
 * ncols:       number of columns in factor matrix
 * tau:         Tau matrix returned from tsqr()
 * nthreads:    number of threads to use for the factorization.
 *              Must match nthreads used in tsqr()
 * local_tiles: number of ncols x ncols blocks to assign to each thread.
 *              Must match local_tiles used in tsqr()
 *
 * OUTPUTS
 * A:           Explicit Q matrix
 */
template <typename algorithmFPType, CpuType cpu>
static void tsgetq( algorithmFPType*  A,
             const size_t nrows,
             const size_t ncols,
             algorithmFPType*  tau,
             const size_t nthreads,
             const size_t local_tiles,
             size_t lwork,
             algorithmFPType* work,
             int* st )
{
    if( *st != PCL_OK )
    {
        return;
    }

    DAAL_INT mkl_m;
    DAAL_INT mkl_n;
    DAAL_INT mkl_k;
    DAAL_INT mkl_lda;
    DAAL_INT mkl_lwork;
    DAAL_INT mkl_info;

    size_t rows_per_thread  = nrows / nthreads;
    size_t chunk_size       = ( local_tiles - 1 ) * ncols;
    size_t tiles_per_thread = ( rows_per_thread + chunk_size - 1 ) / chunk_size;
    size_t Rda              = nthreads * ncols;

    if( nrows < Rda )
    {
        *st = PCL_PARAMETER_ERROR;
        return;
    }

    algorithmFPType* R = service_scalable_malloc<algorithmFPType,cpu>( nthreads * ncols * ncols );
    if( R == NULL )
    {
        *st = PCL_MEMORY_ERROR;
        return;
    }

    daal::threader_for( nthreads, nthreads, [&]( size_t tid )
    {
        algorithmFPType* R_local   = R + tid * ncols;
        size_t start = tid * rows_per_thread;
        algorithmFPType* A_local   = A + start * ncols;

        // Copy triangles from A
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < j + 1; i++ )
            {
                R_local[i + Rda*j] = A_local[i*ncols + j];
            }
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = j + 1; i < ncols; i++ )
            {
                R_local[i + Rda*j] = 0.0f;
            }
        } /* for(size_t j = 0; j < ncols; j++ ) */

    } ); /* daal::threader_for( nthreads, nthreads, [&](size_t tid) */

    mkl_m     = Rda;
    mkl_n     = ncols;
    mkl_k     = ncols;
    mkl_lda   = Rda;
    mkl_lwork = lwork;

    LAPACK_ORGQR(  mkl_m,
                   mkl_n,
                   mkl_k,
                   R,
                   mkl_lda,
                   tau,
                   work,
                   mkl_lwork,
                   &mkl_info );
    if( mkl_info != 0 )
    {
        *st = PCL_MKL_ERROR;
    }

    daal::threader_for( nthreads, nthreads, [&]( size_t tid )
    {

        char mkl_side_local;
        char mkl_trans_local;
        DAAL_INT mkl_m_local;
        DAAL_INT mkl_n_local;
        DAAL_INT mkl_k_local;
        DAAL_INT mkl_lda_local;
        DAAL_INT mkl_ldc_local;
        DAAL_INT mkl_info_local;
        DAAL_INT mkl_lwork_local;

        algorithmFPType* tau_local = tau + tid * tiles_per_thread * ncols + ncols;
        algorithmFPType* R_local   = R + tid * ncols;
        size_t start               = tid * rows_per_thread;
        size_t end                 = (tid == (nthreads-1))?nrows:(start + rows_per_thread );

        algorithmFPType* a = service_scalable_malloc<algorithmFPType,cpu>( local_tiles * ncols * ncols );
        algorithmFPType* b = service_scalable_malloc<algorithmFPType,cpu>( local_tiles * ncols * ncols );
        if( a == NULL || b == NULL )
        {
            if(a)service_scalable_free<algorithmFPType,cpu>( a );
            if(b)service_scalable_free<algorithmFPType,cpu>( b );
            *st = PCL_MEMORY_ERROR;
            return;
        }

        algorithmFPType* A_local = A + start * ncols;

        size_t lwork_local;
        algorithmFPType*     work_local;

        work_alloc<algorithmFPType,cpu>( A_local,
                                         tiles_per_thread * ncols,
                                         ncols,
                                         tau_local,
                                         &lwork_local,
                                         &work_local );
        if( work_local == NULL )
        {
            service_scalable_free<algorithmFPType,cpu>( a );
            service_scalable_free<algorithmFPType,cpu>( b );
            *st = PCL_MEMORY_ERROR;
            return;
        }

        mkl_lwork_local = lwork_local;

        // Copy stacked triangle to top of "a" buffer
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < j + 1; i++ )
            {
                a[i + j*local_tiles*ncols] = R_local[i + Rda*j];
            }
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = j + 1; i < local_tiles*ncols; i++ )
            {
                a[i + j*local_tiles*ncols] = 0.0;
            }
        } /* for(size_t j = 0; j < ncols; j++ ) */

        // Zero out top of "b" buffer
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < ncols; i++ )
            {
                b[i + j*local_tiles*ncols] = 0.0;
            }
        } /* for( size_t j = 0; j < ncols; j++ ) */

        // Apply tiles in reverse order
        size_t first_height = _MIN_( end - start, chunk_size + ncols );
        size_t new_start    = start + first_height;
        size_t ntiles       = (( end - new_start ) + chunk_size - 1 ) / chunk_size;
        tau_local           = tau_local + ntiles * ncols;  // accounts for first tile

        for( size_t _tile = 0; _tile < ntiles; _tile++ )
        {
            size_t tile             = (ntiles - _tile) - 1;
            size_t row              = new_start + tile * chunk_size;
            size_t height           = _MIN_(end - row, chunk_size);
            algorithmFPType* A_tile = A + row * ncols;

            // Copy Q into bottom portion of "b" buffer
            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_IVDEP
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = 0; i < height; i++ )
                {
                    b[i + local_tiles*ncols*j + ncols] = A_tile[i*ncols + j];
                }
            } /* for( size_t j = 0; j < ncols; j++ )  */

            // Zero out bottom portion of "a" buffer
            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_IVDEP
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = 0; i < height; i++ )
                {
                    a[i + local_tiles*ncols*j + ncols] = 0.0;
                }
            } /* for( size_t j = 0; j < ncols; j++ )  */

            // Apply Q in "b" buffer to "a" buffer
            // "a" buffer has upper triangular top tile
            mkl_side_local  = 'L';
            mkl_trans_local = 'N';
            mkl_m_local     = height + ncols;
            mkl_n_local     = ncols;
            mkl_k_local     = ncols;
            mkl_lda_local   = local_tiles * ncols;
            mkl_ldc_local   = local_tiles * ncols;

            LAPACK_ORMQR(  &mkl_side_local,
                           &mkl_trans_local,
                           &mkl_m_local,
                           &mkl_n_local,
                           &mkl_k_local,
                           b,
                           &mkl_lda_local,
                           tau_local,
                           a,
                           &mkl_ldc_local,
                           work_local,
                           &mkl_lwork_local,
                           &mkl_info_local );
            if( mkl_info_local != 0 )
            {
                *st = PCL_MKL_ERROR;
            }

            tau_local -= ncols;

            // Zero out lower triangular portion of "a" top tile.
            // Just in case MKL wrote something here
            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_IVDEP
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = j+1; i < ncols ; i++ )
                {
                    a[i + local_tiles*ncols*j] = 0.0;
                }
            } /* for( size_t j = 0; j < ncols; j++ ) */

            // Copy reconstructed Q from lower portion of "a" buffer to output
            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = 0; i < height; i++ )
                {
                    A_tile[i*ncols + j] = a[i + local_tiles*ncols*j + ncols];
                }
            } /* for( size_t j = 0; j < ncols; j++ ) */

        } /* for(size_t _tile = 0; _tile < ntiles; _tile++)  */

        // Apply first tile
        // Copy entire Q factor into "b" buffer
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = j + 1; i < first_height; i++ )
            {
                b[i + j*local_tiles*ncols] = A_local[i*ncols + j];
            }
        }

        // Zero out bottom of "a" buffer.
        // Only apply Q to upper triangle of "a".
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = ncols; i < first_height; i++ )
            {
                a[i + local_tiles*ncols*j] = 0.0;
            }
        }

        mkl_side_local  = 'L';
        mkl_trans_local = 'N';
        mkl_m_local     = first_height;
        mkl_n_local     = ncols;
        mkl_k_local     = ncols;
        mkl_lda_local   = local_tiles * ncols;
        mkl_ldc_local   = local_tiles * ncols;

        LAPACK_ORMQR(  &mkl_side_local,
                       &mkl_trans_local,
                       &mkl_m_local,
                       &mkl_n_local,
                       &mkl_k_local,
                       b,
                       &mkl_lda_local,
                       tau_local,
                       a,
                       &mkl_ldc_local,
                       work_local,
                       &mkl_lwork_local,
                       &mkl_info_local );
        if( mkl_info_local != 0 )
        {
            *st = PCL_MKL_ERROR;
        }

        // Copy reconstructed Q back
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < first_height; i++ )
            {
                A_local[i*ncols + j] = a[i + j*local_tiles*ncols];
            }
        }

        service_scalable_free<algorithmFPType,cpu>( a );
        service_scalable_free<algorithmFPType,cpu>( b );
        work_free<algorithmFPType,cpu>( work_local );
    } ); /* daal::threader_for( nthreads, nthreads, [&](size_t tid) */

    service_scalable_free<algorithmFPType,cpu>( R );

    return;

} /* void tsgetq */


/*
 * Apply Q factor from tsqr() to an ncols x ncols matrix and
 * return the result in-place of the ncols x ncols input matrix.
 *
 * INPUTS
 * A:           TSQR factor matrix (returned from tsqr())
 * nrows:       number of rows in factor matrix
 * ncols:       number of columns in factor matrix
 * tau:         Tau matrix returned from tsqr()
 * nthreads:    number of threads to use for the factorization.
 *              Must match nthreads used in tsqr()
 * local_tiles: number of ncols x ncols blocks to assign to each thread.
 *              Must match local_tiles used in tsqr()
 * Rin:         Matrix that Q is applied to. Must be nrows x ncols with
 *              only the top ncols x ncols occupied.
 * Rin_lda:     leading dimension of Rin
 *
 * OUTPUTS
 * Rin:         Output matrix (nrows x ncols)
 */
template <typename algorithmFPType, CpuType cpu>
static void tsapplyq(  algorithmFPType*  A,
               const size_t nrows,
               const size_t ncols,
               algorithmFPType* tau,
               const size_t nthreads,
               const size_t local_tiles,
               algorithmFPType* Rin,
               const size_t Rin_lda,
               size_t lwork,
               algorithmFPType* work,
               int* st )
{
    if( *st != PCL_OK )
    {
        return;
    }

    char mkl_side;
    char mkl_trans;
    DAAL_INT mkl_m;
    DAAL_INT mkl_n;
    DAAL_INT mkl_k;
    DAAL_INT mkl_lda;
    DAAL_INT mkl_ldc;
    DAAL_INT mkl_lwork;
    DAAL_INT mkl_info;

    size_t rows_per_thread  = nrows / nthreads;
    size_t chunk_size       = ( local_tiles - 1 ) * ncols;
    size_t tiles_per_thread = ( rows_per_thread + chunk_size - 1 ) / chunk_size;
    size_t Rda              = nthreads * ncols;

    if(nrows < Rda)
    {
        *st = PCL_PARAMETER_ERROR;
        return;
    }

    algorithmFPType* R  = service_scalable_malloc<algorithmFPType,cpu>( nthreads * ncols * ncols );
    algorithmFPType* R2 = service_scalable_malloc<algorithmFPType,cpu>( nthreads * ncols * ncols );
    if(R == NULL || R2 == NULL)
    {
        if( R )service_scalable_free<algorithmFPType,cpu>( R );
        if( R2 )service_scalable_free<algorithmFPType,cpu>( R2 );
        *st = PCL_MEMORY_ERROR;
        return;
    }

    daal::threader_for( nthreads, nthreads, [&]( size_t tid )
    {
        algorithmFPType* R_local   = R + tid * ncols;
        algorithmFPType* R2_local  = R2 + tid * ncols;
        size_t start = tid * rows_per_thread;
        algorithmFPType* A_local   = A + start * ncols;
        algorithmFPType* C_local   = A + start * ncols;

        // Fill "R_local" buffer with stacked upper triangular matrices
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < j + 1; i++ )
            {
                R_local[i + Rda*j] = A_local[i*ncols + j];
            }

           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = j + 1; i < ncols; i++ )
            {
                R_local[i + Rda*j] = 0.0f;
            }
        }

        if( tid == 0 )
        {
            // Fill "R2_local" top square with top square of matrix being multiplied
            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_IVDEP
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = 0; i < ncols; i++ )
                {
                    R2_local[i + Rda*j] = Rin[j*Rin_lda + i];
                }
            }
        }
        else /* if(tid == 0) */
        {
            // memset 0
            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_IVDEP
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = 0; i < ncols; i++ )
                {
                    R2_local[i + Rda*j] = 0.0;
                }
            }
        } /* if(tid == 0) */
    } ); /* daal::threader_for( nthreads, nthreads, [&](size_t tid) */

    // R2 should now be a full dense matrix
    mkl_side  = 'L';
    mkl_trans = 'N';
    mkl_m     = Rda;
    mkl_n     = ncols;
    mkl_k     = ncols;
    mkl_lda   = Rda;
    mkl_ldc   = Rda;
    mkl_lwork = lwork;

    LAPACK_ORMQR(  &mkl_side,
                   &mkl_trans,
                   &mkl_m,
                   &mkl_n,
                   &mkl_k,
                   R,
                   &mkl_lda,
                   tau,
                   R2,
                   &mkl_ldc,
                   work,
                   &mkl_lwork,
                   &mkl_info );
    if( mkl_info != 0 )
    {
        *st = PCL_MKL_ERROR;
    }

    daal::threader_for( nthreads, nthreads, [&]( size_t tid )
    {
        char mkl_side_local;
        char mkl_trans_local;
        DAAL_INT mkl_m_local;
        DAAL_INT mkl_n_local;
        DAAL_INT mkl_k_local;
        DAAL_INT mkl_lda_local;
        DAAL_INT mkl_ldc_local;
        DAAL_INT mkl_info_local;
        DAAL_INT mkl_lwork_local;

        algorithmFPType* tau_local = tau + tid * tiles_per_thread * ncols + ncols;
        algorithmFPType* R_local   = R + tid * ncols;
        algorithmFPType* R2_local  = R2 + tid * ncols;
        size_t start               = tid * rows_per_thread;
        size_t end                 = (tid == (nthreads-1))?nrows:(start + rows_per_thread );

        algorithmFPType* a = service_scalable_malloc<algorithmFPType,cpu>( local_tiles * ncols * ncols );
        algorithmFPType* b = service_scalable_malloc<algorithmFPType,cpu>( local_tiles * ncols * ncols );
        if( a == NULL || b == NULL )
        {
            if(a)service_scalable_free<algorithmFPType,cpu>( a );
            if(b)service_scalable_free<algorithmFPType,cpu>( b );
            *st = PCL_MEMORY_ERROR;
            return;
        }

        algorithmFPType* A_local = A + start * ncols;
        algorithmFPType * C_local = A + start * ncols;

        size_t lwork_local;
        algorithmFPType* work_local;

        work_alloc<algorithmFPType,cpu>( A_local,
                                         tiles_per_thread * ncols,
                                         ncols,
                                         tau_local,
                                         &lwork_local,
                                         &work_local );
        if( work_local == NULL )
        {
            service_scalable_free<algorithmFPType,cpu>( a );
            service_scalable_free<algorithmFPType,cpu>( b );
            *st = PCL_MEMORY_ERROR;
            return;
        }

        mkl_lwork_local = lwork_local;

        // Copy my square of R2 to top of "a" buffer
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < ncols; i++ )
            {
                a[i + j*local_tiles*ncols] = R2_local[i + Rda*j];
            }
        }

        // Zero out top of "b" buffer
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < ncols; i++ )
            {
                b[i + j*local_tiles*ncols] = 0.0;
            }
        }

        // Apply tiles in reverse order
        size_t first_height = _MIN_( end - start, chunk_size + ncols );
        size_t new_start    = start + first_height;
        size_t ntiles       = (( end - new_start ) + chunk_size - 1 ) / chunk_size;
        tau_local           = tau_local + ntiles * ncols;  // accounts for first tile

        for(size_t _tile = 0; _tile < ntiles; _tile++)
        {
            size_t tile             = ( ntiles - _tile ) - 1;
            size_t row              = new_start + tile * chunk_size;
            size_t height           = _MIN_(end - row, chunk_size);
            algorithmFPType* A_tile = A + row * ncols;
            algorithmFPType* C_tile = A + row * ncols;

            // Copy Q factor into bottom portion of "b" buffer
            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_IVDEP
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = 0; i < height; i++ )
                {
                    b[i + local_tiles*ncols*j + ncols] = A_tile[i*ncols + j];
                }
            }

            // Zero out bottom portion of "a" buffer
            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_IVDEP
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = 0; i < height; i++ )
                {
                    a[i + local_tiles*ncols*j + ncols] = 0.0;
                }
            }

            // Apply Q factor to "a" buffer
            mkl_side_local  = 'L';
            mkl_trans_local = 'N';
            mkl_m_local     = height + ncols;
            mkl_n_local     = ncols;
            mkl_k_local     = ncols;
            mkl_lda_local   = local_tiles * ncols;
            mkl_ldc_local   = local_tiles * ncols;

            LAPACK_ORMQR(  &mkl_side_local,
                           &mkl_trans_local,
                           &mkl_m_local,
                           &mkl_n_local,
                           &mkl_k_local,
                           b,
                           &mkl_lda_local,
                           tau_local,
                           a,
                           &mkl_ldc_local,
                           work_local,
                           &mkl_lwork_local,
                           &mkl_info_local );
            if( mkl_info_local != 0 )
            {
                *st = PCL_MKL_ERROR;
            }

            tau_local -= ncols;

            // Copy bottom portion of "a" buffer to output
            for( size_t j = 0; j < ncols; j++ )
            {
               PRAGMA_IVDEP
               PRAGMA_VECTOR_ALWAYS
                for( size_t i = 0; i < height; i++ )
                {
                    C_tile[i*ncols + j] = a[i + local_tiles*ncols*j + ncols];
                }
            }
        } /* for(size_t _tile = 0; _tile < ntiles; _tile++)  */

        // Apply first tile
        // Fill "b" buffer with entire Q factor
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = j + 1; i < first_height; i++ )
            {
                b[i + j*local_tiles*ncols] = A_local[i*ncols + j];
            }
        }

        // Zero out bottom portion of "a" buffer
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = ncols; i < first_height; i++ )
            {
                a[i + local_tiles*ncols*j] = 0.0;
            }
        }

        // Apply Q to "a" buffer
        mkl_side_local  = 'L';
        mkl_trans_local = 'N';
        mkl_m_local     = first_height;
        mkl_n_local     = ncols;
        mkl_k_local     = ncols;
        mkl_lda_local   = local_tiles * ncols;
        mkl_ldc_local   = local_tiles * ncols;

        LAPACK_ORMQR(  &mkl_side_local,
                       &mkl_trans_local,
                       &mkl_m_local,
                       &mkl_n_local,
                       &mkl_k_local,
                       b,
                       &mkl_lda_local,
                       tau_local,
                       a,
                       &mkl_ldc_local,
                       work_local,
                       &mkl_lwork_local,
                       &mkl_info_local );
        if( mkl_info_local != 0 )
        {
            *st = PCL_MKL_ERROR;
        }

        // Write result from "a" buffer to output
        for( size_t j = 0; j < ncols; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0; i < first_height; i++ )
            {
                C_local[i*ncols + j] = a[i + j*local_tiles*ncols];
            }
        }

        service_scalable_free<algorithmFPType,cpu>( a );
        service_scalable_free<algorithmFPType,cpu>( b );
        work_free<algorithmFPType,cpu>( work_local );

    } ); /* daal::threader_for( nthreads, nthreads, [&](size_t tid) */

    service_scalable_free<algorithmFPType,cpu>( R );
    service_scalable_free<algorithmFPType,cpu>( R2 );

    return;

} /* tsapplyq */

template <typename algorithmFPType, CpuType cpu>
static int qr_pcl( algorithmFPType *A_in,       /* nrows * ncols */
             size_t nrows,
             size_t ncols,
             algorithmFPType *Q_out,     /* nrows * ncols */
             algorithmFPType *R_out )    /* ncols * ncols */
{
    int st                       = PCL_OK;
    static const size_t nthreads = threader_get_threads_number();
    algorithmFPType* tau         = NULL;
    algorithmFPType* work        = NULL;
    size_t lwork;

    size_t local_tiles     = NTILES;
    size_t rows_per_thread = nrows / nthreads;
    size_t chunk_size      = ( local_tiles - 1 ) * ncols;
    size_t taus_per_thread = ( rows_per_thread + chunk_size - 1 ) / chunk_size;
    size_t tau_size        = taus_per_thread * ncols * nthreads + ncols;

    do
    {
        tau = service_scalable_malloc<algorithmFPType,cpu>( tau_size );
        if( tau == NULL )
        {
            st = PCL_MEMORY_ERROR;
            break;
        }

        size_t num       = nrows * ncols;
        size_t nBlocks   = nthreads;
        size_t blockSize = num / nBlocks;
        if( nBlocks * blockSize < num ) { nBlocks++; }

        daal::threader_for( nBlocks, nBlocks, [&]( size_t block )
        {
            size_t b = ( block ) * blockSize;
            size_t e = ( block + 1 ) * blockSize;
            if( e > num ) { e = num; }

           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = b; i < e ; i++ )
            {
                 Q_out[i] = A_in[i];
            }
        } );

        work_alloc<algorithmFPType,cpu>( Q_out,
                                         ncols*nthreads,
                                         ncols,
                                         tau,
                                         &lwork,
                                         &work );
        if( work == NULL )
        {
            st = PCL_MEMORY_ERROR;
            break;
        }

        tsqr<algorithmFPType,cpu>( Q_out,
                                   nrows,
                                   ncols,
                                   tau,
                                   nthreads,
                                   local_tiles,
                                   lwork,
                                   work,
                                   &st );
        if( st != PCL_OK )
        {
            break;
        }

        for( size_t i = 0; i < ncols; i++ )
        {

           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t j = 0; j < i; j++ )
            {
                R_out[ i*ncols + j ] = 0;
            }

           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t j = i; j < ncols; j++ )
            {
                R_out[ i*ncols + j ] = Q_out[ i*ncols + j ];
            }
        }

        tsgetq<algorithmFPType,cpu>( Q_out,
                                     nrows,
                                     ncols,
                                     tau,
                                     nthreads,
                                     local_tiles,
                                     lwork,
                                     work,
                                     &st );

    } while(0);

    if( tau )  service_scalable_free<algorithmFPType,cpu>( tau );
    if( work ) work_free<algorithmFPType,cpu>( work );

    return st;

} /* qr_pcl */


template <typename algorithmFPType, CpuType cpu>
static int svd_pcl( algorithmFPType *A_in,      /* nrows * ncols */
              size_t nrows,
              size_t ncols,
              bool needsU,
              algorithmFPType *U_out,    /* nrows * ncols */
              algorithmFPType *S_out,    /* ncols         */
              bool needsVT,
              algorithmFPType *VT_out )  /* ncols * ncols */
{
    int st                       = PCL_OK;
    static const size_t nthreads = threader_get_threads_number();
    size_t local_tiles           = NTILES;

    char mkl_jobu;
    char mkl_jobvt;
    DAAL_INT mkl_m;
    DAAL_INT mkl_n;
    DAAL_INT mkl_lda;
    DAAL_INT mkl_ldu;
    DAAL_INT mkl_ldvt;
    DAAL_INT mkl_info;
    DAAL_INT mkl_lwork;

    size_t num       = nrows * ncols;
    size_t nBlocks   = nthreads;
    size_t blockSize = num / nBlocks;
    if( nBlocks * blockSize < num ) { nBlocks++; }

    algorithmFPType* tstau  = NULL;
    algorithmFPType* work   = NULL;
    algorithmFPType* R      = NULL;
    algorithmFPType* V      = NULL;
    algorithmFPType* superb = NULL;
    algorithmFPType* R_out  = NULL;

    if(needsU)
    {
        R_out  = U_out;
    }
    else
    {
        R_out = service_scalable_malloc<algorithmFPType,cpu>( nrows * ncols );
        if( R_out == NULL )
        {
            st = PCL_MEMORY_ERROR;
            return st;
        }
    }

    daal::threader_for( nBlocks, nBlocks, [&]( size_t block )
    {
        size_t b = ( block ) * blockSize;
        size_t e = ( block + 1 ) * blockSize;
        if( e > num ) { e = num; }

       PRAGMA_IVDEP
       PRAGMA_VECTOR_ALWAYS
        for( size_t i = b; i < e ; i++ )
        {
             R_out[i] = A_in[i];
        }
    } );

    size_t rows_per_thread = nrows / nthreads;
    size_t chunk_size      = ( local_tiles - 1 ) * ncols;
    size_t taus_per_thread = ( rows_per_thread + chunk_size - 1 ) / chunk_size;
    size_t tau_size        = taus_per_thread * ncols * nthreads + ncols;
    size_t lwork;

    do
    {
        tstau = service_scalable_malloc<algorithmFPType,cpu>( tau_size );
        if( tstau == NULL )
        {
            st = PCL_MEMORY_ERROR;
            break;
        }

        work_alloc<algorithmFPType,cpu>( R_out,
                                         ncols*nthreads,
                                         ncols,
                                         tstau,
                                         &lwork,
                                         &work );
        if( work == NULL )
        {
            st = PCL_MEMORY_ERROR;
            break;
        }

        tsqr<algorithmFPType,cpu>( R_out,
                                   nrows,
                                   ncols,
                                   tstau,
                                   nthreads,
                                   local_tiles,
                                   lwork,
                                   work,
                                   &st );
        if( st != PCL_OK )
        {
            break;
        }

        R = service_scalable_malloc<algorithmFPType,cpu>( ncols * ncols );
        if( R == NULL )
        {
            st = PCL_MEMORY_ERROR;
            break;
        }

        V = service_scalable_malloc<algorithmFPType,cpu>( ncols * ncols );
        if( V == NULL )
        {
            st = PCL_MEMORY_ERROR;
            break;
        }

        superb = service_scalable_malloc<algorithmFPType,cpu>( ncols );
        if( superb == NULL )
        {
            st = PCL_MEMORY_ERROR;
            break;
        }

        for( size_t j = 0 ; j < ncols ; j++ )
        {
           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = 0 ; i < j+1 ; i++ )
            {
                R[j*ncols + i] = R_out[i*ncols + j];
            }

           PRAGMA_IVDEP
           PRAGMA_VECTOR_ALWAYS
            for( size_t i = j+1 ; i < ncols ; i++ )
            {
                R[j*ncols + i] = 0.0;
            }
        }

        mkl_jobu  = (needsU)? 'O':'N';
        mkl_jobvt = (needsVT)?'A':'N';
        mkl_m     = ncols;
        mkl_n     = ncols;
        mkl_lda   = ncols;
        mkl_ldu   = ncols;
        mkl_ldvt  = ncols;
        mkl_lwork = lwork;

        LAPACK_GESVD(  mkl_jobu,
                       mkl_jobvt,
                       mkl_m,
                       mkl_n,
                       R,
                       mkl_lda,
                       S_out,
                       static_cast<algorithmFPType*>(NULL),
                       mkl_ldu,
                       V,
                       mkl_ldvt,
                       work,
                       mkl_lwork,
                       &mkl_info );
        if( mkl_info != 0 )
        {
            st = PCL_MKL_ERROR;
            break;
        }

        if(needsVT)
        {
            for( size_t i = 0 ; i < ncols ; i++ )
            {
               PRAGMA_IVDEP
               PRAGMA_VECTOR_ALWAYS
                for( size_t j = 0 ; j < ncols ; j++ )
                {
                    VT_out[i*ncols + j] = V[j*ncols + i];
                }
            }
        }

        if(needsU)
        {
            tsapplyq<algorithmFPType,cpu>( R_out,
                                           nrows,
                                           ncols,
                                           tstau,
                                           nthreads,
                                           local_tiles,
                                           R,
                                           ncols,
                                           lwork,
                                           work,
                                           &st );
        }

    } while(0);

    if( tstau )  service_scalable_free<algorithmFPType,cpu>( tstau );
    if( R )      service_scalable_free<algorithmFPType,cpu>( R );
    if( V )      service_scalable_free<algorithmFPType,cpu>( V );
    if( superb ) service_scalable_free<algorithmFPType,cpu>( superb );
    if( work )   work_free<algorithmFPType,cpu>( work );

    if(!needsU)
    {
        if( R_out ) service_scalable_free<algorithmFPType,cpu>( R_out );
    }

    return st;

} /* svd_pcl */


#endif /* __QR_KERNEL_DEFAULT_PCL_IMPL_I__ */
