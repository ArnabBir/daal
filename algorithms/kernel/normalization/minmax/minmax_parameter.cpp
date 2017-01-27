/* file: minmax_parameter.cpp */
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
//  Implementation of minmax algorithm and types methods.
//--
*/

#include "minmax_types.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace normalization
{
namespace minmax
{
namespace interface1
{

/** Constructs min-max normalization parameters */
DAAL_EXPORT ParameterBase::ParameterBase(double lowerBound, double upperBound,
    const services::SharedPtr<low_order_moments::BatchIface> &moments) :
    lowerBound(lowerBound), upperBound(upperBound), moments(moments) { }

/**
 * Check the correctness of the %ParameterBase object
 */
DAAL_EXPORT void ParameterBase::check() const
{
    if (!moments) { this->_errors->add(services::ErrorNullParameterNotSupported); return; }
    if (lowerBound >= upperBound) { this->_errors->add(services::ErrorLowerBoundGreaterThanOrEqualToUpperBound); return; }
}

}// namespace interface1
}// namespace minmax
}// namespace normalization
}// namespace algorithms
}// namespace daal
