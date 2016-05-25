/* file: ForwardResult.java */
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

package com.intel.daal.algorithms.neural_networks.layers.pooling2d;

import com.intel.daal.data_management.data.HomogenTensor;
import com.intel.daal.data_management.data.Tensor;
import com.intel.daal.services.DaalContext;

/**
 * @brief Class that provides methods to access the result obtained with the compute() method of the forward pooling layer
 */
public class ForwardResult extends com.intel.daal.algorithms.neural_networks.layers.ForwardResult {
    /** @private */
    static {
        System.loadLibrary("JavaAPI");
    }

    /**
     * Constructs the forward pooling layer result
     * @param context   Context to manage the forward pooling layer result
     */
    public ForwardResult(DaalContext context) {
        super(context);
        this.cObject = cNewResult();
    }

    /**
    * Constructs the forward pooling layer result
    * @param context   Context to manage the forward pooling layer result
    * @param cObject   Address of C++ forward result
    */
    public ForwardResult(DaalContext context, long cObject) {
        super(context, cObject);
    }

    private native long cNewResult();
}
