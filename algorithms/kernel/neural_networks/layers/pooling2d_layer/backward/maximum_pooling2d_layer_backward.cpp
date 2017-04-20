/* file: maximum_pooling2d_layer_backward.cpp */
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

#include "maximum_pooling2d_layer_backward_types.h"
#include "maximum_pooling2d_layer_types.h"
#include "serialization_utils.h"

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
namespace backward
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_NEURAL_NETWORKS_LAYERS_MAXIMUM_POOLING2D_BACKWARD_RESULT_ID);
/**
 * Default constructor
 */
Input::Input() {}

/**
 * Returns an input object for backward maximum 2D pooling layer
 * \param[in] id    Identifier of the input object
 * \return          %Input object that corresponds to the given identifier
 */
data_management::TensorPtr Input::get(LayerDataId id) const
{
    services::SharedPtr<layers::LayerData> inputData = get(layers::backward::inputFromForward);
    return services::staticPointerCast<data_management::Tensor, data_management::SerializationIface>((*inputData)[id]);
}

/**
 * Returns an input object for backward maximum 2D pooling layer
 * \param[in] id    Identifier of the input object
 * \return          %Input object that corresponds to the given identifier
 */
data_management::NumericTablePtr Input::get(LayerDataNumericTableId id) const
{
    services::SharedPtr<layers::LayerData> inputData = get(layers::backward::inputFromForward);
    return services::staticPointerCast<data_management::NumericTable, data_management::SerializationIface>((*inputData)[id]);
}

/**
 * Sets an input object for the backward maximum 2D pooling layer
 * \param[in] id  Identifier of the input object
 * \param[in] ptr Pointer to the object
 */
void Input::set(LayerDataId id, const data_management::TensorPtr &ptr)
{
    services::SharedPtr<layers::LayerData> inputData = get(layers::backward::inputFromForward);
    (*inputData)[id] = ptr;
}

/**
 * Sets an input object for the backward maximum 2D pooling layer
 * \param[in] id  Identifier of the input object
 * \param[in] ptr Pointer to the object
 */
void Input::set(LayerDataNumericTableId id, const data_management::NumericTablePtr &ptr)
{
    services::SharedPtr<layers::LayerData> inputData = get(layers::backward::inputFromForward);
    (*inputData)[id] = ptr;
}

/**
 * Checks an input object for the backward maximum 2D pooling layer
 * \param[in] parameter Algorithm parameter
 * \param[in] method    Computation method
 */
void Input::check(const daal::algorithms::Parameter *parameter, int method) const
{
    const Parameter *param = static_cast<const Parameter *>(parameter);
    if (!param->propagateGradient) { return; }

    pooling2d::backward::Input::check(parameter, method);
    if(this->_errors->size() > 0) { return; }

    data_management::NumericTablePtr auxInputDimensions = get(maximum_pooling2d::auxInputDimensions);
    const services::Collection<size_t> &inputGradDims = get(layers::backward::inputGradient)->getDimensions();

    DAAL_CHECK_EX(inputGradDims.size() >= 2, services::ErrorIncorrectNumberOfDimensionsInTensor, services::ParameterName, inputGradientStr());

    if(!data_management::checkTensor(get(maximum_pooling2d::auxSelectedIndices).get(), this->_errors.get(), auxSelectedIndicesStr(), &inputGradDims)) { return; }
    if(!data_management::checkNumericTable(auxInputDimensions.get(), this->_errors.get(), auxInputDimensionsStr(), data_management::packed_mask, 0,
                                           inputGradDims.size(), 1)) { return; }
}

data_management::NumericTablePtr Input::getAuxInputDimensions() const
{
    return get(auxInputDimensions);
}

/**
 * Default constructor
 */
Result::Result() {}

/**
 * Checks the result of the backward maximum 2D pooling layer
 * \param[in] input     %Input object for the layer
 * \param[in] parameter %Parameter of the layer
 * \param[in] method    Computation method
 */
void Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, int method) const
{
    const Parameter *param = static_cast<const Parameter *>(parameter);
    if (!param->propagateGradient) { return; }

    pooling2d::backward::Result::check(input, parameter, method);
}

}// namespace interface1
}// namespace backward
}// namespace maximum_pooling2d
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
