/* file: kdtree_knn_classification_model.h */
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
//  Implementation of the class defining the K-Nearest Neighbors (kNN) classification model
//--
*/

#ifndef __KDTREE_KNN_CLASSIFICATION_MODEL_H__
#define __KDTREE_KNN_CLASSIFICATION_MODEL_H__

#include "algorithms/classifier/classifier_model.h"
#include "data_management/data/aos_numeric_table.h"
#include "data_management/data/soa_numeric_table.h"
#include "data_management/data/homogen_numeric_table.h"
#include "services/daal_strings.h"

namespace daal
{
namespace algorithms
{

/**
 * @defgroup kdtree_knn_classification k-Nearest Neighbors
 * \copydoc daal::algorithms::kdtree_knn_classification
 * @ingroup classification
 * @{
 */

/**
 * \brief Contains classes for KD-tree based kNN algorithm
 */
namespace kdtree_knn_classification
{

/**
 * <a name="DAAL-ENUM-ALGORITHMS__KDTREE_KNN_CLASSIFICATION__DATAUSEINMODEL"></a>
 * \brief The option to enable/disable an usage of the input dataset in kNN model
 */
enum DataUseInModel
{
    doNotUse = 0, /*!< The input data and labels will not be the component of the trained kNN model */
    doUse    = 1  /*!< The input data and labels will be the component of the trained kNN model */
};

/**
 * \brief Contains version 1.0 of the Intel(R) Data Analytics Acceleration Library (Intel(R) DAAL) interface.
 */
namespace interface1
{

/**
 * <a name="DAAL-STRUCT-ALGORITHMS__KDTREE_KNN_CLASSIFICATION__PARAMETER"></a>
 * \brief KD-tree based kNN algorithm parameters
 *
 * \snippet k_nearest_neighbors/kdtree_knn_classification_model.h Parameter source code
 */
/* [Parameter source code] */
struct DAAL_EXPORT Parameter : public daal::algorithms::classifier::Parameter
{
    /**
     *  Main constructor
     *  \param[in] nClasses             Number of classes
     *  \param[in] nNeighbors           Number of neighbors
     *  \param[in] randomSeed           Seed for random choosing elements from training dataset
     *  \param[in] dataUse              The option to enable/disable an usage of the input dataset in kNN model
     */
    Parameter(size_t nClasses = 2, size_t nNeighbors = 1, int randomSeed = 777, DataUseInModel dataUse = doNotUse)
        : daal::algorithms::classifier::Parameter(nClasses),
          k(nNeighbors),
          seed(randomSeed),
          dataUseInModel(dataUse)
    {}

    /**
     * Checks a parameter of the KD-tree based kNN algorithm
     */
    void check() const DAAL_C11_OVERRIDE
    {
        // Inherited.
        daal::algorithms::classifier::Parameter::check();

        DAAL_CHECK_EX(k >= 1, services::ErrorIncorrectParameter, services::ParameterName, kStr());
    }

    size_t k;                      /*!< Number of neighbors */
    int seed;                      /*!< Seed for random choosing elements from training dataset */
    DataUseInModel dataUseInModel; /*!< The option to enable/disable an usage of the input dataset in kNN model */
};
/* [Parameter source code] */

/**
 * <a name="DAAL-CLASS-ALGORITHMS__KDTREE_KNN_CLASSIFICATION__MODEL"></a>
 * \brief %Base class for models trained with the KD-tree based kNN algorithm
 *
 * \tparam modelFPType  Data type to store KD-tree based kNN model data, double or float
 *
 * \par References
 *      - Parameter class
 *      - \ref training::interface1::Batch "training::Batch" class
 *      - \ref prediction::interface1::Batch "prediction::Batch" class
 */
class DAAL_EXPORT Model : public daal::algorithms::classifier::Model
{
public:
    DAAL_DOWN_CAST_OPERATOR(Model, classifier::Model)

    DECLARE_SERIALIZABLE();
    /**
     * Empty constructor for deserialization
     */
    Model();

    virtual ~Model();


    class ModelImpl;

    /**
     * Returns actual model implementation
     * \return Model implementation
     */
    const ModelImpl *impl() const { return _impl; }

    /**
     * Returns actual model implementation
     * \return Model implementation
     */
    ModelImpl *impl() { return _impl; }

protected:
    void serializeImpl(data_management::InputDataArchive   *arch) DAAL_C11_OVERRIDE;

    void deserializeImpl(data_management::OutputDataArchive *arch) DAAL_C11_OVERRIDE;

private:
    ModelImpl *_impl;  /*!< Model implementation */
};

} // namespace interface1

using interface1::Parameter;
using interface1::Model;

} // namespace kdtree_knn_classification

/** @} */
} // namespace algorithms
} // namespace daal

#endif
