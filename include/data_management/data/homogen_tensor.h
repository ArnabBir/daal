/* file: homogen_tensor.h */
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
//  Declaration and implementation of the base class for numeric n-cubes.
//--
*/


#ifndef __HOMOGEN_TENSOR_H__
#define __HOMOGEN_TENSOR_H__

#include "services/daal_defines.h"
#include "data_management/data/tensor.h"

namespace daal
{
namespace data_management
{
namespace interface1
{
/**
 * @ingroup tensor
 * @{
 */
/**
 *  <a name="DAAL-CLASS-DATA_MANAGEMENT__HOMOGENTENSOR"></a>
 *  \brief Class that provides methods to access data stored as a contiguous array
 *  of homogeneous data in rows-major format.
 *  \tparam DataType Defines the underlying data type that describes a tensor
 */
template<typename DataType = double>
class DAAL_EXPORT HomogenTensor : public Tensor
{
public:
    DAAL_CAST_OPERATOR(HomogenTensor<DataType>)

    /**
     * Constructs homogeneous tensor with a user-defined data, number and sizes of dimensions
     * \param nDim     Number of dimensions
     * \param dimSizes Array with sizes for each dimension
     * \param data     Size of the byte array
     */
    HomogenTensor(size_t nDim = 0, const size_t *dimSizes = 0, DataType *data = 0) : Tensor(&_layout), _layout(services::Collection<size_t>(nDim, dimSizes))
    {
        _ptr = data;
        _allocatedSize = 0;

        if( data )
        {
            _allocatedSize = getSize();
            _memStatus = userAllocated;
        }

        if(!dimSizes)
        {
            this->_status |= services::ErrorNullParameterNotSupported;
            return;
        }
    }

    /**
     * Constructs homogeneous tensor with a user-defined data and dimensions
     * \param dims  Collection of tensor dimensions
     * \param data  Size of the byte array
     */
    HomogenTensor(const services::Collection<size_t> &dims, DataType *data);

    /**
     * Constructs homogeneous tensor with a user-defined data and layout
     * \param layout  Tensor layout
     * \param data    Size of the byte array
     */
    HomogenTensor(const TensorOffsetLayout &layout, DataType *data) : Tensor(&_layout), _layout(layout)
    {
        const services::Collection<size_t>& dims = layout.getDimensions();
        _ptr = data;
        _allocatedSize = 0;

        if( data )
        {
            _allocatedSize = getSize();
            _memStatus = userAllocated;
        }

        size_t nDim = dims.size();

        if(nDim == 0)
        {
            this->_status |= services::ErrorNullParameterNotSupported;
            return;
        }
    }

    /**
     * Constructs homogeneous tensor with a user-defined dimensions and memory allocation flag
     * \param dims                  Collection of tensor dimensions
     * \param memoryAllocationFlag  Flag specifying memory allocation
     */
    HomogenTensor(const services::Collection<size_t> &dims, AllocationFlag memoryAllocationFlag) : Tensor(&_layout),
        _allocatedSize(0), _ptr(0), _layout(dims)
    {
        if( memoryAllocationFlag == doAllocate )
        {
            this->_status |= allocateDataMemory();
        }
    }

    /**
     * Constructs homogeneous tensor with a user-defined dimensions, memory allocation flag and fill it with the constant values
     * \param dims                  Collection of tensor dimensions
     * \param memoryAllocationFlag  Flag specifying memory allocation
     * \param initValue             Constant value
     */
    HomogenTensor(const services::Collection<size_t> &dims, AllocationFlag memoryAllocationFlag, const DataType initValue):
        Tensor(&_layout), _allocatedSize(0), _ptr(0), _layout(dims)
    {
        if( memoryAllocationFlag == doAllocate )
        {
            this->_status |= allocateDataMemory();
            this->_status |= assign(initValue);
        }
    }

    /** \private */
    virtual ~HomogenTensor()
    {
        freeDataMemory();
    }

public:
    /**
     *  Returns a pointer to a data set registered in a homogeneous tensor
     *  \return Pointer to the data set
     */
    DataType *getArray() const
    {
        return _ptr;
    }

