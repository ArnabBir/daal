/* file: smoothrelu_layer_backward.cpp */
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
//  Implementation of smoothrelu calculation algorithm and types methods.
//--
*/

#include "smoothrelu_layer_backward_types.h"
#include "smoothrelu_layer_types.h"
#include "serialization_utils.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace smoothrelu
{
namespace backward
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_NEURAL_NETWORKS_LAYERS_SMOOTHRELU_BACKWARD_RESULT_ID);
/** Default constructor */
Input::Input() {};
Input::Input(const Input& other) : super(other) {}

/**
 * Returns an input object for the backward smooth relu layer
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
 * Sets an input object for the backward smooth relu layer
 * \param[in] id      Identifier of the input object
 * \param[in] value   Pointer to the object
 */
void Input::set(LayerDataId id, const services::SharedPtr<data_management::Tensor> &value)
{
    services::SharedPtr<layers::LayerData> layerData =
        services::staticPointerCast<layers::LayerData, data_management::SerializationIface>(Argument::get(layers::backward::inputFromForward));
    (*layerData)[id] = value;
}

/**
 * Checks an input object of the backward smooth relu layer
 * \param[in] par     Parameter of the layer
 * \param[in] method  Computation method
 */
services::Status Input::check(const daal::algorithms::Parameter *par, int method) const
{
    const Parameter *param = static_cast<const Parameter *>(par);
    if (!param->propagateGradient) { return services::Status(); }
    services::Status s;
    DAAL_CHECK_STATUS(s, layers::backward::Input::check(par, method));

    const services::Collection<size_t> &inputDimensions = get(layers::backward::inputGradient)->getDimensions();
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(smoothrelu::auxData).get(), auxDataStr(), &inputDimensions));
    return s;
}

/** Default constructor */
Result::Result() : layers::backward::Result() {};

/**
 * Checks the result of the backward smooth relu layer
 * \param[in] input   %Input object for the layer
 * \param[in] par     %Parameter of the layer
 * \param[in] method  Computation method
 */
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, int method) const
{
    const Parameter *param = static_cast<const Parameter *>(par);
    if (!param->propagateGradient) { return services::Status(); }

    const Input *in = static_cast<const Input *>(input);
    services::Status s;
    DAAL_CHECK_STATUS(s, data_management::checkTensor(get(layers::backward::gradient).get(), gradientStr(), &(in->get(smoothrelu::auxData)->getDimensions())));
    return s;
}

}// namespace interface1
}// namespace backward
}// namespace smoothrelu
}// namespace layers
}// namespace neural_networks
}// namespace algorithms
}// namespace daal
