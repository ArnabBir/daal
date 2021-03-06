/* file: implicit_als_train_init_dense_default_batch_impl.i */
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
//  Implementation of defaultDense method for impicit ALS initialization
//--
*/

#ifndef __IMPLICIT_ALS_TRAIN_INIT_DENSE_DEFAULT_BATCH_IMPL_I__
#define __IMPLICIT_ALS_TRAIN_INIT_DENSE_DEFAULT_BATCH_IMPL_I__

#include "service_numeric_table.h"
#include "service_memory.h"
#include "service_blas.h"
#include "implicit_als_train_init_kernel.h"

namespace daal
{
namespace algorithms
{
namespace implicit_als
{
namespace training
{
namespace init
{
namespace internal
{

using namespace daal::services::internal;
using namespace daal::internal;

template <typename algorithmFPType, CpuType cpu>
services::Status ImplicitALSInitKernel<algorithmFPType, defaultDense, cpu>::compute(
                 const NumericTable *dataTable, NumericTable *itemsFactorsTable, const Parameter *parameter, engines::BatchBase &engine)
{
    const size_t nUsers = dataTable->getNumberOfRows();
    const size_t nItems = dataTable->getNumberOfColumns();
    const size_t nFactors = parameter->nFactors;

    const size_t bufSz = (nItems > nFactors ? nItems : nFactors);
    TArray<algorithmFPType, cpu> ones(nUsers);
    TArray<algorithmFPType, cpu> itemsSum(bufSz);
    DAAL_CHECK_MALLOC(ones.get() && itemsSum.get());

    {
        ReadRows<algorithmFPType, cpu> mtData(*const_cast<NumericTable*>(dataTable), 0, nUsers);
        DAAL_CHECK_BLOCK_STATUS(mtData);
        const algorithmFPType *data = mtData.get();
        const algorithmFPType one(1.0);
        service_memset<algorithmFPType, cpu>(ones.get(), one, nUsers);
    /* Parameters of GEMV function */
    char transa = 'N';
    algorithmFPType alpha = 1.0;
    algorithmFPType beta  = 0.0;
        DAAL_INT ione = 1;

    /* Compute sum of rows of input matrix */
        Blas<algorithmFPType, cpu>::xgemv(&transa, (DAAL_INT *)&nItems, (DAAL_INT *)&nUsers, &alpha,
            const_cast<algorithmFPType*>(data), (DAAL_INT *)&nItems,
            ones.get(), (DAAL_INT *)&ione, &beta, itemsSum.get(), &ione);
    }

    WriteOnlyRows<algorithmFPType, cpu> mtItemsFactors(itemsFactorsTable, 0, nItems);
    DAAL_CHECK_BLOCK_STATUS(mtItemsFactors);
    algorithmFPType *itemsFactors = mtItemsFactors.get();

    const algorithmFPType invNUsers = algorithmFPType(1.0) / algorithmFPType(nUsers);
    for (size_t i = 0; i < nItems; i++)
    {
        itemsFactors[i * nFactors] = itemsSum[i] * invNUsers;
    }

    return this->randFactors(nItems, nFactors, itemsFactors, (int*)itemsSum.get(), engine);
    //reusing itemsSum as an array of ints, to save on memory allocation
}

}
}
}
}
}
}

#endif
