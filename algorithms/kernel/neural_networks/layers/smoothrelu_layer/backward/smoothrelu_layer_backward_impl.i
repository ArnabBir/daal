/* file: smoothrelu_layer_backward_impl.i */
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
// Implementation of the backward smooth rectifier linear unit (smooth relu) layer
//--
*/

#ifndef __SMOOTHRELU_LAYER_BACKWARD_IMPL_I__
#define __SMOOTHRELU_LAYER_BACKWARD_IMPL_I__

using namespace daal::internal;
using namespace daal::services;

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
namespace internal
{

template<typename algorithmFPType, Method method, CpuType cpu>
void SmoothReLUKernel<algorithmFPType, method, cpu>::compute(Tensor *inputTensor, Tensor *forwardValueTensor, Tensor *resultTensor)
{
    computeImpl<cpu>(inputTensor, this->_errors.get(), [=](size_t fDimN, size_t *fDims, size_t nRowsToProcess, const TensorOffsetLayout &layout)
    {
        ReadSubtensor<algorithmFPType, cpu, Tensor> inputBlock(*inputTensor, fDimN, fDims, 0, nRowsToProcess, layout);
        const algorithmFPType *inputArray = inputBlock.get();

        ReadSubtensor<algorithmFPType, cpu, Tensor> forwardValueBlock(*forwardValueTensor, fDimN, fDims, 0, nRowsToProcess, layout);
        const algorithmFPType *forwardValueArray = forwardValueBlock.get();

        WriteSubtensor<algorithmFPType, cpu, Tensor> resultBlock(*resultTensor, fDimN, fDims, 0, nRowsToProcess, layout);
        algorithmFPType *resultArray = resultBlock.get();

        algorithmFPType one = (algorithmFPType)1.0;
        size_t nDataElements = inputBlock.getSize();

        //res = in * 1/(exp(-fO)+1)
       PRAGMA_IVDEP
       PRAGMA_VECTOR_ALWAYS
        for(size_t i = 0; i < nDataElements; i++)
        {
            resultArray[i] = -forwardValueArray[i];

            /* Arguments filtering before vector exponential function call */
            /* There is a known issue that vExp works slowly on large negative arguments (where results are zero or denormals) */
          #if (__CPUID__(DAAL_CPU) != __avx512_mic__)
            if( resultArray[i] < daal::internal::Math<algorithmFPType,cpu>::vExpThreshold() )
            {
                resultArray[i] = daal::internal::Math<algorithmFPType,cpu>::vExpThreshold();
            }
          #endif
        }

        daal::internal::Math<algorithmFPType,cpu>::vExp(nDataElements, resultArray, resultArray);

       PRAGMA_IVDEP
       PRAGMA_VECTOR_ALWAYS
        for(size_t i = 0; i < nDataElements; i++)
        {
            resultArray[i] = one / (resultArray[i] + one);
            resultArray[i] = inputArray[i] * resultArray[i];
        }
    });
}

} // namespace internal
} // backward
} // namespace smoothrelu
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
