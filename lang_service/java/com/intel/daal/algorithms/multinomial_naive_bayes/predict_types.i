/* file: predict_types.i */
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

#include "daal.h"

#include "JComputeMode.h"
#include "multinomial_naive_bayes/prediction/JPredictionMethod.h"
#include "classifier/prediction/JNumericTableInputId.h"
#include "classifier/prediction/JPredictionResultId.h"
#include "classifier/prediction/JModelInputId.h"

using namespace daal;
using namespace daal::algorithms;

#define jBatch   com_intel_daal_algorithms_ComputeMode_batchValue

#define DefaultDense com_intel_daal_algorithms_multinomial_naive_bayes_prediction_PredictionMethod_DefaultDense
#define FastCSR      com_intel_daal_algorithms_multinomial_naive_bayes_prediction_PredictionMethod_FastCSR

#define Data         com_intel_daal_algorithms_classifier_prediction_NumericTableInputId_Data

#define ModelInput   com_intel_daal_algorithms_classifier_prediction_ModelInputId_Model

#define Prediction   com_intel_daal_algorithms_classifier_prediction_PredictionResultId_Prediction

typedef multinomial_naive_bayes::prediction::Batch<float, multinomial_naive_bayes::prediction::defaultDense>     nb_pr_of_s_dd;
typedef multinomial_naive_bayes::prediction::Batch<double, multinomial_naive_bayes::prediction::defaultDense>    nb_pr_of_d_dd;
typedef multinomial_naive_bayes::prediction::Batch<float, multinomial_naive_bayes::prediction::fastCSR>          nb_pr_of_s_fc;
typedef multinomial_naive_bayes::prediction::Batch<double, multinomial_naive_bayes::prediction::fastCSR>         nb_pr_of_d_fc;
