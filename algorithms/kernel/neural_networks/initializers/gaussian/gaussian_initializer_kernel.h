/* file: gaussian_initializer_kernel.h */
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
//  Declaration of template function that calculate gaussian.
//--


#ifndef __GAUSSIAN_INITIALIZER_KERNEL_H__
#define __GAUSSIAN_INITIALIZER_KERNEL_H__

#include "neural_networks/initializers/gaussian/gaussian_initializer.h"
#include "neural_networks/initializers/gaussian/gaussian_initializer_types.h"
#include "kernel.h"
#include "service_math.h"
#include "numeric_table.h"
#include "layer_forward.h"
#include "layer_forward_types.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace initializers
{
namespace gaussian
{
namespace internal
{

/**
 *  \brief Kernel for gaussian calculation
 */
template<typename algorithmFPType, Method method, CpuType cpu>
class GaussianKernel : public Kernel
{
public:
    void compute(const gaussian::Parameter *parameter, Tensor *resultTensor);
};

} // internal
} // gaussian
} // initializers
} // neural_networks
} // algorithms
} // daal

#endif
