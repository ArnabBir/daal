/* file: naivebayes_train_result_fpt.cpp */
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
//  Implementation of multinomial naive bayes algorithm and types methods.
//--
*/

#include "multinomial_naive_bayes_training_types.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace multinomial_naive_bayes
{
namespace training
{
namespace interface1
{
/**
 * Allocates memory for storing final result computed with naive Bayes training algorithm
 * \param[in] input      Pointer to input object
 * \param[in] parameter  Pointer to parameter
 * \param[in] method     Computation method
 */
template <typename algorithmFPType>
DAAL_EXPORT void Result::allocate(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method)
{
    const classifier::training::InputIface *algInput = static_cast<const classifier::training::InputIface *>(input);
    Parameter *algPar = static_cast<Parameter *>(const_cast<daal::algorithms::Parameter *>(parameter));
    size_t nFeatures = algInput->getNumberOfFeatures();
    algorithmFPType dummy = 0;
    set(classifier::training::model, services::SharedPtr<classifier::Model>(new Model(nFeatures, *algPar, dummy)));
}

/**
* Allocates memory for storing final result computed with naive Bayes training algorithm
* \param[in] partialResult      Pointer to partial result structure
* \param[in] parameter          Pointer to parameter structure
* \param[in] method             Computation method
*/
template <typename algorithmFPType>
DAAL_EXPORT void Result::allocate(const daal::algorithms::PartialResult *partialResult, const daal::algorithms::Parameter *parameter, const int method)
{
    const PartialResult *pres = static_cast<const PartialResult *>(partialResult);
    Parameter *algPar = static_cast<Parameter *>(const_cast<daal::algorithms::Parameter *>(parameter));
    size_t nFeatures = pres->getNumberOfFeatures();
    algorithmFPType dummy = 0;
    set(classifier::training::model, services::SharedPtr<classifier::Model>(new Model(nFeatures, *algPar, dummy)));
}

template DAAL_EXPORT void Result::allocate<DAAL_FPTYPE>(const daal::algorithms::Input *input, const daal::algorithms::Parameter *parameter, const int method);
template DAAL_EXPORT void Result::allocate<DAAL_FPTYPE>(const daal::algorithms::PartialResult *partialResult, const daal::algorithms::Parameter *parameter, const int method);

}// namespace interface1
}// namespace training
}// namespace multinomial_naive_bayes
}// namespace algorithms
}// namespace daal
