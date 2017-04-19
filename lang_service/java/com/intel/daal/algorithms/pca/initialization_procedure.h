/* file: initialization_procedure.h */
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

/*
//++
//  Implementation of the class that connects PCA Java initialization procedure
//  to C++ algorithm
//--
*/
#ifndef __INITIALIZATION_PROCEDURE_H__
#define __INITIALIZATION_PROCEDURE_H__

#include <jni.h>
#include <tbb/tbb.h>

#include "algorithms/pca/pca_types.h"
#include "java_callback.h"

namespace daal
{
namespace algorithms
{
namespace pca
{

using namespace daal::data_management;
using namespace daal::services;
/*
 * \brief Class that specifies the default method for partial results initialization
 */
template<Method method>
struct JavaPartialResultInit : public PartialResultsInitIface<method>, public JavaCallback
{
    JavaPartialResultInit(JavaVM *_jvm, jobject _javaObject) : JavaCallback(_jvm, _javaObject)
    { }

    virtual ~JavaPartialResultInit()
    {}

    /*
     * Initialize partial results
     * \param[in]       input     Input parameters of the PCA algorithm
     * \param[in,out]   pres      Partial results of the PCA algorithm
     */
    virtual void operator()(const Input &input, services::SharedPtr<PartialResult<method> > &pres)
    {
        ThreadLocalStorage tls = _tls.local();
        jint status = jvm->AttachCurrentThread((void **)(&tls.jniEnv), NULL);
        JNIEnv *env = tls.jniEnv;

        /* Get current context */
        jclass javaObjectClass = env->GetObjectClass(javaObject);
        if (javaObjectClass == NULL)
        {
            env->ThrowNew(env->FindClass("java/lang/Exception"), "Couldn't find class of this java object");
        }

        jmethodID getContextMethodID = env->GetMethodID(javaObjectClass, "getContext", "()Lcom/intel/daal/services/DaalContext;");
        if (getContextMethodID == NULL)
        {
            env->ThrowNew(env->FindClass("java/lang/Exception"), "Couldn't find getContext method");
        }
        jobject context = env->CallObjectMethod(javaObject, getContextMethodID);

        /* Get java input class object */
        jobject javaInput = constructJavaObjectFromCppObject(env, (jlong)(&input), context, "com/intel/daal/algorithms/pca/Input");

        /* Get java partial result class object */
        SerializationIfacePtr serializablePRes = services::staticPointerCast<SerializationIface, pca::PartialResult<method> >(pres);
        SerializationIfacePtr *_pres = new SerializationIfacePtr(serializablePRes);

        jobject javaPartialResult;
        if(method == correlationDense)
        {
            javaPartialResult = constructJavaObjectFromCppObject(env, (jlong)(_pres), context, "com/intel/daal/algorithms/pca/PartialCorrelationResult");
        }
        else // svdDense
        {
            javaPartialResult = constructJavaObjectFromCppObject(env, (jlong)(_pres), context, "com/intel/daal/algorithms/pca/PartialSVDResult");
        }

        /* Get ID of inintialize method of initialization procedure class */
        jmethodID initializeMethodID = env->GetMethodID(javaObjectClass, "initialize",
                                                        "(Lcom/intel/daal/algorithms/pca/Input;Lcom/intel/daal/algorithms/PartialResult;)V");
        if (initializeMethodID == NULL)
        {
            env->ThrowNew(env->FindClass("java/lang/Exception"), "Couldn't find cInitialize method");
        }

        /* Call inintialize method of initialization procedure class */
        env->CallVoidMethod(javaObject, initializeMethodID, javaInput, javaPartialResult);

        if(!tls.is_main_thread)
        {
            status = jvm->DetachCurrentThread();
        }
        _tls.local() = tls;
    }
};

} // namespace daal::algorithms::pca
} // namespace daal::algorithms
} // namespace daal

#endif
