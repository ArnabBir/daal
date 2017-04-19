/* file: svm_train_batch_container.h */
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
//  Implementation of SVM training algorithm container.
//--
*/

#include "svm_train.h"
#include "svm_train_kernel.h"
#include "svm_train_boser_kernel.h"
#include "classifier_training_types.h"

namespace daal
{
namespace algorithms
{
namespace svm
{
namespace training
{
/**
*  \brief Initialize list of SVM kernels with implementations for supported architectures
*/
template <typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::BatchContainer(daal::services::Environment::env *daalEnv)
{
    __DAAL_INITIALIZE_KERNELS(internal::SVMTrainImpl, method, algorithmFPType);
}

template <typename algorithmFPType, Method method, CpuType cpu>
BatchContainer<algorithmFPType, method, cpu>::~BatchContainer()
{
    __DAAL_DEINITIALIZE_KERNELS();
}

template <typename algorithmFPType, Method method, CpuType cpu>
services::Status BatchContainer<algorithmFPType, method, cpu>::compute()
{
    classifier::training::Input *input = static_cast<classifier::training::Input *>(_in);
    svm::training::Result *result = static_cast<svm::training::Result *>(_res);
    size_t na = input->size();

    NumericTablePtr x = input->get(classifier::training::data);
    NumericTable *y = input->get(classifier::training::labels).get();

    daal::algorithms::Model *r = static_cast<daal::algorithms::Model *>(result->get(classifier::training::model).get());

    daal::algorithms::Parameter *par = _par;
    daal::services::Environment::env &env = *_env;
    __DAAL_CALL_KERNEL(env, internal::SVMTrainImpl, __DAAL_KERNEL_ARGUMENTS(method, algorithmFPType), compute, x, y, r, par);
}

} // namespace training
} // namespace svm
} // namespace algorithms
} // namespace daal
