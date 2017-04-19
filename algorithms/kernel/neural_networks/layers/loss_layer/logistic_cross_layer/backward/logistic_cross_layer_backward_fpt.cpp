/* file: logistic_cross_layer_backward_fpt.cpp */
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
//  Implementation of logistic cross calculation algorithm and types methods.
//--
*/

#include "logistic_cross_layer_types.h"
#include "logistic_cross_layer_backward_types.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace loss
{
namespace logistic_cross
{
namespace backward
{
namespace interface1
{
/**
 * Allocates memory to store the result of the backward logistic cross-entropy layer
 * \param[in] input Pointer to an object containing the input data
 * \param[in] method Computation method for the layer
 * \param[in] parameter %Parameter of the backward logistic cross-entropy layer
 */
template <typename algorithmFPType>
DAAL_EXPORT services::Status Result::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    const layers::Parameter *param = static_cast<const Parameter *>(parameter);
    if (!param->propagateGradient) { return services::Status(); }

    if (!get(layers::backward::gradient))
    {
        const Input *in = static_cast<const Input *>(input);

        services::SharedPtr<data_management::Tensor> probabilitiesTable = in->get(auxData);

        if(probabilitiesTable == 0) return services::Status(services::ErrorNullInputNumericTable);

        DAAL_ALLOCATE_TENSOR_AND_SET(layers::backward::gradient, probabilitiesTable->getDimensions());
    }
    return services::Status();
}

template DAAL_EXPORT services::Status Result::allocate<DAAL_FPTYPE>(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method);

}// namespace interface1
}// namespace backward
}// namespace logistic_cross
}// namespace loss
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
