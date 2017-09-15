/* file: kmeans_lloyd_impl.i */
/*******************************************************************************
* Copyright 2014-2017 Intel Corporation
* All Rights Reserved.
*
* If this  software was obtained  under the  Intel Simplified  Software License,
* the following terms apply:
*
* The source code,  information  and material  ("Material") contained  herein is
* owned by Intel Corporation or its  suppliers or licensors,  and  title to such
* Material remains with Intel  Corporation or its  suppliers or  licensors.  The
* Material  contains  proprietary  information  of  Intel or  its suppliers  and
* licensors.  The Material is protected by  worldwide copyright  laws and treaty
* provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
* modified, published,  uploaded, posted, transmitted,  distributed or disclosed
* in any way without Intel's prior express written permission.  No license under
* any patent,  copyright or other  intellectual property rights  in the Material
* is granted to  or  conferred  upon  you,  either   expressly,  by implication,
* inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
* property rights must be express and approved by Intel in writing.
*
* Unless otherwise agreed by Intel in writing,  you may not remove or alter this
* notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
* suppliers or licensors in any way.
*
*
* If this  software  was obtained  under the  Apache License,  Version  2.0 (the
* "License"), the following terms apply:
*
* You may  not use this  file except  in compliance  with  the License.  You may
* obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*
* Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
* distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the   License  for the   specific  language   governing   permissions  and
* limitations under the License.
*******************************************************************************/

/*
//++
//  Implementation of auxiliary functions used in Lloyd method
//  of K-means algorithm.
//--
*/

#include "service_memory.h"
#include "service_numeric_table.h"
#include "service_defines.h"
#include "service_error_handling.h"

#include "threading.h"
#include "service_blas.h"
#include "service_spblas.h"

// CPU intrinsics for Intel Compiler only
#if defined (__INTEL_COMPILER) && defined(__linux__) && defined(__x86_64__)
    #include <immintrin.h>
#endif


namespace daal
{
namespace algorithms
{
namespace kmeans
{
namespace internal
{

using namespace daal::internal;
using namespace daal::services;
using namespace daal::services::internal;

template<typename algorithmFPType, CpuType cpu>
struct tls_task_t
{
    algorithmFPType *mkl_buff;
    algorithmFPType *cS1;
    int    *cS0;
    algorithmFPType goalFunc;
};

template<typename algorithmFPType, CpuType cpu>
struct task_t
{
    daal::tls<tls_task_t<algorithmFPType, cpu>*> *tls_task;
    algorithmFPType *clSq;
    algorithmFPType *cCenters;

