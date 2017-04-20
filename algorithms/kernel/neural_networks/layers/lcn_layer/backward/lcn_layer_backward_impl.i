/* file: lcn_layer_backward_impl.i */
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
//  Implementation of local contrast normalization algorithm
//--
*/

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace lcn
{
namespace backward
{
namespace internal
{
/* TLS structure with local arrays and variables */
template<typename algorithmFPType, Method method, CpuType cpu>
struct Tls_data
{
    services::ErrorCollection errors;

    TensorPtr wDerTensor; /* only needed by conv compute, not lcn */
    TensorPtr bDerTensor; /* only needed by conv compute, not lcn */

    SharedPtr<HomogenTensor<algorithmFPType> > convResultTensor;
    SharedPtr<HomogenTensor<algorithmFPType> > convInGradTensor;

    /* Create backward convolution algorithm */
    Convolution2dKernel<algorithmFPType, neural_networks::layers::convolution2d::defaultDense, cpu> dconvKernel;

    Tls_data(size_t dataOffsetAfterDim, size_t firstKernelDim,
             size_t secondKrnelDim, size_t firstDim, size_t secondDim)
    {
        Collection<size_t> wDims;
        wDims << 1 << 1 << firstKernelDim << secondKrnelDim;

        Collection<size_t> bDims;
        bDims << 1;

        Collection<size_t> convInpGradDims;
        convInpGradDims << 1 << 1 << firstDim << secondDim;

        wDerTensor       = SharedPtr<HomogenTensor<algorithmFPType> >(new HomogenTensor<algorithmFPType>(wDims, TensorIface::doAllocate));
        bDerTensor       = SharedPtr<HomogenTensor<algorithmFPType> >(new HomogenTensor<algorithmFPType>(bDims, TensorIface::doAllocate));
        convResultTensor = SharedPtr<HomogenTensor<algorithmFPType> >(new HomogenTensor<algorithmFPType>(convInpGradDims, TensorIface::doAllocate));
        convInGradTensor = SharedPtr<HomogenTensor<algorithmFPType> >(new HomogenTensor<algorithmFPType>(convInpGradDims, TensorIface::doAllocate));

        checkTensor(wDerTensor.get(), &errors, "wDerTensor" );
        checkTensor(bDerTensor.get(), &errors, "bDerTensor" );
        checkTensor(convResultTensor.get(), &errors, "convResultTensor" );
        checkTensor(convInGradTensor.get(), &errors, "convInGradTensor" );

        dconvKernel.initialize();
    }

    ~Tls_data()
    {
        dconvKernel.reset();
    }
};

template<typename algorithmFPType, Method method, CpuType cpu>
void LCNKernel<algorithmFPType, method, cpu>::initialize(Tensor *auxCenteredDataTensor, Tensor *auxSigmaTensor, Tensor *auxCTensor,
                                                         Tensor *kernelTensor, const lcn::Parameter *parameter)
{
    const Collection<size_t> &initialDataDims = auxCenteredDataTensor->getDimensions();
    const Collection<size_t> &cDims           = auxCTensor->getDimensions();
    sigmaDims  = auxSigmaTensor->getDimensions();
    kernelDims = kernelTensor->getDimensions();

    nDataRows   = initialDataDims[0];
    nSigmaRows  = sigmaDims[0];
    nCRows      = cDims[0];
    nKernelRows = kernelDims[0];

    nDataElements   = auxCenteredDataTensor->getSize();
    nKernelElements = kernelTensor->getSize();
    nCElements      = auxCTensor->getSize();
    nDims = initialDataDims.size();

    initialFirstDim  = parameter->indices.dims[0];
    initialSecondDim = parameter->indices.dims[1];

    sigmaThreshold = parameter->sigmaDegenerateCasesThreshold;

    initialSumDimension = 1;
    if(parameter->sumDimension)
    {
        fDimN = 1;
        ReadRows<int, cpu, NumericTable> dimBlock(*parameter->sumDimension, 0, 1);
        const int *dimArray = dimBlock.get();
        initialSumDimension = (size_t)dimArray[0];
    }

    /* Get dims collection of repacked data tensor */
    batchDimension = 6 - initialSumDimension - initialFirstDim - initialSecondDim; /* Calculate 4th dimension index. 6 here is a sum of all indexes: 0 + 1 + 2 + 3 */
    dataDims << initialDataDims[batchDimension] << initialDataDims[initialSumDimension] << initialDataDims[initialFirstDim] << initialDataDims[initialSecondDim];

    sumDimension = (size_t)1;
    firstDim     = (size_t)2;
    secondDim    = (size_t)3;

    if(!parameter->sumDimension)
    {
        fDimN = 2;
        dataDims[0] *= dataDims[sumDimension];
        dataDims[sumDimension] = 1;
    }

    dataOffsetBeforeDim = dataDims[0];
    dataOffsetAfterDim  = dataDims[firstDim] * dataDims[secondDim];

    /* Set convolution algorithm parameters */
    convParameter.indices.dims[0] = firstDim;
    convParameter.indices.dims[1] = secondDim;
    convParameter.nGroups = 1;
    convParameter.strides.size[0] = 1;
    convParameter.strides.size[1] = 1;
    convParameter.groupDimension = 1;
    convParameter.nKernels = 1;
    convParameter.kernelSizes.size[0] = kernelDims[0];
    convParameter.kernelSizes.size[1] = kernelDims[1];
    convParameter.paddings.size[0] = kernelDims[0] / 2;
    convParameter.paddings.size[1] = kernelDims[1] / 2;
}

/*  step_1:   g_5   = inputGradient * auxInvMax;
    step_2:   g_13  = sum_sumDimension( inputGradient * auxCenteredData ) * pow(auxInvMax, 2);
    step_3:   g_12  = g_13 * (1 - q) = g_13 - g_10 = step_2 * (1 - q);
    step_4:   g_10  = step_2 * q;
    step_5:   g_8  = (g_10 + g_11) / auxSigma = (g_10 + 1/M * g_12) / auxSigma = (step_3 + 1/M * step_4) / ( auxSigma + e );
    step_6:   g_7   = dconv(g_8) = dconv(step_5);
    step_7:   g_4   = g_5 + g_6 = g_5 + g_7 * auxCenteredData = step_1 + step_6 * auxCenteredData;
    step_8:   g_3   = sum_sumDimension(g_4) = sum_sumDimension(step_7);
    step_9:   g_1   = dconv(g_3) = dconv(step_8);
    step_10:  gradient = g_2 - g_1 = g_4 - g_1 = step_7 - step_9.
*/
template<typename algorithmFPType, Method method, CpuType cpu>
void LCNKernel<algorithmFPType, method, cpu>::compute(Tensor *auxCenteredDataTensor, Tensor *auxSigmaTensor, Tensor *auxCTensor,
                                                      Tensor *auxInvMaxTensor, Tensor *kernelTensor, Tensor *inGradTensor,
                                                      Tensor *gradientTensor, const lcn::Parameter *parameter)
{
    const algorithmFPType one  = 1.0;
    const algorithmFPType zero = 0.0;

    Collection<size_t> dimsOrder;
    dimsOrder << batchDimension << initialSumDimension << initialFirstDim << initialSecondDim;

    TensorOffsetLayout cdLayout = auxCenteredDataTensor->createDefaultSubtensorLayout();
    cdLayout.shuffleDimensions(dimsOrder);

    TensorOffsetLayout inGradLayout = inGradTensor->createDefaultSubtensorLayout();
    inGradLayout.shuffleDimensions(dimsOrder);

    TensorOffsetLayout gradientLayout = gradientTensor->createDefaultSubtensorLayout();
    gradientLayout.shuffleDimensions(dimsOrder);

    ReadSubtensor<algorithmFPType, cpu, Tensor> cBlock(*auxCTensor, 0, 0, 0, nCRows);
    const algorithmFPType *auxCArray = cBlock.get();

    ReadSubtensor<algorithmFPType, cpu, Tensor> kernelBlock(*kernelTensor, 0, 0, 0, nKernelRows);
    const algorithmFPType *kernelArray = kernelBlock.get();

    algorithmFPType divider = one / dataOffsetAfterDim;

    /* Allocate arrays needed for computations */
    TArray<algorithmFPType, cpu> tempArrayOfCSizeBlock(nCElements);
    algorithmFPType *tempArrayOfCSize = tempArrayOfCSizeBlock.get();
    DAAL_CHECK(tempArrayOfCSize, ErrorMemoryAllocationFailed);

    TArray<algorithmFPType, cpu> weightsBlock(nKernelElements);
    algorithmFPType *weightsArray = weightsBlock.get();
    DAAL_CHECK(weightsArray, ErrorMemoryAllocationFailed);

    TArray<size_t, cpu> fDimsBlock(fDimN);
    size_t *fDims = fDimsBlock.get();
    DAAL_CHECK(fDims, ErrorMemoryAllocationFailed);

    /* Compute multiplier to normalize through sumDimension */
    algorithmFPType multiplier = one / dataDims[sumDimension];
    /* Get weightsArray needed for convolution computation */
    for(size_t j = 0; j < nKernelElements; j++)
    {
        weightsArray[j] = kernelArray[j] * multiplier;
    }

    Collection<size_t> wDims;
    wDims << 1 << 1 << kernelDims[0] << kernelDims[1];

    /* Tensors needed for convolution */
    TensorPtr weightsTensor = TensorPtr(new HomogenTensor<algorithmFPType>(wDims, weightsArray));
    DAAL_CHECK(weightsTensor != 0, ErrorMemoryAllocationFailed);

    /* TLS data initialization */
    daal::tls<Tls_data<algorithmFPType, method, cpu> *> tls_data([ & ]()
    {
        return new Tls_data<algorithmFPType, method, cpu>(dataOffsetAfterDim, kernelDims[0], kernelDims[1], dataDims[firstDim], dataDims[secondDim]);
    });

    daal::threader_for(dataOffsetBeforeDim, dataOffsetBeforeDim, [ & ](int i)
    {
        Tls_data<algorithmFPType, method, cpu> *tls_data_local = tls_data.local();
        if(tls_data_local->errors.size() != 0) return;

        algorithmFPType *gSqTempArray    = tls_data_local->convInGradTensor->getArray();
        algorithmFPType *convResultArray = tls_data_local->convResultTensor->getArray();

        algorithmFPType gConvTempValue;
        size_t dataIndex, sigmaIndex;

        getFixedDimsIndexes(fDims, i);

        ReadSubtensor<algorithmFPType, cpu, Tensor> inGradBlock(*inGradTensor, fDimN, fDims, 0, dataDims[fDimN], inGradLayout);
        const algorithmFPType *inGradArray = inGradBlock.get();

        ReadSubtensor<algorithmFPType, cpu, Tensor> cdBlock(*auxCenteredDataTensor, fDimN, fDims, 0, dataDims[fDimN], cdLayout);
        const algorithmFPType *auxCDArray = cdBlock.get();

        WriteSubtensor<algorithmFPType, cpu, Tensor> gradientBlock(*gradientTensor, fDimN, fDims, 0, dataDims[fDimN], gradientLayout);
        algorithmFPType *gradientArray = gradientBlock.get();

        ReadSubtensor<algorithmFPType, cpu, Tensor> sigmaBlock(*auxSigmaTensor, fDimN, fDims, 0, sigmaDims[fDimN]);
        const algorithmFPType *auxSigmaArray = sigmaBlock.get();

        ReadSubtensor<algorithmFPType, cpu, Tensor> invMaxBlock(*auxInvMaxTensor, fDimN, fDims, 0, sigmaDims[fDimN]);
        const algorithmFPType *auxInvMaxArray = invMaxBlock.get();

        tempArrayOfCSize[i] = zero;
        for(size_t k = 0; k < dataOffsetAfterDim; k++)
        {
            gSqTempArray[k] = zero;
        }

        for(size_t j = 0; j < dataDims[sumDimension]; j++)
        {
            size_t j_shift = j * dataOffsetAfterDim;
            for(size_t k = 0; k < dataOffsetAfterDim; k++)
            {
                dataIndex  = k + j_shift;
                /* step_1:   g_5 = inputGradient * auxInvMax */
                gradientArray[dataIndex] = inGradArray[dataIndex] * auxInvMaxArray[k];

                /* step_2:   g_13  = sum_sumDimension( inputGradient * auxCenteredData ) * pow(auxInvMax, 2) */
                gSqTempArray[k] -= inGradArray[dataIndex] * auxCDArray[dataIndex] * auxInvMaxArray[k] * auxInvMaxArray[k];
            }
        }
        invMaxBlock.release();
        inGradBlock.release();

        for(size_t k = 0; k < dataOffsetAfterDim; k++)
        {
            /* step_3:   g_12  = g_13 * (1 - q) = g_13 - g_10 = step_2 * (1 - q) */
            tempArrayOfCSize[i] += gSqTempArray[k] * (one - (auxSigmaArray[k] > auxCArray[i]) );
        }

        for(size_t k = 0; k < dataOffsetAfterDim; k++)
        {
            /* step_4:   g_10  = step_2 * q  */
            gConvTempValue = gSqTempArray[k] * (auxSigmaArray[k] > auxCArray[i]);

            /* step_5:  g_8  = (g_10 + g_11) / auxSigma = (g_10 + 1/M * g_12) / auxSigma = (step_3 + 1/M * step_4) / ( auxSigma + e ) */
            gSqTempArray[k] = (gConvTempValue + divider * tempArrayOfCSize[i]) / (auxSigmaArray[k] + sigmaThreshold);
        }

        sigmaBlock.release();

        /* step_6:  g_7  = dconv(g_8) = dconv(step_5) */
        /* convResultTensor first time is used here as auxData for wDer and bDer not needed calculation, second time as conv result, needed for lcn */
        tls_data_local->dconvKernel.compute(tls_data_local->convInGradTensor.get(), tls_data_local->convResultTensor.get(), weightsTensor.get(), &convParameter,
                            tls_data_local->wDerTensor.get(), tls_data_local->bDerTensor.get(), tls_data_local->convResultTensor.get());

        for(size_t k = 0; k < dataOffsetAfterDim; k++)
        {
            gSqTempArray[k] = zero;
        }

        for(size_t j = 0; j < dataDims[sumDimension]; j++)
        {
            size_t j_shift = j * dataOffsetAfterDim;
            for(size_t k = 0; k < dataOffsetAfterDim; k++)
            {
                dataIndex  = k + j_shift;
                /* step_7:  g_4   = g_5 + g_6 = g_5 + g_7 * auxCenteredData = step_1 + step_6 * auxCenteredData */
                gradientArray[dataIndex] += convResultArray[k] * auxCDArray[dataIndex];

                /* step_8:  g_3   = sum_sumDimension(g_4) = sum_sumDimension(step_7) */
                gSqTempArray[k] += gradientArray[dataIndex];
            }
        }

        cdBlock.release();

        /* step_9:  g_1   = dconv(g_3) = dconv(step_8) */
        tls_data_local->dconvKernel.compute(tls_data_local->convInGradTensor.get(), tls_data_local->convResultTensor.get(), weightsTensor.get(), &convParameter,
                            tls_data_local->wDerTensor.get(), tls_data_local->bDerTensor.get(), tls_data_local->convResultTensor.get());

        for(size_t j = 0; j < dataDims[sumDimension]; j++)
        {
            size_t j_shift = j * dataOffsetAfterDim;
            for(size_t k = 0; k < dataOffsetAfterDim; k++)
            {
                size_t dataIndex  = k + j_shift;
                /* step_10:  gradient = g_2 - g_1 = g_4 - g_1 = step_7 - step_9 */
                gradientArray[dataIndex] -= convResultArray[k];
            }
        }
    });

    tls_data.reduce( [ & ]( Tls_data<algorithmFPType, method, cpu>* tls_data_local )
    {
        if(tls_data_local->errors.size() != 0)
        {
            this->_errors->add(ErrorMemoryAllocationFailed);
        }

        delete tls_data_local;
    } );
}

template<typename algorithmFPType, Method method, CpuType cpu>
void LCNKernel<algorithmFPType, method, cpu>::getFixedDimsIndexes(size_t *fDims, size_t i)
{
    if(fDimN == 1)
    {
        fDims[0] = i;
    }
    else
    {
        size_t offsetAfter = sigmaDims[fDimN - 1];

        /* Get last fixed dim index as the remainder of the division */
        fDims[fDimN - 1] = i % sigmaDims[fDimN - 1];

        /* Count indexes starting from the penultimate element of the fDims[] array*/
        for(size_t j = fDimN - 1; j > 0; j--)
        {
            size_t totalOffset = offsetAfter * sigmaDims[j - 1];
            size_t nTimes = i / totalOffset;

            fDims[j - 1] = (i - totalOffset * nTimes) / offsetAfter;

            offsetAfter *= sigmaDims[j - 1];
        }
    }
}

template<typename algorithmFPType, Method method, CpuType cpu>
void LCNKernel<algorithmFPType, method, cpu>::reset()
{
    dataDims.clear();
    kernelDims.clear();
    sigmaDims.clear();
}

} // internal
} // backward
} // namespace lcn
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal
