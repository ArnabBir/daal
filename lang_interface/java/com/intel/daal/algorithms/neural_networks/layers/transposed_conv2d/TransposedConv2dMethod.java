/* file: TransposedConv2dMethod.java */
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
 * @ingroup transposed_conv2d
 * @{
 */
package com.intel.daal.algorithms.neural_networks.layers.transposed_conv2d;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__NEURAL_NETWORKS__LAYERS__TRANSPOSED_CONV2D__TRANSPOSEDCONV2DMETHOD"></a>
 * @brief Available methods for the 2D transposed convolution layer
 */
public final class TransposedConv2dMethod {
    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    private int _value;

    public TransposedConv2dMethod(int value) {
        _value = value;
    }

    /**
     * Returns the value of the method identifier
     * @return Value of the method identifier
     */
    public int getValue() {
        return _value;
    }

    private static final int DefaultMethodValue = 0;

    public static final TransposedConv2dMethod defaultDense = new TransposedConv2dMethod(DefaultMethodValue); /*!< Default method */
}
/** @} */
