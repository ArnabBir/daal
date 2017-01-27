/* file: classifier_predict_fpt.cpp */
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
//  Implementation of classifier prediction Result.
//--
*/

#include "classifier_predict_types.h"

namespace daal
{
namespace algorithms
{
namespace classifier
{
namespace prediction
{
namespace interface1
{

/**
 * Allocates memory for storing prediction results of the classification algorithm
 * \tparam  algorithmFPType     Data type for storing prediction results
 * \param[in] input     Pointer to the input objects of the classification algorithm
 * \param[in] parameter Pointer to the parameters of the classification algorithm
 * \param[in] method    Computation method
 */
template <typename algorithmFPType>
DAAL_EXPORT void Result::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    set(prediction, data_management::NumericTablePtr(
            new data_management::HomogenNumericTable<algorithmFPType>(
                1, (static_cast<const InputIface *>(input))->getNumberOfRows(),
                data_management::NumericTableIface::doAllocate)));
}

template DAAL_EXPORT void Result::allocate<DAAL_FPTYPE>(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, const int method);

}
}
}
}
}
