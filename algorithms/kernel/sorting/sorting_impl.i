/* file: sorting_impl.i */
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
//  Sorting observations algorithm implementation
//--
*/

#ifndef __SORTING_IMPL__
#define __SORTING_IMPL__

#include "service_micro_table.h"
#include "service_memory.h"
#include "service_math.h"
#include "service_stat.h"

using namespace daal::internal;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace sorting
{
namespace internal
{
template<Method method, typename algorithmFPType, CpuType cpu>
services::Status SortingKernel<method, algorithmFPType, cpu>::compute(NumericTable *inputTable, NumericTable *outputTable)
{
    size_t nFeatures = inputTable->getNumberOfColumns();
    size_t nVectors  = inputTable->getNumberOfRows();

    ReadRows<algorithmFPType, cpu> inputBlock(*inputTable, 0, nVectors);
    const algorithmFPType *data = inputBlock.get();

    WriteRows<algorithmFPType, cpu> otputBlock(*outputTable, 0, nVectors);
    algorithmFPType *sortedData = otputBlock.get();

    int errorcode = Statistics<algorithmFPType, cpu>::xSort(const_cast<algorithmFPType *>(data), nFeatures, nVectors, sortedData);

    if(errorcode) {this->_errors->add(services::ErrorSortingInternal);}
    DAAL_RETURN_STATUS()
}

} // namespace daal::algorithms::sorting::internal
} // namespace daal::algorithms::sorting
} // namespace daal::algorithms
} // namespace daal

#endif
