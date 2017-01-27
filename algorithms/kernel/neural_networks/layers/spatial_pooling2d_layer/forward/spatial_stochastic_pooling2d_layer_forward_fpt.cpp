/* file: spatial_stochastic_pooling2d_layer_forward_fpt.cpp */
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
//  Implementation of spatial pooling2d calculation algorithm and types methods.
//--
*/

#include "spatial_stochastic_pooling2d_layer_types.h"
#include "spatial_stochastic_pooling2d_layer_forward_types.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace spatial_stochastic_pooling2d
{
namespace forward
{
namespace interface1
{
/**
 * Allocates memory to store the result of the forward spatial pyramid stochastic 2D pooling layer
 * \param[in] input Pointer to an object containing the input data
 * \param[in] parameter %Parameter of the forward spatial pyramid stochastic 2D pooling layer
 * \param[in] method Computation method for the layer
 */
template <typename algorithmFPType>
DAAL_EXPORT void Result::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    spatial_pooling2d::forward::Result::allocate<algorithmFPType>(input, parameter, method);
    const Input *in = static_cast<const Input *>(input);

    const Parameter *algParameter = static_cast<const Parameter *>(parameter);

    if(!algParameter->predictionStage)
    {
        const services::Collection<size_t> &dataDims = in->get(layers::forward::data)->getDimensions();
        set(auxInputDimensions, createAuxInputDimensions(dataDims));

        services::Collection<size_t> valueDims = computeValueDimensions(dataDims, algParameter);
        set(auxSelectedIndices, data_management::TensorPtr(
                new data_management::HomogenTensor<int>(valueDims, data_management::Tensor::doAllocate)));
    }
}

template DAAL_EXPORT void Result::allocate<DAAL_FPTYPE>(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method);

}// namespace interface1
}// namespace forward
}// namespace spatial_stochastic_pooling2d
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
