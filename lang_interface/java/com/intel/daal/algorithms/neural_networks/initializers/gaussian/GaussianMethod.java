/* file: GaussianMethod.java */
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
 * @defgroup initializers_gaussian Gaussian Initializer
 * @brief Contains classes for neural network weights and biases gaussian initializer
 * @ingroup initializers
 * @{
 */
package com.intel.daal.algorithms.neural_networks.initializers.gaussian;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__INITIALIZERS__GAUSSIAN__GAUSSIANMETHOD"></a>
 * @brief Available methods for the gaussian initializer
 */
public final class GaussianMethod {
    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    private int _value;

    /**
     * Constructs the method identifier using the provided value
     * @param value     Value of the method identifier
     */
    public GaussianMethod(int value) {
        _value = value;
    }

    /**
     * Returns the value of the method identifier
     * @return Value of the method identifier
     */
    public int getValue() {
        return _value;
    }

    private static final int defaultDenseId = 0;

    public static final GaussianMethod defaultDense = new GaussianMethod(defaultDenseId); /*!< Default: performance-oriented method */
}
/** @} */
