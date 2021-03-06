/* file: SparkRidgeRegressionNormEq.java */
/*******************************************************************************
* Copyright 2017 Intel Corporation
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
//      Java sample of ridge regression in the distributed processing mode.
//
//      The program trains the ridge regression model on a training
//      data set with the normal equations method and computes regression for
//      the test data.
////////////////////////////////////////////////////////////////////////////////
*/

package DAAL;

import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;
import java.util.Map;
import java.util.HashMap;

import org.apache.spark.api.java.*;
import org.apache.spark.api.java.function.*;
import org.apache.spark.SparkConf;

import com.intel.daal.algorithms.ridge_regression.*;
import com.intel.daal.algorithms.ridge_regression.training.*;
import com.intel.daal.algorithms.ridge_regression.prediction.*;

import scala.Tuple2;
import com.intel.daal.data_management.data.*;
import com.intel.daal.services.*;

public class SparkRidgeRegressionNormEq {
    /* Class containing the algorithm results */
    static class RidgeRegressionResult {
        public HomogenNumericTable prediction;
        public HomogenNumericTable beta;
    }

    public static RidgeRegressionResult runRidgeRegression(
            DaalContext context,
            JavaRDD<Tuple2<HomogenNumericTable, HomogenNumericTable>> trainDataRDD,
            JavaRDD<Tuple2<HomogenNumericTable, HomogenNumericTable>> testDataRDD) {

        JavaRDD<PartialResult> partsRDD = trainLocal(trainDataRDD);

        Model model = trainMaster(context, partsRDD);

        HomogenNumericTable prediction = testModel(context, model, testDataRDD);

        RidgeRegressionResult result = new RidgeRegressionResult();
        result.beta = (HomogenNumericTable)model.getBeta();
        result.prediction = prediction;
        return result;
    }

    private static JavaRDD<PartialResult> trainLocal(
            JavaRDD<Tuple2<HomogenNumericTable, HomogenNumericTable>> trainDataRDD) {

        return trainDataRDD.map(new Function<Tuple2<HomogenNumericTable, HomogenNumericTable>, PartialResult>() {
            public PartialResult call(Tuple2<HomogenNumericTable, HomogenNumericTable> tup) {
                DaalContext context = new DaalContext();

                /* Create an algorithm object to train the ridge regression model with the normal equations method */
                TrainingDistributedStep1Local ridgeRegressionTraining = new TrainingDistributedStep1Local(
                    context, Double.class, TrainingMethod.normEqDense);

                /* Set the input data on local nodes */
                tup._1.unpack(context);
                tup._2.unpack(context);
                ridgeRegressionTraining.input.set(TrainingInputId.data, tup._1);
                ridgeRegressionTraining.input.set(TrainingInputId.dependentVariable, tup._2);

                /* Build a partial ridge regression model */
                PartialResult pres = ridgeRegressionTraining.compute();
                pres.pack();

                context.dispose();
                return pres;
            }
        });
    }

    private static Model trainMaster(DaalContext context, JavaRDD<PartialResult> partsRDD) {

        /* Create an algorithm object to train the ridge regression model with the normal equations method */
        TrainingDistributedStep2Master ridgeRegressionTraining = new TrainingDistributedStep2Master(
            context, Double.class, TrainingMethod.normEqDense);

        /* Add partial results computed on local nodes to the algorithm on the master node */
        List<PartialResult> collectedPres = partsRDD.collect();
        for (PartialResult value : collectedPres) {
            value.unpack(context);
            ridgeRegressionTraining.input.add(MasterInputId.partialModels, value);
        }

        /* Build and retrieve the final ridge regression model */
        ridgeRegressionTraining.compute();

        TrainingResult trainingResult = ridgeRegressionTraining.finalizeCompute();

        Model model = trainingResult.get(TrainingResultId.model);
        return model;
    }

    private static HomogenNumericTable testModel(
            DaalContext context,
            final Model model,
            JavaRDD<Tuple2<HomogenNumericTable, HomogenNumericTable>> testData) {

        /* Create algorithm objects to predict values of ridge regression with the default method */
        PredictionBatch ridgeRegressionPredict = new PredictionBatch(
            context, Double.class, PredictionMethod.defaultDense);

        /* Pass the test data to the algorithm */
        List<Tuple2<HomogenNumericTable, HomogenNumericTable>> parts_List = testData.collect();
        for (Tuple2<HomogenNumericTable, HomogenNumericTable> value : parts_List) {
            value._1.unpack(context);
            ridgeRegressionPredict.input.set(PredictionInputId.data, value._1);
        }

        ridgeRegressionPredict.input.set(PredictionInputId.model, model);

        /* Compute and retrieve the prediction results */
        PredictionResult predictionResult = ridgeRegressionPredict.compute();

        return (HomogenNumericTable)predictionResult.get(PredictionResultId.prediction);
    }
}
