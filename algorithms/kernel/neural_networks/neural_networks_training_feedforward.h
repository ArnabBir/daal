/* file: neural_networks_training_feedforward.h */
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
//  Declaration of common functions for using optimizaion solver
//  in feedforward neural network
//--
*/

#ifndef __NEURAL_NETWORKS_TRAINING_FEEDFORWARD_H__
#define __NEURAL_NETWORKS_TRAINING_FEEDFORWARD_H__

#include "algorithms/optimization_solver/objective_function/precomputed_batch.h"
#include "algorithms/optimization_solver/iterative_solver/iterative_solver_batch.h"
#include "algorithms/neural_networks/neural_networks_types.h"

namespace daal
{
namespace algorithms
{
namespace neural_networks
{
namespace internal
{

template<typename algorithmFPType>
class Solver
{
public:
    typedef optimization_solver::precomputed::Batch<algorithmFPType> ObjectiveFunction;
    typedef services::SharedPtr<optimization_solver::iterative_solver::Batch>  IterativeSolverPtr;

    Solver();
    ~Solver();
    services::SharedPtr<services::KernelErrorCollection> updateWeightsAndBiases(
                const data_management::NumericTablePtr &weightsAndBiases,
                const data_management::NumericTablePtr &weightsAndBiasesDerivatives);

    data_management::NumericTablePtr getMinimum();
    services::Status init(const IterativeSolverPtr &_solver);
    services::Status setSolverOptionalResult(algorithms::OptionalArgumentPtr optRes)
    {
        if(solverResult && optRes)
        {
            solverResult->set(optimization_solver::iterative_solver::optionalResult, optRes);
        }
        return services::Status();
    }
    algorithms::OptionalArgumentPtr getSolverOptionalResult()
    {
        if(solverResult)
        {
            return solverResult->get(optimization_solver::iterative_solver::optionalResult);
        }
        return algorithms::OptionalArgumentPtr();
    }
    size_t getNIterations() const {return _nIterationSolver;}
    size_t getBatchSize() const {return _batchSize;}
protected:
    services::SharedPtr<ObjectiveFunction> precomputed;
    IterativeSolverPtr solver;
    services::SharedPtr<optimization_solver::iterative_solver::Result> solverResult;
    services::SharedPtr<data_management::HomogenNumericTable<int> > nIterations;
    size_t _nIterationSolver;
    size_t _batchSize;
};

class LearnableLayerIndices
{
public:
    LearnableLayerIndices(ForwardLayers *forwardLayers);
    virtual ~LearnableLayerIndices();

    size_t nLearnable() const;

    size_t layerIndex(size_t idx) const;

    bool getError() const;
protected:
    size_t nLearnableLayers;
    size_t *layerIndices;
    bool memAllocError;
};

}
}
}
}

#endif
