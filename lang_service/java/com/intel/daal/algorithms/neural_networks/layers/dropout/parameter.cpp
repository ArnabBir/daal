/* file: parameter.cpp */
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

#include <jni.h>
#include "neural_networks/layers/dropout/JParameter.h"

#include "daal.h"

#include "common_helpers.h"

USING_COMMON_NAMESPACES();
using namespace daal::algorithms::neural_networks::layers;

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_dropout_Parameter
 * Method:    cInit
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_dropout_Parameter_cInit
  (JNIEnv *env, jobject thisObj)
{
    return (jlong)(new dropout::Parameter);
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_dropout_Parameter
 * Method:    cGetRetainRatio
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_dropout_Parameter_cGetRetainRatio
  (JNIEnv *env, jobject thisObj, jlong cParameter)
{
    return (((dropout::Parameter *)cParameter))->retainRatio;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_dropout_Parameter
 * Method:    cSetRetainRatio
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_dropout_Parameter_cSetRetainRatio
  (JNIEnv *env, jobject thisObj, jlong cParameter, jdouble retainRatio)
{
    (((dropout::Parameter *)cParameter))->retainRatio = retainRatio;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_dropout_Parameter
 * Method:    cGetSeed
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_dropout_Parameter_cGetSeed
  (JNIEnv *env, jobject thisObj, jlong cParameter)
{
    return (jlong)((((dropout::Parameter *)cParameter))->seed);
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_dropout_Parameter
 * Method:    cSetSeed
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_dropout_Parameter_cSetSeed
  (JNIEnv *env, jobject thisObj, jlong cParameter, jlong seed)
{
    (((dropout::Parameter *)cParameter))->seed = (size_t)seed;
}
