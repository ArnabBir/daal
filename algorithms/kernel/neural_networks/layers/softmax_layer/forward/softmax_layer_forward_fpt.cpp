/* file: softmax_layer_forward_fpt.cpp */
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
//  Implementation of softmax calculation algorithm and types methods.
//--
*/

#include "softmax_layer_forward_types.h"
#include "softmax_layer_types.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace softmax
{
namespace forward
{
namespace interface1
{
/**
* Allocates memory to store the result of the forward softmax layer
* \param[in] input     Pointer to an object containing the input data
* \param[in] par      %Parameter of the backward softmax layer
* \param[in] method    Computation method
*/
template <typename algorithmFPType>
DAAL_EXPORT services::Status Result::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, const int method)
{
    const layers::forward::Input *in = static_cast<const layers::forward::Input * >(input);
    services::Status s;
    if (!get(layers::forward::value))
    {
        DAAL_ALLOCATE_TENSOR_AND_SET(layers::forward::value, in->get(layers::forward::data)->getDimensions());
    }
    const layers::Parameter *parameter = static_cast<const layers::Parameter * >(par);
    if(!parameter->predictionStage)
    {
        if (!get(layers::forward::resultForBackward))
        {
            set(layers::forward::resultForBackward, services::SharedPtr<LayerData>(new LayerData()));
        }
        s |= setResultForBackward(input);
    }
    return s;
}

template DAAL_EXPORT services::Status Result::allocate<DAAL_FPTYPE>(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method);

}// namespace interface1
}// namespace forward
}// namespace softmax
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
