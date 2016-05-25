/* file: smoothrelu_layer_forward.h */
/*******************************************************************************
* Copyright 2014-2016 Intel Corporation
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
//  Implementation of the interface for the forward SmoothReLU layer in the batch processing mode
//--
*/

#ifndef __NEURAL_NETWORK_SMOOTHRELU_LAYER_FORWARD_H__
#define __NEURAL_NETWORK_SMOOTHRELU_LAYER_FORWARD_H__

#include "algorithms/algorithm.h"
#include "data_management/data/tensor.h"
#include "services/daal_defines.h"
#include "algorithms/neural_networks/layers/layer.h"
#include "algorithms/neural_networks/layers/smoothrelu/smoothrelu_layer_types.h"
#include "algorithms/neural_networks/layers/smoothrelu/smoothrelu_layer_forward_types.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace layers
{
namespace smoothrelu
{
namespace forward
{
namespace interface1
{
/**
* \brief Provides methods to run implementations of the forward smooth relu layer
*        This class is associated with the daal::algorithms::neural_networks::layers::smoothrelu::forward::Batch class
*        and supports the method of forward smooth relu layer computation in the batch processing mode
*
* \tparam algorithmFPType  Data type to use in intermediate computations of forward smooth relu layer, double or float
* \tparam method           Computation method of the layer, \ref daal::algorithms::neural_networks::layers::smoothrelu::Method
* \tparam cpu              Version of the cpu-specific implementation of the layer, \ref daal::CpuType
*/
template<typename algorithmFPType, Method method, CpuType cpu>
class DAAL_EXPORT BatchContainer : public AnalysisContainerIface<batch>
{
public:
    /**
    * Constructs a container for the forward SmoothReLU layer with a specified environment
    * in the batch processing mode
    * \param[in] daalEnv   Environment object
    */
    BatchContainer(daal::services::Environment::env *daalEnv);
    /** Default destructor */
    ~BatchContainer();
    /**
     * Computes the result of the forward SmoothReLU layer in the batch processing mode
     */
    void compute() DAAL_C11_OVERRIDE;
};

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__LAYERS__SMOOTHRELU__FORWARD__BATCH"></a>
 * \brief Provides methods for the forward smooth relu layer in the batch processing mode
 *
 * \tparam algorithmFPType  Data type to use in intermediate computations for the forward smooth relu layer, double or float
 * \tparam method           Forward smooth relu layer method, \ref Method
 *
 * \par Enumerations
 *      - \ref Method                     Computation methods for the forward smooth relu layer
 *      - \ref forward::InputId           Identifiers of input objects for the forward smooth relu layer
 *      - \ref forward::ResultId          Identifiers of result objects for the forward smooth relu layer
 *      - \ref forward::ResultLayerDataId Identifiers of extra results computed by the forward smooth relu layer
 *      - \ref LayerDataId                Identifiers of collection in result objects for the forward smooth relu layer
 *
 * \par References
 *      - <a href="DAAL-REF-SMOOTHRELUFORWARD-ALGORITHM">Forward smooth relu layer description and usage models</a>
 *      - \ref backward::interface1::Batch "backward::Batch" class
 */
template<typename algorithmFPType = float, Method method = defaultDense>
class Batch : public layers::forward::LayerIface
{
public:
    Input input;          /*!< Forward smooth relu layer input */

    /** Default constructor */
    Batch()
    {
        initialize();
    };

    /**
     * Constructs a forward smooth relu layer by copying input objects
     * and parameters of another forward smooth relu layer in the batch processing mode
     * \param[in] other Algorithm to use as the source to initialize the input objects
     *                  and parameters of the layer
     */
    Batch(const Batch<algorithmFPType, method> &other)
    {
        initialize();
        input.set(layers::forward::data, other.input.get(layers::forward::data));
        input.set(layers::forward::weights, other.input.get(layers::forward::weights));
        input.set(layers::forward::biases, other.input.get(layers::forward::biases));
    }

    /**
    * Returns the method of the layer
    * \return Method of the layer
    */
    virtual int getMethod() const DAAL_C11_OVERRIDE { return(int) method; }

    /**
     * Returns the structure that contains the input objects of the forward smooth relu layer
     * \return Structure that contains the input objects of the forward smooth relu layer
     */
    virtual Input *getLayerInput() DAAL_C11_OVERRIDE { return &input; }

    /**
     * Returns the structure that contains parameters of the forward smooth relu layer
     * \return Structure that contains parameters of the forward smooth relu layer
     */
    virtual Parameter *getLayerParameter() DAAL_C11_OVERRIDE { return NULL; };

    /**
     * Returns the structure that contains result of the forward smooth relu layer
     * \return Structure that contains result of the forward smooth relu layer
     */
    services::SharedPtr<layers::forward::Result> getLayerResult() DAAL_C11_OVERRIDE
    {
        return _result;
    }

    /**
     * Returns the structure that contains the result of the forward smooth relu layer
     * \return Structure that contains the result of the forward smooth relu layer
     */
    services::SharedPtr<Result> getResult()
    {
        return _result;
    }

    /**
     * Registers user-allocated memory to store the result of the forward smooth relu layer
     * \param[in] result Structure to store the result of the forward smooth relu layer
     */
    void setResult(const services::SharedPtr<Result>& result)
    {
        _result = result;
        _res = _result.get();
    }

    /**
     * Returns a pointer to a newly allocated forward smooth relu layer
     * with a copy of the input objects and parameters for this forward smooth relu layer
     * in the batch processing mode
     * \return Pointer to the newly allocated layer
     */
    services::SharedPtr<Batch<algorithmFPType, method> > clone() const
    {
        return services::SharedPtr<Batch<algorithmFPType, method> >(cloneImpl());
    }

    /**
    * Allocates memory to store the result of the forward smooth relu layer
    */
    virtual void allocateResult() DAAL_C11_OVERRIDE
    {
        this->_result->template allocate<algorithmFPType>(&(this->input), NULL, (int) method);
        this->_res = this->_result.get();
    }

protected:
    virtual Batch<algorithmFPType, method> *cloneImpl() const DAAL_C11_OVERRIDE
    {
        return new Batch<algorithmFPType, method>(*this);
    }

    void initialize()
    {
        Analysis<batch>::_ac = new __DAAL_ALGORITHM_CONTAINER(batch, BatchContainer, algorithmFPType, method)(&_env);
        _in = &input;
        _result = services::SharedPtr<Result>(new Result());
    }

private:
    services::SharedPtr<Result> _result;
};
} // namespace interface1
using interface1::BatchContainer;
using interface1::Batch;
} // namespace forward
} // namespace smoothrelu
} // namespace layers
} // namespace neural_networks
} // namespace algorithms
} // namespace daal
#endif
