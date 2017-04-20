/* file: InitializationProcedureCorrelation.java */
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
 * @ingroup pca
 * @{
 */
package com.intel.daal.algorithms.pca;

import java.nio.DoubleBuffer;
import java.nio.IntBuffer;

import com.intel.daal.data_management.data.NumericTable;
import com.intel.daal.services.DaalContext;

/**
 * <a name="DAAL-CLASS-ALGORITHMS__PCA__INITIALIZATIONPROCEDURECORRELATION"></a>
 * @brief Class that specifies the default method for partial results initialization
 */
public class InitializationProcedureCorrelation extends InitializationProcedureIface {

    /**
     * Constructs default initialization procedure
     * @param context   Context to manage the algorithm
     */
    public InitializationProcedureCorrelation(DaalContext context) {
        super(context);
        this.cInitIface = cNewJavaInitializationProcedure();
        this.cObject = this.cInitIface;
    }

    /**
     * Initializes partial results
     * @param input         Input objects for the PCA algorithm
     * @param partialResult Partial results of the PCA algorithm
     */
    @Override
    public void initialize(Input input, com.intel.daal.algorithms.PartialResult partialResult) {
        NumericTable dataTable = input.get(InputId.data);
        if(dataTable == null) { throw new IllegalArgumentException("Null input data"); }
        int nColumns = (int) (dataTable.getNumberOfColumns());

        PartialCorrelationResult partialCorrelationResult = (PartialCorrelationResult) partialResult;

        NumericTable nObservationsCorrelationTable = partialCorrelationResult.get(PartialCorrelationResultID.nObservations);
        NumericTable sumCorrelationTable = partialCorrelationResult.get(PartialCorrelationResultID.crossProductCorrelation);
        NumericTable crossProductCorrelationTable = partialCorrelationResult.get(PartialCorrelationResultID.sumCorrelation);

        IntBuffer nObservationsCorrelationBuffer = IntBuffer.allocate(1);
        DoubleBuffer sumCorrelationBuffer = DoubleBuffer.allocate(nColumns);
        DoubleBuffer crossProductCorrelationBuffer = DoubleBuffer.allocate(nColumns);

        if(nObservationsCorrelationTable == null || sumCorrelationTable == null || crossProductCorrelationTable == null) {
            throw new IllegalArgumentException("Null table in PartialCorrelationResult"); }

        nObservationsCorrelationBuffer = nObservationsCorrelationTable.getBlockOfRows(0, 1, nObservationsCorrelationBuffer);
        sumCorrelationBuffer = sumCorrelationTable.getBlockOfRows(0, 1, sumCorrelationBuffer);
        crossProductCorrelationBuffer = crossProductCorrelationTable.getBlockOfRows(0, 1, crossProductCorrelationBuffer);

        nObservationsCorrelationBuffer.put(0, 0);
        for (int i = 0; i < nColumns; i++) {
            sumCorrelationBuffer.put(i, 0.0);
            crossProductCorrelationBuffer.put(i, 0.0);
        }

        nObservationsCorrelationTable.releaseBlockOfRows(0, 1, nObservationsCorrelationBuffer);
        sumCorrelationTable.releaseBlockOfRows(0, 1, sumCorrelationBuffer);
        crossProductCorrelationTable.releaseBlockOfRows(0, 1, crossProductCorrelationBuffer);
    }

    /**
    * Releases memory allocated for the native iface object
    */
    @Override
    public void dispose() {
        if(this.cInitIface != 0) {
            cInitIfaceDispose(this.cInitIface);
            this.cInitIface = 0;
        }
    }

    private long cInitIface;

    private native long cNewJavaInitializationProcedure();
    private native void cInitIfaceDispose(long cInitIface);
}
/** @} */
