/* file: cordistance_full_impl.i */
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
//  Implementation of correlation distance for result in full layout.
//--
*/
#include "service_defines.h"
using namespace daal::internal;

namespace daal
{
namespace algorithms
{
namespace correlation_distance
{
namespace internal
{
template <typename algorithmFPType, CpuType cpu>
int corDistanceFull(const NumericTable* xTable, NumericTable* rTable)
{
    size_t p   = xTable->getNumberOfColumns();      /* Dimension of input feature vector */
    size_t n   = xTable->getNumberOfRows();         /* Number of input feature vectors   */

    size_t i, j;

    size_t nBlocks = n / blockSizeDefault;
    nBlocks += (nBlocks*blockSizeDefault != n);

    /* compute major diagonal blocks of the distance matrix */
    daal::threader_for(nBlocks, nBlocks, [ = ](size_t k1)
    {
        DAAL_INT blockSize1 = blockSizeDefault;
        if (k1 == nBlocks - 1)
        {
            blockSize1 = n - k1*blockSizeDefault;
        }

        /* read access to blockSize1 rows in input dataset at k1*blockSizeDefault*p row */
        algorithmFPType* x;
        BlockMicroTable<algorithmFPType, readOnly, cpu> xBlock( xTable );
        xBlock.getBlockOfRows(k1*blockSizeDefault, blockSize1, &x);

        /* write access to blockSize1 rows in output dataset at k1*blockSizeDefault*p row */
        algorithmFPType* r;
        BlockMicroTable<algorithmFPType, writeOnly, cpu> rBlock( rTable );
        rBlock.getBlockOfRows(k1*blockSizeDefault, blockSize1, &r);
        /* move to respective column position in output dataset */
        algorithmFPType* rr = r + k1 * blockSizeDefault;


        algorithmFPType sum[blockSizeDefault], buf[blockSizeDefault * blockSizeDefault];

        /* compute sums for elements in each row of the block */
        for ( size_t i = 0; i < blockSize1; i++ )
        {
            algorithmFPType s = (algorithmFPType)0.0;
             PRAGMA_SIMD_ASSERT
             for ( size_t j = 0; j < p; j++ )
             {
                 s += x[i*p+j];
             }
             sum[i] = s;
         }

        /* calculate sum^t * sum */
        algorithmFPType one  = (algorithmFPType)1.0;
        algorithmFPType zero = (algorithmFPType)0.0;
        algorithmFPType alpha = one, beta = zero;
        char transa = 'N', transb = 'T';
        DAAL_INT m = blockSize1, k = 1, nn = blockSize1;
        DAAL_INT lda = m, ldb = nn, ldc = m;

        Blas<algorithmFPType, cpu>::xxgemm(&transa, &transb, &m, &nn, &k, &alpha, sum, &lda, sum, &ldb, &beta, buf, &ldc);

        /* calculate x * x^t - 1/p * sum^t * sum */
        alpha = one; beta = -one / (algorithmFPType)p;
        transa = 'T'; transb = 'N';
        m = blockSize1, k = p, nn = blockSize1;
        lda = k, ldb = k, ldc = m;

        Blas<algorithmFPType, cpu>::xxgemm(&transa, &transb, &m, &nn, &k, &alpha, x, &lda, x, &ldb, &beta, buf, &ldc);

        PRAGMA_SIMD_ASSERT
        for ( size_t i = 0; i < blockSize1; i++)
        {
            if (buf[i * blockSize1 + i] > (algorithmFPType)0.0)
            {
                buf[i * blockSize1 + i] = (algorithmFPType)1.0 / daal::internal::Math<algorithmFPType,cpu>::sSqrt(buf[i*blockSize1 + i]);
            }
        }

        for ( size_t i = 0; i < blockSize1; i++)
        {
            PRAGMA_SIMD_ASSERT
            for ( size_t j = i + 1; j < blockSize1; j++)
            {
                buf[i * blockSize1 + j] = 1.0 - buf[i * blockSize1 + j] * buf[i * blockSize1 + i] * buf[j * blockSize1 + j];
            }
        }

        for (size_t i = 0; i < blockSize1; i++)
        {
            PRAGMA_SIMD_ASSERT
            for (size_t j = i; j < blockSize1; j++)
            {
                rr[i * n + j] = buf[i*blockSize1 + j];
            }
        }

        xBlock.release();
        rBlock.release();
    } );

    /* compute off-diagonal blocks of the distance matrix */
    daal::threader_for(nBlocks, nBlocks, [ = ](size_t k1)
    {
        DAAL_INT blockSize1 = blockSizeDefault;
        if (k1 == nBlocks - 1)
        {
            blockSize1 = n - k1*blockSizeDefault;
        }

        size_t shift1 = k1 * blockSizeDefault;
        /* read access to blockSize1 rows in input dataset at k1*blockSizeDefault row */
        algorithmFPType* x1;
        BlockMicroTable<algorithmFPType, readOnly, cpu> xBlock1( xTable );
        xBlock1.getBlockOfRows(shift1, blockSize1, &x1);

        algorithmFPType sum1[blockSizeDefault];

        /* compute sums for elements in each row of the block x1 */
        for ( size_t i = 0; i < blockSize1; i++ )
        {
             algorithmFPType s = (algorithmFPType)0.0;
             PRAGMA_SIMD_ASSERT
             for ( size_t j = 0; j < p; j++ )
             {
                 s += x1[i*p+j];
             }
             sum1[i] = s;
         }


        daal::threader_for(nBlocks-k1-1, nBlocks-k1-1, [ =, &sum1 ](size_t k3)
        {
            DAAL_INT blockSize2 = blockSizeDefault;
            size_t k2 = k3+k1+1;
            size_t nl = n, pl=p;
            algorithmFPType* sum1l = const_cast<algorithmFPType *>(sum1);

            if (k2 == nBlocks - 1)
            {
                blockSize2 = nl - k2*blockSizeDefault;
            }

            size_t shift2 = k2 * blockSizeDefault;

            /* read access to blockSize1 rows in input dataset at k1*blockSizeDefault row */
            algorithmFPType* x2;
            BlockMicroTable<algorithmFPType, readOnly, cpu> xBlock2( xTable );
            xBlock2.getBlockOfRows(shift2, blockSize2, &x2);

            algorithmFPType diag[blockSizeDefault], buf[blockSizeDefault * blockSizeDefault];
            algorithmFPType* r;

            BlockMicroTable<algorithmFPType, writeOnly, cpu> rBlock( rTable );

            rBlock.getBlockOfRows(shift2, blockSize2, &r);

            for(size_t i = 0; i < blockSize2; i++ )
            {
                diag[i] = r[i * nl + shift2+i];
            }
            rBlock.release();

            /* write access to blockSize2 rows in output dataset at k2*blockSizeDefault row */
            rBlock.getBlockOfRows(shift1, blockSize1, &r);
            /* move to respective column position in output dataset */
            algorithmFPType* rr = r + shift2;

            algorithmFPType sum2[blockSizeDefault];

            /* compute sums for elements in each row of the block x2 */
            for ( size_t i = 0; i < blockSize2; i++ )
            {
                 algorithmFPType s = (algorithmFPType)0.0;
                 PRAGMA_SIMD_ASSERT
                 for ( size_t j = 0; j < pl; j++ )
                 {
                     s += x2[i*pl+j];
                 }
                 sum2[i] = s;
            }

            /* calculate sum1^t * sum2 */
            algorithmFPType one  = (algorithmFPType)1.0;
            algorithmFPType zero = (algorithmFPType)0.0;
            algorithmFPType alpha = one, beta = zero;
            char transa = 'N', transb = 'T';
            DAAL_INT m = blockSize2, k = 1, nn = blockSize1;
            DAAL_INT lda = m, ldb = nn, ldc = m;

            Blas<algorithmFPType, cpu>::xxgemm(&transa, &transb, &m, &nn, &k, &alpha, sum2, &lda, sum1l, &ldb, &beta, buf, &ldc);

            /* calculate x1 * x2^t - 1/p * sum1^t * sum2 */
            alpha = one; beta = -one/(algorithmFPType)pl;
            transa = 'T'; transb = 'N';
            m = blockSize2; k = pl; nn = blockSize1;
            lda = k; ldb = k; ldc = m;

            Blas<algorithmFPType, cpu>::xxgemm(&transa, &transb, &m, &nn, &k, &alpha, x2, &lda, x1, &ldb, &beta, buf, &ldc);

            for (size_t i = 0; i < blockSize1; i++)
            {
                PRAGMA_SIMD_ASSERT
                for (size_t j = 0; j < blockSize2; j++)
                {
                    buf[i * blockSize2 + j] = 1.0 - buf[i * blockSize2 + j] * r[i * nl + (shift1+i)] * diag[j];
                }
            }

            for (size_t i = 0; i < blockSize1; i++)
            {
                PRAGMA_SIMD_ASSERT
                for (size_t j = 0; j < blockSize2; j++)
                {
                    rr[i * nl + j] = buf[i*blockSize2 + j];
                }
            }

            xBlock2.release();
            rBlock.release();
        } );

        xBlock1.release();
    } );

    // copy upper triangular of r into lower triangular and unit diagonal
    daal::threader_for(nBlocks, nBlocks, [ = ](size_t k1)
    {
        size_t blockSize1 = blockSizeDefault;
        if (k1 == nBlocks - 1)
        {
            blockSize1 = n - k1*blockSizeDefault;
        }

        algorithmFPType *r1;
        BlockMicroTable<algorithmFPType, readWrite, cpu>  rBlock1( rTable );

        size_t shift1 = k1 * blockSizeDefault;

        rBlock1.getBlockOfRows(shift1, blockSize1, &r1);

        algorithmFPType* rr1 = r1 + shift1;

        for ( size_t i = 0; i < blockSize1; i++ )
        {
            rr1[i*n+i] = 0.0;
            for ( size_t j = i+1; j < blockSize1; j++ )
            {
                rr1[j*n+i] = rr1[i*n + j];
            }
        }

        daal::threader_for(nBlocks-k1-1, nBlocks-k1-1, [ = ](size_t k3)
        {
            size_t k2 = k3 + k1 + 1;
            size_t nl = n;
            BlockMicroTable<algorithmFPType, writeOnly, cpu>  rBlock2( rTable );
            algorithmFPType *r2, *rr1, *rr2;
            size_t blockSize2 = blockSizeDefault;
            if (k2 == nBlocks - 1)
            {
                blockSize2 = nl - k2*blockSizeDefault;
            }

            size_t shift2 = k2 * blockSizeDefault;
            rBlock2.getBlockOfRows(shift2, blockSize2, &r2);

            rr1 = r1 + shift2;
            rr2 = r2 + shift1;

            for (size_t i = 0; i < blockSize1; i++)
            {
                for (size_t j = 0; j < blockSize2; j++)
                {
                    rr2[j*nl+i] = rr1[i*nl + j];
                }
            }
            rBlock2.release();
        } );

        rBlock1.release();
    } );

     return 0;
}

} // namespace internal

} // namespace correlation_distance

} // namespace algorithms

} // namespace daal
