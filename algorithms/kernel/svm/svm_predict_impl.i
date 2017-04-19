/* file: svm_predict_impl.i */
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
//  SVM prediction algorithm implementation
//--
*/

#ifndef __SVM_PREDICT_IMPL_I__
#define __SVM_PREDICT_IMPL_I__

#include "service_memory.h"
#include "service_micro_table.h"
#include "service_numeric_table.h"

using namespace daal::internal;
using namespace daal::services::internal;

namespace daal
{
namespace algorithms
{
namespace svm
{
namespace prediction
{
namespace internal
{

template <typename algorithmFPType, CpuType cpu>
struct SVMPredictImpl<defaultDense, algorithmFPType, cpu> : public Kernel
{
    services::Status compute(const NumericTablePtr a, const daal::algorithms::Model *m, NumericTablePtr r,
                             const daal::algorithms::Parameter *par)
    {
        algorithmFPType zero = 0.0;
        NumericTablePtr xTable = a;
        FeatureMicroTable<algorithmFPType, writeOnly, cpu> mtR(r.get());
        size_t nVectors = xTable->getNumberOfRows();

        Model *model = static_cast<Model *>(const_cast<daal::algorithms::Model *>(m));
        Parameter *parameter = static_cast<Parameter *>(const_cast<daal::algorithms::Parameter *>(par));

        services::SharedPtr<kernel_function::KernelIface> kernel = parameter->kernel->clone();
        kernel->getErrors()->setCanThrow(false);

        NumericTablePtr svTable       = model->getSupportVectors();
        NumericTablePtr svCoeffTable  = model->getClassificationCoefficients();
        algorithmFPType bias = (algorithmFPType)model->getBias();

        FeatureMicroTable<algorithmFPType, readOnly, cpu> mtSVCoeff(svCoeffTable.get());

        size_t nSV = mtSVCoeff.getFullNumberOfRows();
        algorithmFPType *svCoeff;

        algorithmFPType *distance;
        mtR.getBlockOfColumnValues(0, 0, nVectors, &distance);

        if (nSV == 0)
        {
            for (size_t i = 0; i < nVectors; i++)
            {
                distance[i] = zero;
            }
        }
        else
        {
            mtSVCoeff.getBlockOfColumnValues(0, 0, nSV, &svCoeff);

            algorithmFPType *buf = (algorithmFPType *)daal::services::daal_malloc(nSV * nVectors * sizeof(algorithmFPType));
            if (buf == NULL) { return services::Status(services::ErrorMemoryAllocationFailed); }

            NumericTablePtr shResNT(new HomogenNumericTableCPU<algorithmFPType, cpu>(buf, nSV, nVectors));

            services::SharedPtr<kernel_function::Result> shRes(new kernel_function::Result());
            shRes->set(kernel_function::values, shResNT);

            kernel->setResult(shRes);
            kernel->inputBase->set(kernel_function::X, xTable);
            kernel->inputBase->set(kernel_function::Y, svTable);
            kernel->parameterBase->computationMode = kernel_function::matrixMatrix;
            services::Status s = kernel->computeNoThrow();
            if(!s)
            {
                mtSVCoeff.release();
                daal::services::daal_free(buf);
                mtR.release();
                this->_errors->add(services::ErrorSVMinnerKernel);
                this->_errors->add(kernel->getErrors()->getErrors());
                DAAL_RETURN_STATUS();
            }
            for (size_t i = 0; i < nVectors; i++)
            {
                distance[i] = bias;
                for (size_t j = 0; j < nSV; j++)
                {
                    distance[i] += buf[i * nSV + j] * svCoeff[j];
                }
            }

            mtSVCoeff.release();
            daal::services::daal_free(buf);
        }
        mtR.release();
        DAAL_RETURN_STATUS();
    }
};

} // namespace internal

} // namespace prediction

} // namespace svm

} // namespace algorithms

} // namespace daal

#endif
