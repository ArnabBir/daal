/* file: transposed_conv2d_layer_backward_batch.h */
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
//  Implementation of transposed convolution2d calculation algorithm and types methods.
//--
*/

#include "transposed_conv2d_layer_backward_types.h"
#include "transposed_conv2d_layer_types.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace transposed_conv2d
{
namespace backward
{
namespace interface1
{
/**
 * Allocates memory to store the result of backward 2D transposed convolution layer
 * \param[in] input     Object containing the input data
 * \param[in] parameter %Parameter of backward 2D transposed convolution layer
 * \param[in] method    Computation method
 *
 * \return Status of computations
 */
template <typename algorithmFPType>
DAAL_EXPORT services::Status Result::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    using daal::data_management::Tensor;
    using daal::data_management::HomogenTensor;

    const Input *in = static_cast<const Input *>(input);
    const Parameter *param =  static_cast<const Parameter * >(parameter);

    services::SharedPtr<Tensor> auxDataTable = in->get(auxData);
    services::SharedPtr<Tensor> wTable       = in->get(auxWeights);

    services::Collection<size_t> bDims;
    bDims.push_back(param->nKernels);

    DAAL_CHECK(auxDataTable && wTable, services::ErrorNullInputNumericTable);

    if (param->propagateGradient && !get(layers::backward::gradient))
    {
        set(layers::backward::gradient, services::SharedPtr<Tensor>(
                        new HomogenTensor<algorithmFPType>(auxDataTable->getDimensions(), Tensor::doAllocate)));
    }
    if (!get(layers::backward::weightDerivatives))
    {
        set(layers::backward::weightDerivatives, services::SharedPtr<Tensor>(
                        new HomogenTensor<algorithmFPType>(wTable->getDimensions(), Tensor::doAllocate)));
    }
    if (!get(layers::backward::biasDerivatives))
    {
        set(layers::backward::biasDerivatives, services::SharedPtr<Tensor>(
                        new HomogenTensor<algorithmFPType>(bDims, Tensor::doAllocate)));
    }
    return services::Status();
}

}// namespace interface1
}// namespace forward
}// namespace transposed_conv2d
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
