/* file: gbt_training_parameter.h */
/*******************************************************************************
* Copyright 2014-2017 Intel Corporation
* All Rights Reserved.
*
* If this  software was obtained  under the  Intel Simplified  Software License,
* the following terms apply:
*
* The source code,  information  and material  ("Material") contained  herein is
* owned by Intel Corporation or its  suppliers or licensors,  and  title to such
* Material remains with Intel  Corporation or its  suppliers or  licensors.  The
* Material  contains  proprietary  information  of  Intel or  its suppliers  and
* licensors.  The Material is protected by  worldwide copyright  laws and treaty
* provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
* modified, published,  uploaded, posted, transmitted,  distributed or disclosed
* in any way without Intel's prior express written permission.  No license under
* any patent,  copyright or other  intellectual property rights  in the Material
* is granted to  or  conferred  upon  you,  either   expressly,  by implication,
* inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
* property rights must be express and approved by Intel in writing.
*
* Unless otherwise agreed by Intel in writing,  you may not remove or alter this
* notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
* suppliers or licensors in any way.
*
*
* If this  software  was obtained  under the  Apache License,  Version  2.0 (the
* "License"), the following terms apply:
*
* You may  not use this  file except  in compliance  with  the License.  You may
* obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*
* Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
* distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the   License  for the   specific  language   governing   permissions  and
* limitations under the License.
*******************************************************************************/

/*
//++
//  Gradient Boosted Trees training parameter class
//--
*/

#ifndef __GBT_TRAINING_PARAMETER_H__
#define __GBT_TRAINING_PARAMETER_H__

#include "algorithms/algorithm.h"
#include "data_management/data/numeric_table.h"
#include "data_management/data/data_serialize.h"
#include "services/daal_defines.h"
#include "algorithms/engines/mt19937/mt19937.h"

namespace daal
{
namespace algorithms
{
/**
 * @defgroup base_gbt Base Gradient Boosted Trees
 * \brief Contains base classes of the gradient boosted trees algorithm
 * @ingroup training_and_prediction
 */
/**
 * \brief Contains classes of the gradient boosted trees algorithm
 */
namespace gbt
{
/**
 * \brief Contains a class for model-based training
 */
namespace training
{
/**
 * @ingroup base_gbt
 * @{
 */

/**
 * <a name="DAAL-ENUM-ALGORITHMS__GBT__TRAINING__SPLIT_METHOD"></a>
 * \brief Split finding method in gradient boosted trees algorithm
 */
enum SplitMethod
{
    exact = 0,         /*!< Exact greedy method */
    defaultSplit = 0  /*!< Default split finding method */
};

/**
 * \brief Contains version 1.0 of the Intel(R) Data Analytics Acceleration Library (Intel(R) DAAL) interface
 */
namespace interface1
{

/**
 * <a name="DAAL-CLASS-ALGORITHMS__GBT__TRAINING__PARAMETER"></a>
 * \brief Parameters for the gradient boosted trees algorithm
 *
 * \snippet gradient_boosted_trees/gbt_training_parameter.h Parameter source code
 */
/* [Parameter source code] */
class DAAL_EXPORT Parameter
{
public:
    Parameter();

    SplitMethod splitMethod;                /*!< Split finding method. Default is exact */
    size_t maxIterations;                   /*!< Maximal number of iterations of the gradient boosted trees training algorithm.
                                                 Default is 50 */
    size_t maxTreeDepth;                    /*!< Maximal tree depth, 0 for unlimited. Default is 6 */
    double shrinkage;                       /*!< Learning rate of the boosting procedure.
                                                 Scales the contribution of each tree by a factor (0, 1].
                                                 Default is 0.3 */
    double minSplitLoss;                    /*!< Loss regularization parameter. Min loss reduction required to make a further partition
                                                 on a leaf node of the tree.
                                                 Range: [0, inf). Default is 0 */
    double lambda;                          /*!< L2 regularization parameter on weights.
                                                 Range: [0, inf). Default is 1 */
    double observationsPerTreeFraction;     /*!< Fraction of observations used for a training of one tree, sampling without replacement.
                                                 Range: (0, 1]. Default is 1 (no sampling, entire dataset is used) */
    size_t featuresPerNode;                 /*!< Number of features tried as possible splits per node.
                                                 Range : [0, p] where p is the total number of features.
                                                 Default is 0 (use all features) */
    size_t minObservationsInLeafNode;       /*!< Minimal number of observations in a leaf node. Default is 5. */
    bool memorySavingMode;                  /*!< If true then use memory saving (but slower) mode. Default is false */
    engines::EnginePtr engine;              /*!< Engine for the random numbers generator used by the algorithms */
    double approxSplitAccuracy;             /*!< Used with 'approximate' split finding method only.
                                                 O(1/approxSplitAccuracy) is an estimate for a number of bins
                                                 Range (0, 1). Default is 0.03 */
    size_t maxBins;                         /*!< Used with 'histogramm' split finding method only.
                                                 Maximal number of discrete bins to bucket continuous features.
                                                 Default is 256. Increasing the number results in the higher the computation costs */
    int internalOptions;                    /*!< Internal options */
};
/* [Parameter source code] */
} // namespace interface1
using interface1::Parameter;
/** @} */
} // namespace training
}
}
} // namespace daal
#endif
