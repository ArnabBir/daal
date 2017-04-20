/* file: logistic_cross_layer_forward_fpt.cpp */
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
#include "logistic_cross_layer_forward_types.h"

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
namespace forward
{
namespace interface1
{

/**
 * Allocates memory to store the result of the forward logistic cross-entropy layer
 * \param[in] input Pointer to an object containing the input data
 * \param[in] method Computation method for the layer
 * \param[in] parameter %Parameter of the forward logistic cross-entropy layer
 */
template <typename algorithmFPType>
DAAL_EXPORT void Result::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    const Input *in = static_cast<const Input * >(input);

    services::Collection<size_t> valueDim(1);
    valueDim[0] = 1;
    if (!get(layers::forward::value))
    {
        DAAL_ALLOCATE_TENSOR_AND_SET(layers::forward::value, valueDim);
    }

    if (!get(layers::forward::resultForBackward))
    {
        set(layers::forward::resultForBackward, services::SharedPtr<LayerData>(new LayerData()));
    }

    const layers::Parameter *par = static_cast<const layers::Parameter * >(parameter);
    if(!par->predictionStage)
    {
        setResultForBackward(input);
    }
}

template DAAL_EXPORT void Result::allocate<DAAL_FPTYPE>(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method);

}// namespace interface1
}// namespace forward
}// namespace logistic_cross
}// namespace loss
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
