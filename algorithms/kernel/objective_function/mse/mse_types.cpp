/* file: mse_types.cpp */
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
//  Implementation of mse classes.
//--
*/

#include "algorithms/optimization_solver/objective_function/mse_types.h"
#include "numeric_table.h"

using namespace daal::data_management;

namespace daal
{
namespace algorithms
{
namespace optimization_solver
{
namespace mse
{
namespace interface1
{
/**
 * Constructs the parameter of Mean squared error objective function
 * \param[in] numberOfTerms    The number of terms in the function
 * \param[in] batchIndices     Numeric table of size 1 x m where m is batch size that represent
                               a batch of indices used to compute the function results, e.g.,
                               value of the sum of the functions. If no indices are provided,
                               all terms will be used in the computations.
 * \param[in] resultsToCompute 64 bit integer flag that indicates the results to compute
 */
Parameter::Parameter(size_t numberOfTerms, data_management::NumericTablePtr batchIndices, const DAAL_UINT64 resultsToCompute) :
                     sum_of_functions::Parameter(numberOfTerms, batchIndices, resultsToCompute)
{}

/**
 * Constructs an Parameter by copying input objects and parameters of another Parameter
 * \param[in] other An object to be used as the source to initialize object
 */
Parameter::Parameter(const Parameter &other) :
    sum_of_functions::Parameter(other)
{}

/**
 * Checks the correctness of the parameter
 */
void Parameter::check() const
{
    sum_of_functions::Parameter::check();
}

/** Default constructor */
Input::Input() : sum_of_functions::Input(3)
{}

/**
 * Sets one input object for Mean squared error objective function
 * \param[in] id    Identifier of the input object
 * \param[in] ptr   Pointer to the object
 */
void Input::set(InputId id, const data_management::NumericTablePtr &ptr)
{
    Argument::set(id, ptr);
}

/**
 * Returns the input numeric table for Mean squared error objective function
 * \param[in] id    Identifier of the input numeric table
 * \return          %Input object that corresponds to the given identifier
 */
data_management::NumericTablePtr Input::get(InputId id) const
{
    return services::staticPointerCast<data_management::NumericTable, data_management::SerializationIface>(Argument::get(id));
}

/**
 * Checks the correctness of the input
 * \param[in] par       Pointer to the structure of the algorithm parameters
 * \param[in] method    Computation method
 */
void Input::check(const daal::algorithms::Parameter *par, int method) const
{
    sum_of_functions::Input::check(par, method);
    DAAL_CHECK(Argument::size() == 3, ErrorIncorrectNumberOfInputNumericTables);

    if(!checkNumericTable(get(data).get(), this->_errors.get(), dataStr(), 0, 0)) {return;}

    size_t nColsInData = get(data)->getNumberOfColumns();
    size_t nRowsInData = get(data)->getNumberOfRows();

    if(!checkNumericTable(get(dependentVariables).get(), this->_errors.get(), dependentVariablesStr(), 0, 0, 1, nRowsInData)) {return;}
    if(!checkNumericTable(get(argument).get(), this->_errors.get(), argumentStr(), 0, 0, 1, nColsInData + 1)) {return;}
}

} // namespace interface1
} // namespace mse
} // namespace optimization_solver
} // namespace algorithm
} // namespace daal
