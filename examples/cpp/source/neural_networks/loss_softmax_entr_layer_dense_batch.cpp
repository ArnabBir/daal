/* file: loss_softmax_entr_layer_dense_batch.cpp */
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
!  Content:
!    C++ example of forward and backward softmax cross-entropy layer usage
!
!******************************************************************************/

/**
 * <a name="DAAL-EXAMPLE-CPP-LOSS_SOFTMAX_CROSS_ENTROPY_LAYER_BATCH"></a>
 * \example loss_softmax_entr_layer_dense_batch.cpp
 */

#include "daal.h"
#include "service.h"

using namespace std;
using namespace daal;
using namespace daal::algorithms;
using namespace daal::algorithms::neural_networks::layers;
using namespace daal::data_management;
using namespace daal::services;

static const size_t nDim = 3;
static const size_t dims[] = {3, 2, 4};
static const size_t gtDims[] = {3, 1, 4};
static float gTArray[3][1][4] = {{{1, 0, 0, 1}},
                                                {{0, 0, 1, 1}},
                                                                {{1, 0, 0, 1}}};
static float dataArray[3][2][4] = {{{ 1,  2,  3,  4},
                                    { 5,  6,  7,  8}},
                                                    {{9, 10, 11, 12},
                                                    {13, 14, 15, 16}},
                                                                    {{17, 18, 19, 20},
                                                                    {21, 22, 23, 24}}};

int main()
{
    TensorPtr tensorData(new HomogenTensor<float>(nDim, dims, (float *)dataArray));
    TensorPtr groundTruth(new HomogenTensor<float>(nDim, gtDims, (float *)gTArray));

    printTensor(tensorData, "Forward softmax cross-entropy layer input data:");
    printTensor(groundTruth, "Forward softmax cross-entropy layerr input ground truth:");

    /* Create an algorithm to compute forward softmax cross-entropy layer results using default method */
    loss::softmax_cross::forward::Batch<> softmaxCrossEntropyLayerForward;

    /* Set input objects for the forward softmax cross-entropy layer */
    softmaxCrossEntropyLayerForward.input.set(forward::data, tensorData);
    softmaxCrossEntropyLayerForward.input.set(loss::forward::groundTruth, groundTruth);

    /* Compute forward softmax cross-entropy layer results */
    softmaxCrossEntropyLayerForward.compute();

    /* Print the results of the forward softmax cross-entropy layer */
    services::SharedPtr<loss::softmax_cross::forward::Result> forwardResult = softmaxCrossEntropyLayerForward.getResult();
    printTensor(forwardResult->get(forward::value), "Forward softmax cross-entropy layer result (first 5 rows):", 5);
    printTensor(forwardResult->get(loss::softmax_cross::auxProbabilities), "Softmax Cross-Entropy layer probabilities estimations (first 5 rows):", 5);
    printTensor(forwardResult->get(loss::softmax_cross::auxGroundTruth), "Softmax Cross-Entropy layer ground truth (first 5 rows):", 5);

    /* Create an algorithm to compute backward softmax cross-entropy layer results using default method */
    loss::softmax_cross::backward::Batch<> softmaxCrossEntropyLayerBackward;

    /* Set input objects for the backward softmax cross-entropy layer */
    softmaxCrossEntropyLayerBackward.input.set(backward::inputFromForward, forwardResult->get(forward::resultForBackward));

    /* Compute backward softmax cross-entropy layer results */
    softmaxCrossEntropyLayerBackward.compute();

    /* Print the results of the backward softmax cross-entropy layer */
    services::SharedPtr<backward::Result> backwardResult = softmaxCrossEntropyLayerBackward.getResult();
    printTensor(backwardResult->get(backward::gradient), "Backward softmax cross-entropy layer result (first 5 rows):", 5);

    return 0;
}