    /**
     *  Sets a pointer to a homogeneous data set
     *  \param[in] ptr Pointer to the data set in the homogeneous format
     */
    services::Status setArray( DataType *const ptr )
    {
        services::Status s;
        DAAL_CHECK_STATUS(s, freeDataMemory());
        if(!ptr)
        {
            return services::Status(services::ErrorNullParameterNotSupported);
        }
        _ptr = ptr;
        _memStatus = userAllocated;
        return s;
    }

    /**
     *  Returns a homogeneous tensor offset layout
     *  \return Layout
     */
    TensorOffsetLayout& getTensorLayout()
    {
        return _layout;
    }

    /**
     *  Creates default tensor layout
     *  \return Layout
     */
    virtual TensorOffsetLayout createDefaultSubtensorLayout() const DAAL_C11_OVERRIDE
    {
        return TensorOffsetLayout(_layout);
    }

    /**
     *  Creates raw tensor layout
     *  \return Layout
     */
    virtual TensorOffsetLayout createRawSubtensorLayout() const DAAL_C11_OVERRIDE
    {
        TensorOffsetLayout layout(_layout);
        layout.sortOffsets();
        return layout;
    }

    //the descriptions of the methods below are inherited from the base class
    virtual services::Status setDimensions(size_t nDim, const size_t *dimSizes) DAAL_C11_OVERRIDE
    {
        if(!dimSizes)
        {
            return services::Status(services::ErrorNullParameterNotSupported);
        }

        _layout = TensorOffsetLayout(services::Collection<size_t>(nDim, dimSizes));
        return services::Status();
    }

    virtual services::Status setDimensions(const services::Collection<size_t>& dimensions) DAAL_C11_OVERRIDE
    {
        if(!dimensions.size())
        {
            return services::Status(services::ErrorNullParameterNotSupported);
        }

        _layout = TensorOffsetLayout(dimensions);
        return services::Status();
    }

    virtual services::Status allocateDataMemory(daal::MemType type = daal::dram) DAAL_C11_OVERRIDE
    {
        services::Status s;
        DAAL_CHECK_STATUS(s, freeDataMemory());

        if( _memStatus != notAllocated )
        {
            /* Error is already reported by freeDataMemory() */
            return services::Status();
        }

        size_t size = getSize();

        if( size == 0 )
        {
            return services::Status();
        }

        _ptr = (DataType *)daal::services::daal_malloc( size * sizeof(DataType) );

        if( _ptr == 0 )
        {
            return services::Status(services::ErrorMemoryAllocationFailed);
        }

        _allocatedSize = getSize();
        _memStatus = internallyAllocated;
        return services::Status();
    }

    /**
     *  Fills a tensor with a constant value
     *  \param[in]  initValue  Constant value to initialize entries of the homogeneous tensor
     */
    services::Status assign(const DataType initValue)
    {
        size_t size = getSize();

        for(size_t i = 0; i < size; i++)
        {
            _ptr[i] = initValue;
        }
        return services::Status();
    }

    //the descriptions of the methods below are inherited from the base class
    virtual services::Status freeDataMemory() DAAL_C11_OVERRIDE
    {
        if( getDataMemoryStatus() == internallyAllocated && _allocatedSize > 0 )
        {
            daal::services::daal_free(_ptr);
        }

        _ptr = 0;
        _allocatedSize = 0;
        _memStatus = notAllocated;
        return services::Status();
    }

    services::Status getSubtensorEx(size_t fixedDims, const size_t *fixedDimNums, size_t rangeDimIdx, size_t rangeDimNum,
                        ReadWriteMode rwflag, SubtensorDescriptor<double> &block,
                        const TensorOffsetLayout& layout) DAAL_C11_OVERRIDE;
    services::Status getSubtensorEx(size_t fixedDims, const size_t *fixedDimNums, size_t rangeDimIdx, size_t rangeDimNum,
                        ReadWriteMode rwflag, SubtensorDescriptor<float> &block,
                        const TensorOffsetLayout& layout) DAAL_C11_OVERRIDE;
    services::Status getSubtensorEx(size_t fixedDims, const size_t *fixedDimNums, size_t rangeDimIdx, size_t rangeDimNum,
                        ReadWriteMode rwflag, SubtensorDescriptor<int> &block,
                        const TensorOffsetLayout& layout) DAAL_C11_OVERRIDE;

