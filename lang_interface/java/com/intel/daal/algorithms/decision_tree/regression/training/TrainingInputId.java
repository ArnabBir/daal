/* file: TrainingInputId.java */
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

/**
 * @defgroup decision_tree_regression_training Training
 * @brief Contains classes for training the decision_tree regression model
 * @ingroup decision_tree_regression
 * @{
 */
/**
 * @brief Contains classes for training the regression model
 */
package com.intel.daal.algorithms.decision_tree.regression.training;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__DECISION_TREE__REGRESSION__TRAINING__TRAININGINPUTID"></a>
 * @brief Available identifiers of the results in the training stage of decision tree
 */
public final class TrainingInputId {
    private int _value;

    /** Default constructor */
    public TrainingInputId(int value) {
        _value = value;
    }

    /**
     * Returns the value corresponding to the identifier of input object
     * @return Value corresponding to the identifier
     */
    public int getValue() {
        return _value;
    }

    private static final int dataId                          = 0;
    private static final int dependentVariablesId            = 1;
    private static final int dataForPruningId                = 2;
    private static final int dependentVariablesForPruningId  = 3;

    public static final TrainingInputId data                         = new TrainingInputId(dataId);
        /*!< Input data table */
    public static final TrainingInputId dependentVariables           = new TrainingInputId(dependentVariablesId);
        /*!< Values of the dependent variable for the input data */
    public static final TrainingInputId dataForPruning               = new TrainingInputId(dataForPruningId);
        /*!< Pruning data set */
    public static final TrainingInputId dependentVariablesForPruning = new TrainingInputId(dependentVariablesForPruningId);
        /*!< Labels of the pruning data set */
}
/** @} */
