/* file: transposed_conv2d_layer_backward.cpp */
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
#include "serialization_utils.h"

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
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_NEURAL_NETWORKS_LAYERS_TRANSPOSED_CONV2D_BACKWARD_RESULT_ID);
/**
 * Default constructor
 */
Input::Input() {};

/**
 * Returns an input object for backward 2D transposed convolution layer
 * \param[in] id    Identifier of the input object
 * \return          %Input object that corresponds to the given identifier
 */
services::SharedPtr<data_management::Tensor> Input::get(LayerDataId id) const
{
    services::SharedPtr<layers::LayerData> layerData =
        services::staticPointerCast<layers::LayerData, data_management::SerializationIface>(Argument::get(layers::backward::inputFromForward));
    return services::staticPointerCast<data_management::Tensor, data_management::SerializationIface>((*layerData)[id]);
}

/**
 * Sets input for the backward 2D transposed convolution layer
 * \param[in] id    Identifier of the input  object
 * \param[in] value Input object to set
 */
void Input::set(LayerDataId id, const services::SharedPtr<data_management::Tensor> &value)
{
    services::SharedPtr<layers::LayerData> layerData = get(layers::backward::inputFromForward);
    (*layerData)[id] = value;
}

/**
 * Checks an input object of the 2D transposed convolution layer
 * \param[in] parameter %Parameter of layer
 * \param[in] method    Computation method of the layer
 */
void Input::check(const daal::algorithms::Parameter *parameter, int method) const
{
    layers::backward::Input::check(parameter, method);
    if( this->_errors->size() > 0 ) { return; }

    services::SharedPtr<data_management::Tensor> xTensor = get(auxData);
    if (!data_management::checkTensor(xTensor.get(), this->_errors.get(), auxDataStr())) { return; }

    const Parameter *par = static_cast<const Parameter *>(parameter);

    services::Collection<size_t> gradDims;
    size_t c1 = par->valueSizes.size[0];
    size_t c2 = par->valueSizes.size[1];

    const services::Collection<size_t> &xDims = xTensor->getDimensions();
    if(c1 == 0 && c2 == 0)
    {
        c1 = par->strides.size[0] * xDims[par->indices.dims[0]] + par->kernelSizes.size[0] - par->strides.size[0] - 2 * par->paddings.size[0];
        c2 = par->strides.size[1] * xDims[par->indices.dims[1]] + par->kernelSizes.size[1] - par->strides.size[1] - 2 * par->paddings.size[1];
    }
    for(size_t i = 0; i < xDims.size(); i++)
    {
        if(i == par->indices.dims[0]) { gradDims.push_back(c1); }
        else if(i == par->indices.dims[1]) { gradDims.push_back(c2); }
        else if(i == par->groupDimension) { gradDims.push_back(par->nKernels); }
        else { gradDims.push_back( xDims[i] ); }
    }
    if (!data_management::checkTensor(get(layers::backward::inputGradient).get(), this->_errors.get(), inputGradientStr(), &gradDims)) { return; }

    services::Collection<size_t> wDims;
    if (par->nGroups > 1) {
        wDims.push_back(par->nGroups);
    }
    wDims.push_back(xTensor->getDimensionSize(par->groupDimension) / (par->nGroups));
    wDims.push_back((par->nKernels) / (par->nGroups));
    wDims.push_back(par->kernelSizes.size[0]);
    wDims.push_back(par->kernelSizes.size[1]);
    if (!data_management::checkTensor(get(auxWeights).get(), this->_errors.get(), auxWeightsStr(), &wDims)) { return; }
}

/**
 * Default constructor
 */
Result::Result() : layers::backward::Result() {}

/**
 * Checks the result of the 2D transposed convolution layer
 * \param[in] input   %Input object of the layer
 * \param[in] par     %Parameter of the layer
 * \param[in] method  Computation method of the layer
 */
void Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, int method) const
{
    layers::backward::Result::check(input, par, method);
    if( this->_errors->size() > 0 ) { return; }

    const Input *algInput = static_cast<const Input *>(input);
    const Parameter *param = static_cast<const Parameter *>(par);

    if (param->propagateGradient)
    {
        if (!data_management::checkTensor(get(layers::backward::gradient).get(), this->_errors.get(), gradientStr(),
                                          &(algInput->get(auxData)->getDimensions()))) { return; }
    }
    if (!data_management::checkTensor(get(layers::backward::weightDerivatives).get(), this->_errors.get(), weightDerivativesStr(),
                                      &(algInput->get(auxWeights)->getDimensions()))) { return; }

    services::Collection<size_t> bDims;
    bDims.push_back(param->nKernels);

    if (!data_management::checkTensor(get(layers::backward::biasDerivatives).get(), this->_errors.get(), biasDerivativesStr(), &bDims)) { return; }
}

}// namespace interface1
}// namespace forward
}// namespace transposed_conv2d
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
