/* file: SVDDenseBatch.java */
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
 //     Java example of singular value decomposition (SVD) in the batch
 //     processing mode
 ////////////////////////////////////////////////////////////////////////////////
 */

/**
 * <a name="DAAL-EXAMPLE-JAVA-SVDBATCH">
 * @example SVDDenseBatch.java
 */

package com.intel.daal.examples.svd;

import com.intel.daal.algorithms.svd.Batch;
import com.intel.daal.algorithms.svd.InputId;
import com.intel.daal.algorithms.svd.Method;
import com.intel.daal.algorithms.svd.Result;
import com.intel.daal.algorithms.svd.ResultId;
import com.intel.daal.data_management.data.NumericTable;
import com.intel.daal.data_management.data_source.DataSource;
import com.intel.daal.data_management.data_source.FileDataSource;
import com.intel.daal.examples.utils.Service;
import com.intel.daal.services.DaalContext;


class SVDDenseBatch {
    /* Input data set parameters */
    private static final String dataset  = "../data/batch/svd.csv";

    private static DaalContext context = new DaalContext();

    public static void main(String[] args) throws java.io.FileNotFoundException, java.io.IOException {
        /* Initialize FileDataSource to retrieve the input data from a .csv file */
        FileDataSource dataSource = new FileDataSource(context, dataset,
                DataSource.DictionaryCreationFlag.DoDictionaryFromContext,
                DataSource.NumericTableAllocationFlag.DoAllocateNumericTable);

        /* Retrieve the data from an input file */
        dataSource.loadDataBlock();

        /* Create an algorithm to compute SVD */
        Batch svdAlgorithm = new Batch(context, Float.class, Method.defaultDense);

        /* Set the input data to the SVD algorithm */
        NumericTable input = dataSource.getNumericTable();
        svdAlgorithm.input.set(InputId.data, input);

        Result res = svdAlgorithm.compute();

        /* Print the results */
        NumericTable leftSingularMatrix = res.get(ResultId.leftSingularMatrix);
        NumericTable singularValues = res.get(ResultId.singularValues);
        NumericTable rightSingularMatrix = res.get(ResultId.rightSingularMatrix);

        Service.printNumericTable("Left orthogonal matrix U (10 first vectors):", leftSingularMatrix, 10);
        Service.printNumericTable("Singular values:", singularValues);
        Service.printNumericTable("Right orthogonal matrix V:", rightSingularMatrix);

        context.dispose();
    }
}
