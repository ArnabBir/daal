/* file: abs_layer_backward_batch_container.h */
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
//  Implementation of abs layer container.
//--
*/

#ifndef __ABS_LAYER_BACKWARD_BATCH_CONTAINER_H__
#define __ABS_LAYER_BACKWARD_BATCH_CONTAINER_H__

#include "neural_networks/layers/abs/abs_layer.h"
#include "abs_layer_backward_kernel.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace abs
{
namespace backward
{
namespace interface1
{
template<typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::BatchContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::AbsKernel, algorithmFPType, method);
}

template<typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::~BatchContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template<typename algorithmFPType, Method method, CpuType cpu>
void BatchContainer<algorithmFPType, method, cpu>::compute()
{
    abs::backward::Input *input = static_cast<abs::backward::Input *>(_in);
    abs::backward::Result *result = static_cast<abs::backward::Result *>(_res);

    const layers::Parameter *par = static_cast<const layers::Parameter *>(_par);
    if (!par->propagateGradient) { return; }

    daal::services::Environment::env &env = *_env;

    Tensor *inputTensor  = input->get(layers::backward::inputGradient).get();
    Tensor *dataTensor   = input->get(abs::auxData).get();
    Tensor *resultTensor = result->get(layers::backward::gradient).get();

    __DAAL_CALL_KERNEL(env, internal::AbsKernel, __DAAL_KERNEL_ARGUMENTS(algorithmFPType, method), compute, inputTensor, dataTensor, resultTensor);
}
} // namespace interface1
} // namespace backward

} // namespace abs
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
