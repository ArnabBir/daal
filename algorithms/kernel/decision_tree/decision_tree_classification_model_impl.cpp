/* file: decision_tree_classification_model_impl.cpp */
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
//  Implementation of the class defining the Decision tree model
//--
*/

#include "decision_tree_classification_model_impl.h"
#include "serialization_utils.h"

namespace daal
{
namespace algorithms
{
namespace decision_tree
{
namespace classification
{
namespace interface1
{

using namespace daal::data_management;
using namespace daal::services;

__DAAL_REGISTER_SERIALIZATION_CLASS(Model, SERIALIZATION_DECISION_TREE_CLASSIFICATION_MODEL_ID);

Model::Model() : daal::algorithms::classifier::Model(), _impl(new ModelImpl) {}

Model::~Model() {}

void Model::serializeImpl(data_management::InputDataArchive  * arch)
{
    daal::algorithms::Model::serialImpl<data_management::InputDataArchive, false>(arch);
    _impl->serialImpl<data_management::InputDataArchive, false>(arch);
}

void Model::deserializeImpl(data_management::OutputDataArchive * arch)
{
    daal::algorithms::Model::serialImpl<data_management::OutputDataArchive, true>(arch);
    _impl->serialImpl<data_management::OutputDataArchive, true>(arch);
}

} // namespace interface1
} // namespace classification
} // namespace decision_tree
} // namespace algorithms
} // namespace daal
