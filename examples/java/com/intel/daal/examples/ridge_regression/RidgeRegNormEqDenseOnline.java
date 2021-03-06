/* file: RidgeRegNormEqDenseOnline.java */
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
 //  Content:
 //     Java example of ridge regression in the online processing mode.
 //
 //     The program trains the ridge regression model on a training
 //     data set with the normal equations method and computes regression for
 //     the test data.
 ////////////////////////////////////////////////////////////////////////////////
 */

/**
 * <a name="DAAL-EXAMPLE-JAVA-RIDGEREGRESSIONNORMEQONLINE">
 * @example RidgeRegNormEqDenseOnline.java
 */

package com.intel.daal.examples.ridge_regression;

import com.intel.daal.algorithms.ridge_regression.Model;
import com.intel.daal.algorithms.ridge_regression.prediction.*;
import com.intel.daal.algorithms.ridge_regression.training.*;
import com.intel.daal.data_management.data.NumericTable;
import com.intel.daal.data_management.data.HomogenNumericTable;
import com.intel.daal.data_management.data.MergedNumericTable;
import com.intel.daal.data_management.data_source.DataSource;
import com.intel.daal.data_management.data_source.FileDataSource;
import com.intel.daal.examples.utils.Service;
import com.intel.daal.services.DaalContext;

class RidgeRegNormEqDenseOnline {
    /* Input data set parameters */
    private static final String trainDatasetFileName = "../data/online/linear_regression_train.csv";

    private static final String testDatasetFileName  = "../data/online/linear_regression_test.csv";

    private static final int nFeatures            = 10;  /* Number of features in training and testing data sets */
    private static final int nDependentVariables  = 2;   /* Number of dependent variables that correspond to each observation */
    private static final int nTrainVectorsInBlock = 250;

    static Model        model;
    static NumericTable results;
    static NumericTable testDependentVariables;

    private static DaalContext context = new DaalContext();

    public static void main(String[] args) throws java.io.FileNotFoundException, java.io.IOException {

        trainModel();

        testModel();

        printResults();

        context.dispose();
    }

    private static void trainModel() {

        /* Initialize FileDataSource to retrieve the input data from a .csv file */
        FileDataSource trainDataSource = new FileDataSource(context, trainDatasetFileName,
                DataSource.DictionaryCreationFlag.DoDictionaryFromContext,
                DataSource.NumericTableAllocationFlag.NotAllocateNumericTable);

        /* Create Numeric Tables for training data and labels */
        NumericTable trainData = new HomogenNumericTable(context, Float.class, nFeatures, 0, NumericTable.AllocationFlag.DoNotAllocate);
        NumericTable trainDependentVariables = new HomogenNumericTable(context, Float.class, nDependentVariables, 0,
                                                                       NumericTable.AllocationFlag.DoNotAllocate);
        MergedNumericTable mergedData = new MergedNumericTable(context);
        mergedData.addNumericTable(trainData);
        mergedData.addNumericTable(trainDependentVariables);

        TrainingOnline ridgeRegressionTraining = new TrainingOnline(context, Float.class, TrainingMethod.normEqDense);

        ridgeRegressionTraining.input.set(TrainingInputId.data, trainData);
        ridgeRegressionTraining.input.set(TrainingInputId.dependentVariable, trainDependentVariables);

        while ((trainDataSource.loadDataBlock(nTrainVectorsInBlock, mergedData)) == nTrainVectorsInBlock) {
            ridgeRegressionTraining.compute();
        }

        TrainingResult trainingResult = ridgeRegressionTraining.finalizeCompute();
        model = trainingResult.get(TrainingResultId.model);
    }

    private static void testModel() {
        /* Initialize FileDataSource to retrieve the input data from a .csv file */
        FileDataSource testDataSource = new FileDataSource(context, testDatasetFileName,
                DataSource.DictionaryCreationFlag.DoDictionaryFromContext,
                DataSource.NumericTableAllocationFlag.NotAllocateNumericTable);

        /* Create Numeric Tables for testing data and labels */
        NumericTable testData = new HomogenNumericTable(context, Float.class, nFeatures, 0, NumericTable.AllocationFlag.DoNotAllocate);
        testDependentVariables = new HomogenNumericTable(context, Float.class, nDependentVariables, 0, NumericTable.AllocationFlag.DoNotAllocate);
        MergedNumericTable mergedData = new MergedNumericTable(context);
        mergedData.addNumericTable(testData);
        mergedData.addNumericTable(testDependentVariables);

        /* Retrieve the data from an input file */
        testDataSource.loadDataBlock(mergedData);

        /* Create algorithm objects to predict values of ridge regression with the default method */
        PredictionBatch ridgeRegressionPredict = new PredictionBatch(context, Float.class,
                PredictionMethod.defaultDense);

        /* Provide the input data */
        ridgeRegressionPredict.input.set(PredictionInputId.data, testData);
        ridgeRegressionPredict.input.set(PredictionInputId.model, model);

        /* Compute and retrieve the prediction results */
        PredictionResult predictionResult = ridgeRegressionPredict.compute();

        results = predictionResult.get(PredictionResultId.prediction);
    }

    private static void printResults() {
        NumericTable beta = model.getBeta();
        NumericTable expected = testDependentVariables;
        Service.printNumericTable("Coefficients: ", beta);
        Service.printNumericTable("First 10 rows of results (obtained): ", results, 10);
        Service.printNumericTable("First 10 rows of results (expected): ", expected, 10);
    }
}
