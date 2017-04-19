/* file: neural_networks_prediction_feedforward_kernel.h */
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

//++
//  Declaration of template function that calculate neural networks.
//--


#ifndef __NEURAL_NETWORKS_PREDICTION_FEEDFORWARD_KERNEL_H__
#define __NEURAL_NETWORKS_PREDICTION_FEEDFORWARD_KERNEL_H__

#include "neural_networks/neural_networks_prediction.h"
#include "neural_networks/neural_networks_types.h"
#include "neural_networks/neural_networks_prediction_types.h"
#include "kernel.h"
#include "homogen_tensor.h"
#include "service_tensor.h"
#include "service_numeric_table.h"
#include "neural_networks_feedforward.h"

using namespace daal::data_management;
using namespace daal::services;
using namespace daal::internal;
using namespace daal::algorithms::neural_networks::internal;
using namespace daal::algorithms::neural_networks::layers;

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace prediction
{
namespace internal
{
/**
 *  \brief Kernel for neural network calculation
 */
template<typename algorithmFPType, Method method, CpuType cpu>
class NeuralNetworksFeedforwardPredictionKernel : public Kernel
{
public:
    NeuralNetworksFeedforwardPredictionKernel() : lastLayersIndices(NULL),
                                                  lastLayerResults(NULL),
                                                  predictions(NULL) {}

    services::Status compute(const Input *input, Result *result);
    services::Status initialize(const Input *input, const neural_networks::prediction::Parameter *parameter, Result *result);
    services::Status reset();
private:
    size_t nLastLayers;
    size_t nLayers;
    size_t nSamples;
    size_t batchSize;
    LastLayerIndices* lastLayersIndices;
    SharedPtr<HomogenTensor<algorithmFPType> > sample;
    ReadSubtensor<algorithmFPType, cpu> *lastLayerResults;
    WriteOnlySubtensor<algorithmFPType, cpu> *predictions;
};

} // namespace daal::internal
} // namespace prediction
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