    services::Status getSubtensor(size_t fixedDims, const size_t* fixedDimNums, size_t rangeDimIdx, size_t rangeDimNum,
        ReadWriteMode rwflag, SubtensorDescriptor<double>& subtensor ) DAAL_C11_OVERRIDE
    {
        return getSubtensorEx(fixedDims, fixedDimNums, rangeDimIdx, rangeDimNum, rwflag, subtensor, _layout );
    }

    services::Status getSubtensor(size_t fixedDims, const size_t* fixedDimNums, size_t rangeDimIdx, size_t rangeDimNum,
        ReadWriteMode rwflag, SubtensorDescriptor<float>& subtensor ) DAAL_C11_OVERRIDE
    {
        return getSubtensorEx(fixedDims, fixedDimNums, rangeDimIdx, rangeDimNum, rwflag, subtensor, _layout );
    }

    services::Status getSubtensor(size_t fixedDims, const size_t* fixedDimNums, size_t rangeDimIdx, size_t rangeDimNum,
        ReadWriteMode rwflag, SubtensorDescriptor<int>& subtensor ) DAAL_C11_OVERRIDE
    {
        return getSubtensorEx(fixedDims, fixedDimNums, rangeDimIdx, rangeDimNum, rwflag, subtensor, _layout );
    }

    services::Status releaseSubtensor(SubtensorDescriptor<double> &block) DAAL_C11_OVERRIDE;
    services::Status releaseSubtensor(SubtensorDescriptor<float>  &block) DAAL_C11_OVERRIDE;
    services::Status releaseSubtensor(SubtensorDescriptor<int>    &block) DAAL_C11_OVERRIDE;

    DAAL_DEPRECATED_VIRTUAL virtual services::SharedPtr<Tensor> getSampleTensor(size_t firstDimIndex) DAAL_C11_OVERRIDE
    {
        services::Collection<size_t> newDims = getDimensions();
        if(!_ptr || newDims.size() == 0 || newDims[0] <= firstDimIndex) { return services::SharedPtr<Tensor>(); }
        newDims[0] = 1;
        const size_t *_dimOffsets = &((_layout.getOffsets())[0]);
        return services::SharedPtr<Tensor>(new HomogenTensor<DataType>(newDims, _ptr + _dimOffsets[0]*firstDimIndex));
    }

    virtual int getSerializationTag() DAAL_C11_OVERRIDE
    {
        return data_feature_utils::getIndexNumType<DataType>() + SERIALIZATION_HOMOGEN_TENSOR_ID;
    }

protected:
    void serializeImpl  (InputDataArchive  *archive) DAAL_C11_OVERRIDE
    {serialImpl<InputDataArchive, false>( archive );}

    void deserializeImpl(OutputDataArchive *archive) DAAL_C11_OVERRIDE
    {serialImpl<OutputDataArchive, true>( archive );}

    template<typename Archive, bool onDeserialize>
    void serialImpl( Archive *archive )
    {
        Tensor::serialImpl<Archive, onDeserialize>( archive );

        archive->setObj( &_layout );

        bool isAllocated = (_memStatus != notAllocated);
        archive->set( isAllocated );

        if( onDeserialize )
        {
            freeDataMemory();

            if( isAllocated )
            {
                allocateDataMemory();
            }
        }

        if(_memStatus != notAllocated)
        {
            archive->set( _ptr, getSize() );
        }
    }

private:
    template <typename T>
    services::Status getTSubtensor( size_t fixedDims, const size_t *fixedDimNums, size_t rangeDimIdx, size_t rangeDimNum, int rwFlag,
                        SubtensorDescriptor<T> &block, const TensorOffsetLayout& layout );
    template <typename T>
    services::Status releaseTSubtensor( SubtensorDescriptor<T> &block );

private:
    DataType *_ptr;
    size_t    _allocatedSize;
    TensorOffsetLayout _layout;
};
/** @} */

}
using interface1::HomogenTensor;

}
} // namespace daal

#endif
