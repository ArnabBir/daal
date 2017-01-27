/* file: maximum_pooling2d_layer_forward_fpt.cpp */
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
//  Implementation of maximum_pooling2d calculation algorithm and types methods.
//--
*/

#include "maximum_pooling2d_layer_forward_types.h"
#include "maximum_pooling2d_layer_types.h"

#include "mkl_tensor.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace maximum_pooling2d
{
namespace forward
{
namespace interface1
{
/**
 * Allocates memory to store the result of the forward maximum 2D pooling layer
 * \param[in] input Pointer to an object containing the input data
 * \param[in] parameter %Parameter of the forward maximum 2D pooling layer
 * \param[in] method Computation method for the layer
 */
template <typename algorithmFPType>
DAAL_EXPORT void Result::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    using daal::data_management::Tensor;
    using daal::internal::MklTensor;

    pooling2d::forward::Result::allocate<algorithmFPType>(input, parameter, method);

    const Parameter *algParameter = static_cast<const Parameter *>(parameter);
    const Input *in = static_cast<const Input *>(input);
    const services::Collection<size_t> &dataDims = in->get(layers::forward::data)->getDimensions();
    services::Collection<size_t> valueDims(dataDims);
    computeValueDimensions(valueDims, algParameter);
    if (get(layers::forward::resultForBackward))
    {
        set(auxSelectedIndices, data_management::TensorPtr(
                new MklTensor<double>(valueDims, data_management::Tensor::doAllocate)));
        if(!algParameter->predictionStage)
        {
            set(auxData, in->get(layers::forward::data));
            set(auxInputDimensions, createAuxInputDimensions(dataDims));
        }
    }
}

template DAAL_EXPORT void Result::allocate<DAAL_FPTYPE>(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method);

}// namespace interface1
}// namespace forward
}// namespace maximum_pooling2d
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
