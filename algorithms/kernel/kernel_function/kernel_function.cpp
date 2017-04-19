/* file: kernel_function.cpp */
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
//  Implementation of kernel function algorithm and types methods.
//--
*/

#include "kernel_function_types.h"
#include "serialization_utils.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace kernel_function
{
namespace interface1
{
__DAAL_REGISTER_SERIALIZATION_CLASS(Result, SERIALIZATION_KERNEL_FUNCTION_RESULT_ID);

ParameterBase::ParameterBase(size_t rowIndexX, size_t rowIndexY, size_t rowIndexResult, ComputationMode computationMode) :
    rowIndexX(rowIndexX), rowIndexY(rowIndexY), rowIndexResult(rowIndexResult), computationMode(computationMode) {}

Input::Input() : daal::algorithms::Input(2) {}

/**
* Returns the input object of the kernel function algorithm
* \param[in] id    Identifier of the input object
* \return          %Input object that corresponds to the given identifier
*/
data_management::NumericTablePtr Input::get(InputId id) const
{
    return services::staticPointerCast<data_management::NumericTable, data_management::SerializationIface>(Argument::get(id));
}

/**
* Sets the input object of the kernel function algorithm
* \param[in] id    Identifier of the input object
* \param[in] ptr   Pointer to the input object
*/
void Input::set(InputId id, const data_management::NumericTablePtr &ptr)
{
    Argument::set(id, ptr);
}

services::Status Input::checkCSR() const
{
    services::Status s;
    int csrLayout = (int)data_management::NumericTableIface::csrArray;

    s |= data_management::checkNumericTable(get(X).get(), XStr(), 0, csrLayout);
    if(!s) return s;

    size_t nFeaturesX = get(X)->getNumberOfColumns();
    s |= data_management::checkNumericTable(get(Y).get(), YStr(), 0, csrLayout, nFeaturesX);
    return s;
}

services::Status Input::checkDense() const
{
    services::Status s;
    s |= data_management::checkNumericTable(get(X).get(), XStr());
    if(!s) return s;

    size_t nFeaturesX = get(X)->getNumberOfColumns();
    s |= data_management::checkNumericTable(get(Y).get(), YStr(), 0, 0, nFeaturesX);
    return s;
}
/**
 * Returns the result of the kernel function algorithm
 * \param[in] id   Identifier of the result
 * \return         Final result that corresponds to the given identifier
 */
data_management::NumericTablePtr Result::get(ResultId id) const
{
    return services::staticPointerCast<data_management::NumericTable, data_management::SerializationIface>(Argument::get(id));
}

/**
 * Sets the result of the kernel function algorithm
 * \param[in] id    Identifier of the partial result
 * \param[in] ptr   Pointer to the object
 */
void Result::set(ResultId id, const data_management::NumericTablePtr &ptr)
{
    Argument::set(id, ptr);
}

/**
* Checks the result of the kernel function algorithm
* \param[in] input   %Input objects of the algorithm
* \param[in] par     %Parameter of the algorithm
* \param[in] method  Computation method of the algorithm
*/
services::Status Result::check(const daal::algorithms::Input *input, const daal::algorithms::Parameter *par, int method) const
{
    services::Status s;
    Input *algInput = static_cast<Input *>(const_cast<daal::algorithms::Input *>(input));
    ParameterBase *algParameter = static_cast<ParameterBase *>(const_cast<daal::algorithms::Parameter *>(par));

    size_t nRowsX = algInput->get(X)->getNumberOfRows();
    size_t nRowsY = algInput->get(Y)->getNumberOfRows();

    int unexpectedLayouts = data_management::packed_mask;
    s |= data_management::checkNumericTable(get(values).get(), valuesStr(), unexpectedLayouts, 0, 0, nRowsX);
    if(!s) return s;

    size_t nVectorsValues = get(values)->getNumberOfRows();

    if(algParameter->rowIndexResult >= nVectorsValues)
    {
        return Status(Error::create(services::ErrorIncorrectParameter, services::ParameterName, rowIndexResultStr()));
    }
    if(algParameter->rowIndexX >= nRowsX)
    {
        return Status(Error::create(services::ErrorIncorrectParameter, services::ParameterName, rowIndexXStr()));
    }
    if(algParameter->rowIndexY >= nRowsY)
    {
        return Status(Error::create(services::ErrorIncorrectParameter, services::ParameterName, rowIndexYStr()));
    }
    return s;
}

}// namespace interface1
}// namespace kernel_function
}// namespace algorithms
}// namespace daal
