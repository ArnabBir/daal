/* file: LogisticCrossLayerDataId.java */
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
 * @ingroup logistic_cross
 * @{
 */
package com.intel.daal.algorithms.neural_networks.layers.logistic_cross;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__LAYERS__LOGISTIC_CROSS__LOGISTICCROSSLAYERDATAID"></a>
 * \brief Identifiers of input objects for the backward logistic cross-entropy layer and results for the forward logistic cross-entropy layer
 */
public final class LogisticCrossLayerDataId {
    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    private int _value;

    /**
     * Constructs the result object identifier using the provided value
     * @param value     Value of the input object identifier
     */
    public LogisticCrossLayerDataId(int value) {
        _value = value;
    }

    /**
     * Returns the value corresponding to the result identifier
     * @return Value corresponding to the result identifier
     */
    public int getValue() {
        return _value;
    }

    private static final int auxDataId = 2;
    private static final int auxGroundTruthId = 3;

    public static final LogisticCrossLayerDataId auxData = new LogisticCrossLayerDataId(auxDataId); /*!< Tensor that stores probabilities for the forward logistic cross-entropy layer */
    public static final LogisticCrossLayerDataId auxGroundTruth = new LogisticCrossLayerDataId(auxGroundTruthId); /*!< Tensor that stores ground truth data for the forward logistic cross-entropy layer */
}
/** @} */