    int      dim;
    int      clNum;
    int      max_block_size;

};

template<typename algorithmFPType, CpuType cpu>
services::Status kmeansInitTask(int dim, int clNum, algorithmFPType *centroids, void *& res)
{
    res = nullptr;
    typedef struct task_t<algorithmFPType, cpu> TTask;
    TTask *t = (TTask *)daal::services::daal_malloc(sizeof(TTask));
    DAAL_CHECK(t, services::ErrorMemoryAllocationFailed);

    t->dim       = dim;
    t->clNum     = clNum;
    t->cCenters  = centroids;
    t->max_block_size = 512;

    /* Allocate memory for all arrays inside TLS */
    t->tls_task = new daal::tls<tls_task_t<algorithmFPType, cpu>*>([=]()-> tls_task_t<algorithmFPType, cpu> *
    {
        tls_task_t<algorithmFPType, cpu> *tt = new tls_task_t<algorithmFPType, cpu>;
        if(!tt)
            return nullptr;

        tt->mkl_buff = service_calloc<algorithmFPType, cpu>(t->max_block_size * t->clNum);
        if(!tt->mkl_buff)
        {
            delete tt;
            return nullptr;
        }

        tt->cS1      = service_calloc<algorithmFPType, cpu>(t->clNum * t->dim);
        if(!tt->cS1)
        {
            service_free<algorithmFPType, cpu>(tt->mkl_buff);
            delete tt;
            return nullptr;
        }

        tt->cS0      = service_calloc<int,cpu>(t->clNum);
        if(!tt->cS0)
        {
            delete tt;
            return nullptr;
        }

        tt->goalFunc = (algorithmFPType)(0.0);

        return tt;
    } ); /* Allocate memory for all arrays inside TLS: end */

    if(!t->tls_task)
    {
        daal::services::daal_free(t);
        return services::Status(services::ErrorMemoryAllocationFailed);
    }

    t->clSq      = service_calloc<algorithmFPType, cpu>(clNum);
    if(!t->clSq)
    {
        daal::services::daal_free(t);
        return Status(services::ErrorMemoryAllocationFailed);
    }

    for(size_t k=0;k<clNum;k++)
    {
        for(size_t j=0;j<dim;j++)
        {
            t->clSq[k] += centroids[k*dim + j]*centroids[k*dim + j] * 0.5;
        }
    }

    void * task_id;
    *(size_t*)(&task_id) = (size_t)t;

    res = task_id;
    return Status();
}

template<typename algorithmFPType>
struct Fp2IntSize {};
template<> struct Fp2IntSize<float>  { typedef int IntT;     };
template<> struct Fp2IntSize<double> { typedef __int64 IntT; };

template<typename algorithmFPType, CpuType cpu, int assignFlag>
services::Status addNTToTaskThreadedDense(void *task_id, const NumericTable *ntData, algorithmFPType *catCoef, NumericTable *ntAssign = 0)
{
    struct task_t<algorithmFPType, cpu> *t  = static_cast<task_t<algorithmFPType, cpu> *>(task_id);

    const size_t n = ntData->getNumberOfRows();

    const size_t blockSizeDeafult = t->max_block_size;

    size_t nBlocks = n / blockSizeDeafult;
    nBlocks += (nBlocks*blockSizeDeafult != n);

    SafeStatus safeStat;
    daal::threader_for(nBlocks, nBlocks, [=, &safeStat](int k)
    {
        struct tls_task_t<algorithmFPType, cpu> *tt = t->tls_task->local();
        size_t blockSize = blockSizeDeafult;
        if( k == nBlocks-1 )
        {
            blockSize = n - k*blockSizeDeafult;
        }

        ReadRows<algorithmFPType, cpu> mtData(*const_cast<NumericTable *>(ntData), k*blockSizeDeafult, blockSize);
        DAAL_CHECK_BLOCK_STATUS_THR(mtData);
        const algorithmFPType *data = mtData.get();

        size_t p           = t->dim;
        size_t nClusters   = t->clNum;
        algorithmFPType *inClusters = t->cCenters;
        algorithmFPType *clustersSq = t->clSq;
        int    *cS0        = tt->cS0;
        algorithmFPType *cS1        = tt->cS1;
        algorithmFPType *trg        = &(tt->goalFunc);
        algorithmFPType *x_clusters = tt->mkl_buff;

        WriteOnlyRows<int, cpu> assignBlock(assignFlag ? const_cast<NumericTable *>(ntAssign) : nullptr, k*blockSizeDeafult, blockSize);
        int* assignments = nullptr;
        if(assignFlag)
        {
            DAAL_CHECK_BLOCK_STATUS_THR(assignBlock);
            assignments = assignBlock.get();
        }

        char transa = 't';
        char transb = 'n';
        DAAL_INT _m = blockSize;
        DAAL_INT _n = nClusters;
        DAAL_INT _k = p;
        algorithmFPType alpha = -1.0;
        DAAL_INT lda = p;
        DAAL_INT ldy = p;
        algorithmFPType beta = 1.0;
        DAAL_INT ldaty = blockSize;

      PRAGMA_IVDEP
        for (size_t j = 0; j < nClusters; j++)
        {
            for (size_t i = 0; i < blockSize; i++)
            {
                x_clusters[i + j*blockSize] = clustersSq[j];
            }
        }

        Blas<algorithmFPType, cpu>::xxgemm(&transa, &transb, &_m, &_n, &_k, &alpha, data,
                                           &lda, inClusters, &ldy, &beta, x_clusters, &ldaty);

        typedef typename Fp2IntSize<algorithmFPType>::IntT algIntType;

      PRAGMA_ICC_OMP(simd simdlen(16))
        for (algIntType i = 0; i < (algIntType)blockSize; i++)
        {
            algorithmFPType minGoalVal = x_clusters[i];
            algIntType minIdx = 0;

            for (algIntType j = 0; j < (algIntType)nClusters; j++)
            {
                algorithmFPType localGoalVal = x_clusters[i + j*blockSize];
                if( minGoalVal > localGoalVal )
                {
                    minGoalVal = localGoalVal;
                    minIdx = j;
                }
            }

            minGoalVal *= 2.0;

            *((algIntType*)&(x_clusters[i])) = minIdx;
            x_clusters[i+blockSize] = minGoalVal;
        }

        algorithmFPType goal = (algorithmFPType)0;
        for (size_t i = 0; i < blockSize; i++)
        {
            size_t minIdx = *((algIntType*)&(x_clusters[i]));
            algorithmFPType minGoalVal = x_clusters[i+blockSize];

          PRAGMA_ICC_NO16(omp simd reduction(+:minGoalVal))
            for (size_t j = 0; j < p; j++)
            {
                cS1[minIdx * p + j] += data[i*p + j];
                minGoalVal += data[ i*p + j ] * data[ i*p + j ];
            }

            cS0[minIdx]++;

            goal += minGoalVal;

            if(assignFlag)
            {
                assignments[i] = (int)minIdx;
            }
        } /* for (size_t i = 0; i < blockSize; i++) */

        *trg  += goal;
    } ); /* daal::threader_for( nBlocks, nBlocks, [=](int k) */
    return safeStat.detach();
}

template<typename algorithmFPType, CpuType cpu, int assignFlag>
services::Status addNTToTaskThreadedCSR(void *task_id, const NumericTable *ntDataGen, algorithmFPType *catCoef, NumericTable *ntAssign = 0)
{
    CSRNumericTableIface *ntData  = dynamic_cast<CSRNumericTableIface *>(const_cast<NumericTable *>(ntDataGen));

    struct task_t<algorithmFPType, cpu> *t  = static_cast<task_t<algorithmFPType, cpu> *>(task_id);

    size_t n = ntDataGen->getNumberOfRows();

    size_t blockSizeDeafult = t->max_block_size;

    size_t nBlocks = n / blockSizeDeafult;
    nBlocks += (nBlocks*blockSizeDeafult != n);

    SafeStatus safeStat;
    daal::threader_for(nBlocks, nBlocks, [=, &safeStat](int k)
    {
        struct tls_task_t<algorithmFPType, cpu> *tt = t->tls_task->local();
        size_t blockSize = blockSizeDeafult;
        if( k == nBlocks-1 )
        {
            blockSize = n - k*blockSizeDeafult;
        }

        ReadRowsCSR<algorithmFPType, cpu> dataBlock(ntData, k*blockSizeDeafult, blockSize);
        DAAL_CHECK_BLOCK_STATUS_THR(dataBlock);

        const algorithmFPType *data  = dataBlock.values();
        const size_t *colIdx = dataBlock.cols();
        const size_t *rowIdx = dataBlock.rows();

        size_t p           = t->dim;
        size_t nClusters   = t->clNum;
        algorithmFPType *inClusters = t->cCenters;
        algorithmFPType *clustersSq = t->clSq;
        int    *cS0        = tt->cS0;
        algorithmFPType *cS1        = tt->cS1;
        algorithmFPType *trg        = &(tt->goalFunc);
        algorithmFPType *x_clusters = tt->mkl_buff;

        WriteOnlyRows<int, cpu> assignBlock(assignFlag ? ntAssign : nullptr, k*blockSizeDeafult, blockSize);
        int* assignments = nullptr;
        if(assignFlag)
        {
            DAAL_CHECK_BLOCK_STATUS_THR(assignBlock);
            assignments = assignBlock.get();
        }
        {
            char transa = 'n';
            DAAL_INT _n = blockSize;
            DAAL_INT _p = p;
            DAAL_INT _c = nClusters;
            algorithmFPType alpha = 1.0;
            algorithmFPType beta  = 0.0;
            DAAL_INT ldaty = blockSize;
            char matdescra[6] = {'G',0,0,'F',0,0};

            SpBlas<algorithmFPType, cpu>::xxcsrmm(&transa, &_n, &_c, &_p, &alpha, matdescra,
                                                  data, (DAAL_INT *)colIdx, (DAAL_INT *)rowIdx,
                                                  inClusters, &_p, &beta, x_clusters, &_n);
        }

        size_t csrCursor=0;
        for (size_t i = 0; i < blockSize; i++)
        {
            algorithmFPType minGoalVal = clustersSq[0] - x_clusters[i];
            size_t minIdx = 0;

            for (size_t j = 0; j < nClusters; j++)
            {
                if( minGoalVal > clustersSq[j] - x_clusters[i + j*blockSize] )
                {
                    minGoalVal = clustersSq[j] - x_clusters[i + j*blockSize];
                    minIdx = j;
                }
            }

            minGoalVal *= 2.0;

            size_t valuesNum = rowIdx[i+1]-rowIdx[i];
            for (size_t j = 0; j < valuesNum; j++)
            {
                cS1[minIdx * p + colIdx[csrCursor]-1] += data[csrCursor];
                minGoalVal += data[csrCursor]*data[csrCursor];
                csrCursor++;
            }

            *trg += minGoalVal;

            cS0[minIdx]++;

            if(assignFlag)
            {
                assignments[i] = (int)minIdx;
            }
        }
    } );
    return safeStat.detach();
}

template<Method method, typename algorithmFPType, CpuType cpu, int assignFlag>
services::Status addNTToTaskThreaded(void *task_id, const NumericTable *ntData, algorithmFPType *catCoef, NumericTable *ntAssign = 0 )
{
    if(method == lloydDense)
    {
        return addNTToTaskThreadedDense<algorithmFPType, cpu, assignFlag>( task_id, ntData, catCoef, ntAssign );
    }
    else if(method == lloydCSR)
    {
        return addNTToTaskThreadedCSR<algorithmFPType, cpu, assignFlag>( task_id, ntData, catCoef, ntAssign );
    }
    DAAL_ASSERT(false);
    return services::Status();
}

template<Method method, typename algorithmFPType, CpuType cpu>
services::Status getNTAssignmentsThreaded(void *task_id, const NumericTable *ntData, const NumericTable *ntAssign, algorithmFPType *catCoef)
{
    struct task_t<algorithmFPType, cpu> *t  = static_cast<task_t<algorithmFPType, cpu> *>(task_id);

    const size_t n = ntData->getNumberOfRows();

    size_t blockSizeDeafult = t->max_block_size;

    size_t nBlocks = n / blockSizeDeafult;
    nBlocks += (nBlocks*blockSizeDeafult != n);

    SafeStatus safeStat;
    daal::threader_for(nBlocks, nBlocks, [=, &safeStat](int k)
    {
        struct tls_task_t<algorithmFPType, cpu> *tt = t->tls_task->local();
        size_t blockSize = blockSizeDeafult;
        if( k == nBlocks-1 )
        {
            blockSize = n - k*blockSizeDeafult;
        }

        ReadRows<algorithmFPType, cpu> mtData(*const_cast<NumericTable*>(ntData), k*blockSizeDeafult, blockSize);
        DAAL_CHECK_BLOCK_STATUS_THR(mtData);
        WriteOnlyRows<int, cpu> mtAssign(*const_cast<NumericTable*>(ntAssign), k*blockSizeDeafult, blockSize);
        DAAL_CHECK_BLOCK_STATUS_THR(mtAssign);
        const algorithmFPType *data = mtData.get();
        int *assign = mtAssign.get();

        size_t p           = t->dim;
        size_t nClusters   = t->clNum;
        algorithmFPType *inClusters = t->cCenters;
        algorithmFPType *clustersSq = t->clSq;
        algorithmFPType *x_clusters = tt->mkl_buff;

        char transa = 't';
        char transb = 'n';
        DAAL_INT _m = nClusters;
        DAAL_INT _n = blockSize;
        DAAL_INT _k = p;
        algorithmFPType alpha = 1.0;
        DAAL_INT lda = p;
        DAAL_INT ldy = p;
        algorithmFPType beta = 0.0;
        DAAL_INT ldaty = nClusters;

        Blas<algorithmFPType, cpu>::xxgemm(&transa, &transb, &_m, &_n, &_k, &alpha, inClusters,
                                           &lda, data, &ldy, &beta, x_clusters, &ldaty);

        for (size_t i = 0; i < blockSize; i++)
        {
            algorithmFPType minGoalVal = clustersSq[0] - x_clusters[i * nClusters];
            size_t minIdx = 0;

            for (size_t j = 0; j < nClusters; j++)
            {
                if( minGoalVal > clustersSq[j] - x_clusters[i*nClusters + j] )
                {
                    minGoalVal = clustersSq[j] - x_clusters[i*nClusters + j];
                    minIdx = j;
                }
            }

            assign[i] = minIdx;
        }

    } );
    return safeStat.detach();
}

template<typename algorithmFPType, CpuType cpu>
int kmeansUpdateCluster(void *task_id, int jidx, algorithmFPType *s1)
{
    int i, j;
    struct task_t<algorithmFPType, cpu> *t = static_cast<task_t<algorithmFPType, cpu> *>(task_id);

    int idx   = (int)jidx;
    int dim   = t->dim;
    int clNum = t->clNum;

    int s0=0;

    t->tls_task->reduce( [&](tls_task_t<algorithmFPType, cpu> *tt)-> void
    {
        s0 += tt->cS0[idx];
    } );

    t->tls_task->reduce( [ = ](tls_task_t<algorithmFPType, cpu> *tt)-> void
    {
        int j;
      PRAGMA_IVDEP
        for(j=0;j<dim;j++)
        {
            s1[j] += tt->cS1[idx*dim + j];
        }
    } );

    return s0;
}

template<typename algorithmFPType, CpuType cpu>
void kmeansClearClusters(void *task_id, algorithmFPType *goalFunc)
{
    int i, j;
    struct task_t<algorithmFPType, cpu> *t = static_cast<task_t<algorithmFPType, cpu> *>(task_id);

    if( t->clNum != 0)
    {
        t->clNum = 0;

        if( goalFunc!= 0 )
        {
            *goalFunc = (algorithmFPType)(0.0);

            t->tls_task->reduce( [ = ](tls_task_t<algorithmFPType, cpu> *tt)-> void
            {
                (*goalFunc) += tt->goalFunc;
            } );
        }

        t->tls_task->reduce( [ = ](tls_task_t<algorithmFPType, cpu> *tt)-> void
        {
            service_free<int, cpu>( tt->cS0 );
            service_free<algorithmFPType, cpu>( tt->cS1 );
            service_free<algorithmFPType, cpu>( tt->mkl_buff );
            delete tt;
        } );
        delete t->tls_task;

        service_free<algorithmFPType, cpu>( t->clSq );

    }

    daal::services::daal_free(t);
}

} // namespace daal::algorithms::kmeans::internal
} // namespace daal::algorithms::kmeans
} // namespace daal::algorithms
} // namespace daal
