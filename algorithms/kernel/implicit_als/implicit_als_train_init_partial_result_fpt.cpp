/* file: implicit_als_train_init_partial_result_fpt.cpp */
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
//  Implementation of implicit als algorithm and types methods.
//--
*/

#include "implicit_als_training_init_types.h"
#include "implicit_als_train_init_parameter.h"

using namespace daal::data_management;
using namespace daal::services;

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
namespace interface1
{
template <typename algorithmFPType>
DAAL_EXPORT services::Status PartialResultBase::allocate(size_t nParts)
{
    KeyValueDataCollectionPtr outputCollection (new KeyValueDataCollection());
    KeyValueDataCollectionPtr offsetsCollection(new KeyValueDataCollection());
    for (size_t i = 0; i < nParts; i++)
    {
        (*outputCollection )[i].reset(new HomogenNumericTable<int>(NULL, 1, 0));
        (*offsetsCollection)[i].reset(new HomogenNumericTable<int>(1, 1, NumericTable::doAllocate));
    }
    set(outputOfInitForComputeStep3, outputCollection);
    set(offsets, offsetsCollection);
    return services::Status();
}

template <typename algorithmFPType>
DAAL_EXPORT services::Status PartialResult::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    const DistributedInput<step1Local> *algInput = static_cast<const DistributedInput<step1Local> *>(input);
    const Parameter *algParameter = static_cast<const Parameter *>(parameter);
    implicit_als::Parameter modelParameter(algParameter->nFactors);

    set(partialModel, PartialModelPtr(new PartialModel(
                      modelParameter, algInput->getNumberOfItems(), (algorithmFPType)0.0)));

    SharedPtr<HomogenNumericTable<int> > partitionTable = internal::getPartition(algParameter);
    size_t nParts = partitionTable->getNumberOfRows() - 1;
    int *partitionData = partitionTable->getArray();

    this->PartialResultBase::allocate<algorithmFPType>(nParts);

    KeyValueDataCollectionPtr dataPartsCollection    (new KeyValueDataCollection());
    for (size_t i = 0; i < nParts; i++)
    {
        (*dataPartsCollection)[i].reset(new CSRNumericTable((algorithmFPType *)NULL, NULL, NULL,
                algInput->get(data)->getNumberOfRows(), size_t(partitionData[i + 1] - partitionData[i])));
    }
    set(outputOfStep1ForStep2, dataPartsCollection);
    return services::Status();
}

template <typename algorithmFPType>
DAAL_EXPORT services::Status DistributedPartialResultStep2::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    const DistributedInput<step2Local> *algInput = static_cast<const DistributedInput<step2Local> *>(input);
    KeyValueDataCollectionPtr dataPartsCollection = algInput->get(inputOfStep2FromStep1);
    size_t nParts = dataPartsCollection->size();

    this->PartialResultBase::allocate<algorithmFPType>(nParts);

    size_t fullNItems = 0;
    for (size_t i = 0; i < nParts; i++)
    {
        fullNItems += NumericTable::cast((*dataPartsCollection)[i])->getNumberOfColumns();
    }
    set(transposedData, NumericTablePtr(new CSRNumericTable((algorithmFPType *)NULL, NULL, NULL,
            fullNItems, NumericTable::cast((*dataPartsCollection)[0])->getNumberOfRows())));
    return services::Status();
}

template DAAL_EXPORT services::Status PartialResultBase::allocate<DAAL_FPTYPE>(size_t nParts);
template DAAL_EXPORT services::Status PartialResult::allocate<DAAL_FPTYPE>(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method);
template DAAL_EXPORT services::Status DistributedPartialResultStep2::allocate<DAAL_FPTYPE>(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method);

}// namespace interface1
}// namespace init
}// namespace training
}// namespace implicit_als
}// namespace algorithms
}// namespace daal
