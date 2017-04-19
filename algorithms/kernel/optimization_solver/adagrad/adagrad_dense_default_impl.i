/* file: adagrad_dense_default_impl.i */
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
//  Implementation of adagrad algorithm
//--
*/

#ifndef __ADAGRAD_DENSE_DEFAULT_IMPL_I__
#define __ADAGRAD_DENSE_DEFAULT_IMPL_I__

#include "service_micro_table.h"
#include "service_numeric_table.h"
#include "service_math.h"
#include "service_utils.h"
#include "service_numeric_table.h"
#include "iterative_solver_kernel.h"
#include "algorithms/optimization_solver/iterative_solver/iterative_solver_types.h"
#include "algorithms/optimization_solver/adagrad/adagrad_types.h"

using namespace daal::internal;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace optimization_solver
{
namespace adagrad
{
namespace internal
{

template<typename algorithmFPType, Method method, CpuType cpu>
void initAccumulatedGrad(algorithmFPType *accumulatedG,
    size_t nRows, NumericTable* pOptInput, algorithmFPType degenerateCasesThreshold)
{
    if(pOptInput)
    {
        /* Initialize accumulatedG and invAccumGrad from optional input data */
        ReadRows<algorithmFPType, cpu> optInputBD(*pOptInput, 0, nRows);
        daal_memcpy_s(accumulatedG, nRows * sizeof(algorithmFPType), optInputBD.get(), nRows * sizeof(algorithmFPType));
    }
    else
    {
        for(size_t i = 0; i < nRows; i++)
        {
            accumulatedG[i] = algorithmFPType(0.0);
        }
    }
}

/**
 *  \brief Kernel for Adagrad calculation
 */
template<typename algorithmFPType, Method method, CpuType cpu>
services::Status AdagradKernel<algorithmFPType, method, cpu>::compute(NumericTable *inputArgument, NumericTable *minimum, NumericTable *nIterations,
                                                          NumericTable *gradientSquareSumResult, NumericTable *gradientSquareSumInput,
                                                          OptionalArgument *optionalArgument, OptionalArgument *optionalResult, Parameter *parameter)
{
    const size_t nRows = inputArgument->getNumberOfRows();

    WriteRows<algorithmFPType, cpu> workValueBD(*minimum, 0, nRows);
    algorithmFPType *workValue = workValueBD.get();

    //init workValue
    {
        ReadRows<algorithmFPType, cpu> startValueBD(*inputArgument, 0, nRows);
        const algorithmFPType *startValueArray = startValueBD.get();
        daal_memcpy_s(workValue, nRows * sizeof(algorithmFPType), startValueArray, nRows * sizeof(algorithmFPType));
    }

    const size_t nIter = parameter->nIterations;
    WriteRows<int, cpu> nIterationsBD(*nIterations, 0, 1);
    int *nProceededIterations = nIterationsBD.get();
    /* if nIter == 0, set result as start point, the number of executed iters to 0 */
    if(nIter == 0)
    {
        nProceededIterations[0] = 0;
        DAAL_RETURN_STATUS()
    }

    NumericTable *lastIterationInput = (optionalArgument) ? NumericTable::cast(optionalArgument->get(iterative_solver::lastIteration)).get() : nullptr;
    NumericTable *lastIterationResult = (optionalResult) ? NumericTable::cast(optionalResult->get(iterative_solver::lastIteration)).get() : nullptr;

    sum_of_functions::BatchPtr function = parameter->function;

    /*Get random indices for SGD from parameter or from rng generator*/
    const bool isPredefinedBatchIndices = parameter->batchIndices;
    const size_t batchSize = parameter->batchSize;
    const algorithmFPType degenerateCasesThreshold = (algorithmFPType)parameter->degenerateCasesThreshold;
    ReadRows<int, cpu> predefinedBatchIndicesBD(parameter->batchIndices.get(), 0, nIter);
    iterative_solver::internal::RngTask<int, cpu> rngTask(predefinedBatchIndicesBD.get(), batchSize);
    if((!parameter->batchIndices) && !rngTask.init(optionalArgument, parameter->function->sumOfFunctionsParameter->numberOfTerms, parameter->seed, adagrad::rngState))
    {
        this->_errors->add(ErrorMemoryAllocationFailed);
        DAAL_RETURN_STATUS()
    }

    SharedPtr<HomogenNumericTableCPU<int, cpu>> ntBatchIndices(new HomogenNumericTableCPU<int, cpu>(NULL, batchSize, 1));
    function->sumOfFunctionsParameter->batchIndices = ntBatchIndices;
    function->sumOfFunctionsInput->set(sum_of_functions::argument, NumericTablePtr(new HomogenNumericTableCPU<algorithmFPType, cpu>(workValue, 1, nRows)));

    NumericTablePtr ntlearningRate = parameter->learningRate;
    ReadRows<algorithmFPType, cpu> learningRateBD(*ntlearningRate, 0, 1);
    const algorithmFPType learningRate = *learningRateBD.get();

    *nProceededIterations = (int)nIter;

    SmartPtr<cpu> smAccumulatedG(nRows * sizeof(algorithmFPType));
    algorithmFPType *accumulatedG = (algorithmFPType *)smAccumulatedG.get();

    initAccumulatedGrad<algorithmFPType, method, cpu>(accumulatedG, nRows, gradientSquareSumInput, degenerateCasesThreshold);

    size_t epoch = 0, startIteration = 0, nProceededIters = 0;
    if(lastIterationInput != nullptr)
    {
        ReadRows<int, cpu, NumericTable> lastIterationInputBD(lastIterationInput, 0, 1);
        const int *lastIterationInputArray = lastIterationInputBD.get();
        startIteration = lastIterationInputArray[0];
    }
    for(epoch = startIteration; epoch < (startIteration + nIter); epoch++)
    {
        ntBatchIndices->setArray(const_cast<int*>(rngTask.get(*this->_errors)),ntBatchIndices->getNumberOfRows());
        services::Status s = function->computeNoThrow();
        if(!s)
        {
            this->_errors->add(function->getErrors()->getErrors());
            *nProceededIterations = (int)(epoch - startIteration);
            break;
        }

        auto ntGradient = function->getResult()->get(objective_function::gradientIdx);
        ReadRows<algorithmFPType, cpu> ntGradientBD(*ntGradient, 0, nRows);
        const algorithmFPType *gradient = ntGradientBD.get();
        if(nIter > 1)
        {
            const algorithmFPType pointNorm = vectorNorm(workValue, nRows);
            const algorithmFPType gradientNorm = vectorNorm(gradient, nRows);
            const algorithmFPType gradientThreshold = parameter->accuracyThreshold * daal::internal::Math<algorithmFPType, cpu>::sMax(algorithmFPType(1.0), pointNorm);
            if(gradientNorm <= gradientThreshold)
            {
                *nProceededIterations = (int)(epoch - startIteration);
                if(parameter->optionalResultRequired)
                {
                    PRAGMA_IVDEP
                    PRAGMA_VECTOR_ALWAYS
                    for(size_t j = 0; j < nRows; j++)
                    {
                        accumulatedG[j] += gradient[j] * gradient[j];
                    }
                }
                break;
            }
        }

        PRAGMA_IVDEP
        PRAGMA_VECTOR_ALWAYS
        for(size_t j = 0; j < nRows; j++)
        {
            accumulatedG[j] += gradient[j] * gradient[j];
            const algorithmFPType invAccumGrad = algorithmFPType(1.0) / daal::internal::Math<algorithmFPType,cpu>::sSqrt(accumulatedG[j] + degenerateCasesThreshold);
            workValue[j] = workValue[j] - learningRate * gradient[j] * invAccumGrad;
        }
        nProceededIters++;
    }
    if(!parameter->optionalResultRequired)
        DAAL_RETURN_STATUS()
    NumericTable *pOptResult = gradientSquareSumResult;
    /* Copy accumulatedG to output */
    WriteRows<algorithmFPType, cpu> optResultBD(*pOptResult, 0, nRows);
    daal_memcpy_s(optResultBD.get(), nRows * sizeof(algorithmFPType), accumulatedG, nRows * sizeof(algorithmFPType));
    if(lastIterationResult)
    {
        WriteRows<int, cpu, NumericTable> lastIterationResultBD(lastIterationResult, 0, 1);
        int *lastIterationResultArray = lastIterationResultBD.get();
        lastIterationResultArray[0] = startIteration + nProceededIters;
    }
    if(!rngTask.save(optionalResult, adagrad::rngState, *this->_errors))
    {
        this->_errors->add(ErrorMemoryAllocationFailed);
        DAAL_RETURN_STATUS()
    }
    DAAL_RETURN_STATUS()
}

} // namespace daal::internal

} // namespace adagrad

} // namespace optimization_solver

} // namespace algorithms

} // namespace daal

#endif
