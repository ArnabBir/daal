/* file: kmeans_result_types.cpp */
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
//  Implementation of kmeans classes.
//--
*/

#include "algorithms/kmeans/kmeans_types.h"
#include "daal_defines.h"
#include "serialization_utils.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace kmeans
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_KMEANS_RESULT_ID);
Result::Result() : daal::algorithms::Result(4) {}

/**
 * Returns the result of the K-Means algorithm
 * \param[in] id   Result identifier
 * \return         Result that corresponds to the given identifier
 */
NumericTablePtr Result::get(ResultId id) const
{
    return staticPointerCast<NumericTable, SerializationIface>(Argument::get(id));
}

/**
 * Sets the result of the K-Means algorithm
 * \param[in] id    Identifier of the result
 * \param[in] ptr   Pointer to the object
 */
void Result::set(ResultId id, const NumericTablePtr &ptr)
{
    Argument::set(id, ptr);
}

/**
* Checks the result of the K-Means algorithm
* \param[in] input   %Input objects for the algorithm
* \param[in] par     Algorithm parameter
* \param[in] method  Computation method
*/
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, int method) const
{
    Input *algInput = dynamic_cast<Input *>(const_cast<daal::algorithms::Input *>(input));
    size_t inputFeatures   = algInput->getNumberOfFeatures();

    const Parameter *kmPar = static_cast<const Parameter *>(par);
    int unexpectedLayouts = (int)packed_mask;
    services::Status s;
    s |= checkNumericTable(get(centroids).get(),    centroidsStr(), unexpectedLayouts, 0, inputFeatures, kmPar->nClusters);
    if(!s) return s;
    s |= checkNumericTable(get(objectiveFunction).get(), goalFunctionStr(), unexpectedLayouts, 0, 1, 1);
    if(!s) return s;
    s |= checkNumericTable(get(nIterations).get(),  nIterationsStr(), unexpectedLayouts, 0, 1, 1);
    if(!s) return s;

    if(kmPar->assignFlag)
    {
        NumericTablePtr assignmentsTable = get(assignments);
        size_t inputRows = algInput->get(data)->getNumberOfRows();
        s |= checkNumericTable(get(assignments).get(),assignmentsStr(), unexpectedLayouts, 0, 1, inputRows);
    }
    return s;
}

/**
 * Checks the results of the K-Means algorithm
 * \param[in] pres    Partial results of the algorithm
 * \param[in] par     Algorithm parameter
 * \param[in] method  Computation method
 */
services::Status Result::check(const daal::algorithms::PartialResult *pres, const daal::algorithms::Parameter *par, int method) const
{
    const Parameter *kmPar = static_cast<const Parameter *>(par);
    int unexpectedLayouts = (int)packed_mask;
    PartialResult *algPres = static_cast<PartialResult *>(const_cast<daal::algorithms::PartialResult *>(pres));
    size_t presFeatures = algPres->get(partialSums)->getNumberOfColumns();
    services::Status s;
    s |= checkNumericTable(get(centroids).get(), centroidsStr(), unexpectedLayouts, 0, presFeatures, kmPar->nClusters);
    if(!s) return s;
    s |= checkNumericTable(get(objectiveFunction).get(), goalFunctionStr(), unexpectedLayouts, 0, 1, 1);
    return s;
}

} // namespace interface1
} // namespace kmeans
} // namespace algorithm
} // namespace daal
