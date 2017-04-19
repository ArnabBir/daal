/* file: df_training_parameter.cpp */
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
//  Implementation of decision forest training parameter class
//--
*/

#include "algorithms/decision_forest/decision_forest_training_parameter.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace decision_forest
{
namespace training
{
using namespace daal::services;
Status checkImpl(const decision_forest::training::Parameter& prm)
{
    DAAL_CHECK_EX(prm.nTrees, ErrorIncorrectParameter, ParameterName, nTreesStr());
    DAAL_CHECK_EX(prm.minObservationsInLeafNode, ErrorIncorrectParameter, ParameterName, minObservationsInLeafNodeStr());
    DAAL_CHECK_EX((prm.observationsPerTreeFraction > 0) && (prm.observationsPerTreeFraction <= 1),
        ErrorIncorrectParameter, ParameterName, observationsPerTreeFractionStr());
    DAAL_CHECK_EX((prm.impurityThreshold >= 0), ErrorIncorrectParameter, ParameterName, impurityThresholdStr());
    return Status();
}

} // namespace training
} // namespace decision_forest
} // namespace algorithms
} // namespace daal
