/* file: relu_layer_forward_impl.i */
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
//  Implementation of relu algorithm
//--
*/

#include "service_tensor.h"
#include "service_dnn.h"
#include "mkl_tensor.h"

#ifndef __RELU_LAYER_FORWARD_IMPL_I__
#define __RELU_LAYER_FORWARD_IMPL_I__

using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace relu
{
namespace forward
{
namespace internal
{

template<typename algorithmFPType, Method method, CpuType cpu>
services::Status ReLUKernel<algorithmFPType, method, cpu>::compute(Tensor *inputTensor, Tensor *resultTensor)
{
    MklTensor<algorithmFPType> *inputMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(inputTensor);
    MklTensor<algorithmFPType> *resultMklTensor = dynamic_cast<MklTensor<algorithmFPType>*>(resultTensor);

    if (inputMklTensor != 0 && resultMklTensor != 0)
    {
        dnnLayout_t inputLayout = (dnnLayout_t)inputMklTensor->getDnnLayout();
        dnnLayout_t resultLayout;

        dnnError_t err;

        if (reluPrim == NULL)
        {
            err = dnn::xReLUCreateForward( &reluPrim, inputLayout, (algorithmFPType)0.0); ON_ERR(err);
        }

        if (inputMklTensor != resultMklTensor)
        {
            err = dnn::xLayoutCreateFromPrimitive(&resultLayout, reluPrim, dnnResourceDst); ON_ERR(err);
            resultMklTensor->setDnnLayout(resultLayout);
        }

        algorithmFPType* reluRes[dnnResourceNumber] = {0};

        reluRes[dnnResourceSrc] = inputMklTensor->getDnnArray();
        reluRes[dnnResourceDst] = resultMklTensor->getDnnArray();

        err = dnn::xExecute(reluPrim, (void**)reluRes); ON_ERR(err);
    }
    else
    {
        __DAAL_MAKE_TENSOR_THREADSAFE(resultTensor)

        computeImpl<cpu>(inputTensor, this->_errors.get(), [=](size_t fDimN, size_t *fDims, size_t nRowsToProcess, const TensorOffsetLayout &layout)
        {
            ReadSubtensor<algorithmFPType, cpu, Tensor> inputBlock(*inputTensor, fDimN, fDims, 0, nRowsToProcess, layout);
            const algorithmFPType *inputArray = inputBlock.get();

            WriteSubtensor<algorithmFPType, cpu, Tensor> resultBlock(*resultTensor, fDimN, fDims, 0, nRowsToProcess, layout);
            algorithmFPType *resultArray = resultBlock.get();

            algorithmFPType zero = (algorithmFPType)0;
            size_t nDataElements = inputBlock.getSize();
            for(size_t i = 0; i < nDataElements; i++)
            {
                if(inputArray[i] > zero)
                {
                    resultArray[i] = inputArray[i];
                }
                else
                {
                    resultArray[i] = zero;
                }
            }
        });
    }
    DAAL_RETURN_STATUS()
}

} // namespace internal
} // namespace forward
} // namespace relu
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal

#endif
