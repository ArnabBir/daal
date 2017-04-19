/* file: outlierdetection_univariate_kernel.h */
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
//  Declaration of template structs for Outliers Detection.
//--
*/

#ifndef __UNIVAR_OUTLIERDETECTION_KERNEL_H__
#define __UNIVAR_OUTLIERDETECTION_KERNEL_H__

#include "numeric_table.h"
#include "outlier_detection_univariate.h"
#include "outlier_detection_univariate_types.h"
#include "kernel.h"
#include "numeric_table.h"

using namespace daal::data_management;

namespace daal
{
namespace algorithms
{
namespace univariate_outlier_detection
{
namespace internal
{

template <typename algorithmFPType, Method method, CpuType cpu>
struct OutlierDetectionKernel : public Kernel
{
    services::Status compute(const NumericTable *a, NumericTable *r, const daal::algorithms::Parameter *par);
};

} // namespace internal

} // namespace univariate_outlier_detection

} // namespace algorithms

} // namespace daal

#endif
