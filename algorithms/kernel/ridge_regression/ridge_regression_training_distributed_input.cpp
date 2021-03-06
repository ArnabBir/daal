/* file: ridge_regression_training_distributed_input.cpp */
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
//++
//  Implementation of ridge regression algorithm classes.
//--
*/

#include "algorithms/ridge_regression/ridge_regression_training_types.h"

using namespace daal::data_management;
using namespace daal::services;

namespace daal
{
namespace algorithms
{
namespace ridge_regression
{
namespace training
{
namespace interface1
{

template class DistributedInput<step2Master>;

DistributedInput<step2Master>::DistributedInput() : daal::algorithms::Input(lastStep2MasterInputId + 1)
{
    Argument::set(partialModels, DataCollectionPtr(new DataCollection()));
};

DistributedInput<step2Master>::DistributedInput(const DistributedInput<step2Master>& other) : daal::algorithms::Input(other){}

/**
 * Gets an input object for ridge regression model-based training in the second step of the distributed processing mode
 * \param[in] id    Identifier of the input object
 * \return          %Input object that corresponds to the given identifier
 */
DataCollectionPtr DistributedInput<step2Master>::get(Step2MasterInputId id) const
{
    return staticPointerCast<DataCollection, SerializationIface>(Argument::get(id));
}
/**
 * Sets an input object for ridge regression model-based training in the second step of the distributed processing mode
 * \param[in] id    Identifier of the input object
 * \param[in] ptr   %Input object
 */
void DistributedInput<step2Master>::set(Step2MasterInputId id, const DataCollectionPtr & ptr)
{
    Argument::set(id, ptr);
}
/**
 Adds an input object for ridge regression model-based training in the second step of the distributed processing mode
 * \param[in] id      Identifier of the input object
 * \param[in] partialResult   %Input object
 */
void DistributedInput<step2Master>::add(Step2MasterInputId id, const PartialResultPtr & partialResult)
{
    const DataCollectionPtr collection = staticPointerCast<DataCollection, SerializationIface>(Argument::get(id));
    collection->push_back(staticPointerCast<SerializationIface, ridge_regression::Model>(partialResult->get(training::partialModel)));
}
/**
 * Returns the number of columns in the input data set
 * \return Number of columns in the input data set
 */
size_t DistributedInput<step2Master>::getNumberOfFeatures() const
{
    const DataCollectionPtr partialModelsCollection = static_cast<DataCollectionPtr >(get(partialModels));
    if (partialModelsCollection->size() == 0) { return 0; }
    const ridge_regression::Model * const partialModel = static_cast<const daal::algorithms::ridge_regression::Model *>(((*partialModelsCollection)[0]).get());
    return partialModel->getNumberOfFeatures();
}
/**
 * Returns the number of dependent variables
 * \return Number of dependent variables
 */
size_t DistributedInput<step2Master>::getNumberOfDependentVariables() const
{
    const DataCollectionPtr partialModelsCollection = static_cast<DataCollectionPtr >(get(partialModels));
    if (partialModelsCollection->size() == 0) { return 0; }
    const ridge_regression::Model * const partialModel = static_cast<const daal::algorithms::ridge_regression::Model *>(((*partialModelsCollection)[0]).get());
    return partialModel->getNumberOfResponses();
}
/**
 * Checks an input object for ridge regression model-based training in the second step
 * of the distributed processing mode
 */
services::Status DistributedInput<step2Master>::check(const daal::algorithms::Parameter *parameter, int method) const
{
    DataCollectionPtr collection = DataCollection::cast(Argument::get(partialModels));
    DAAL_CHECK(collection, ErrorNullInputDataCollection);

    size_t nBlocks = collection->size();
    DAAL_CHECK(nBlocks > 0, ErrorIncorrectNumberOfInputNumericTables);

    DAAL_CHECK((*collection)[0], ErrorNullModel);
    ridge_regression::ModelPtr firstPartialModel = ridge_regression::Model::cast((*collection)[0]);
    DAAL_CHECK(firstPartialModel, ErrorIncorrectElementInPartialResultCollection);

    size_t nBeta = firstPartialModel->getNumberOfBetas();
    size_t nResponses = firstPartialModel->getNumberOfResponses();

    services::Status s;

    for(size_t i = 0; i < nBlocks; i++)
    {
        DAAL_CHECK((*collection)[i], ErrorNullModel);
        ridge_regression::ModelPtr partialModel = ridge_regression::Model::cast((*collection)[i]);

        DAAL_CHECK(partialModel, ErrorIncorrectElementInPartialResultCollection);

        DAAL_CHECK_STATUS(
            s, ridge_regression::checkModel(partialModel.get(), *parameter, nBeta, nResponses, method));
    }

    return s;
}

} // namespace interface1
} // namespace training
} // namespace ridge_regression
} // namespace algorithms
} // namespace daal
