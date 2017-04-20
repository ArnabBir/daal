/* file: algorithm_base_common.h */
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
//  Implementation of base classes defining algorithm interface.
//--
*/

#ifndef __ALGORITHM_BASE_COMMON_H__
#define __ALGORITHM_BASE_COMMON_H__

#include "services/daal_memory.h"
#include "services/daal_kernel_defines.h"
#include "services/error_handling.h"
#include "services/env_detect.h"
#include "algorithms/algorithm_types.h"

namespace daal
{
namespace algorithms
{
namespace interface1
{

/**
 * @addtogroup base_algorithms
 * @{
 */

/**
 *  <a name="DAAL-CLASS-ALGORITHMS__ALGORITHMIFACE"></a>
 *  \brief Abstract class which defines interface for the library component
 *         related to data processing involving execution of the algorithms
 *         for analysis, modeling, and prediction
 */
class AlgorithmIface
{
public:
    DAAL_NEW_DELETE();

    virtual ~AlgorithmIface() {}

    /**
     * Validates parameters of the compute method
     */
    virtual void checkComputeParams() = 0;

    /**
     * Validates result parameters of the compute method
     */
    virtual void checkResult() = 0;

    /**
     * Returns errors during the computations
     * \return Errors during the computations
     */
    virtual services::SharedPtr<services::ErrorCollection> getErrors() = 0;
};

/**
 *  <a name="DAAL-CLASS-ALGORITHMS__ALGORITHMIFACEIMPL"></a>
 *  \brief Implements the abstract interface AlgorithmIface. AlgorithmIfaceImpl is, in turn, the base class
 *         for the classes interfacing the major compute modes: batch, online and distributed
 */
class AlgorithmIfaceImpl : public AlgorithmIface
{
public:
    /** Default constructor */
    AlgorithmIfaceImpl() : _enableChecks(true), _errors(new services::ErrorCollection()) {}

    virtual ~AlgorithmIfaceImpl() {}

    /**
     * Sets flag of requiring parameters checks
     * \param enableChecksFlag True if checks are needed, false if no checks are required
     */
    void enableChecks(bool enableChecksFlag)
    {
        _enableChecks = enableChecksFlag;
    }

    /**
     * Returns flag of checking necessity
     * \return flag of checking necessity
     */
    bool isChecksEnabled() const
    {
        return _enableChecks;
    }

    /**
     * Returns error collection of the algorithm
     * \return Error collection of the algorithm
     */
    services::SharedPtr<services::ErrorCollection> getErrors()
    {
        return _errors;
    }

private:
    bool _enableChecks;

protected:
    void throwIfPossible()
    {
#if (!defined(DAAL_NOTHROW_EXCEPTIONS))
        throw services::Exception::getException(this->_errors->getDescription());
#endif
    }

    void getEnvironment()
    {
        int cpuid = (int)daal::services::Environment::getInstance()->getCpuId();
        if(cpuid < 0)
        {
            _errors->add(services::ErrorCpuNotSupported);
        }
        _env.cpuid = cpuid;
    }

    daal::services::Environment::env    _env;

    services::SharedPtr<services::ErrorCollection> _errors;
};

/** @} */
} // namespace interface1
using interface1::AlgorithmIface;
using interface1::AlgorithmIfaceImpl;

}
}
#endif
