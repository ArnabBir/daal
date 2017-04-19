/* file: homogen_numeric_table.h */
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
//  Implementation of a homogeneous numeric table.
//--
*/


#ifndef __HOMOGEN_NUMERIC_TABLE_H__
#define __HOMOGEN_NUMERIC_TABLE_H__

#include "data_management/data/numeric_table.h"
#include "services/daal_memory.h"
#include "services/daal_defines.h"

namespace daal
{
namespace data_management
{

namespace interface1
{
/**
 * @ingroup numeric_tables
 * @{
 */
/**
 *  <a name="DAAL-CLASS-DATA_MANAGEMENT__HOMOGENNUMERICTABLE"></a>
 *  \brief Class that provides methods to access data stored as a contiguous array
 *  of homogeneous feature vectors. Table rows contain feature vectors,
 *  and columns contain values of individual features.
 *  \tparam DataType Defines the underlying data type that describes a Numeric Table
 */
template<typename DataType = double>
class HomogenNumericTable : public NumericTable
{
public:
    DAAL_CAST_OPERATOR(HomogenNumericTable)
    /**
     *  Typedef that stores a datatype used for template instantiation
     */
    typedef DataType baseDataType;

public:
    /**
     *  Constructor for an empty Numeric Table with a predefined NumericTableDictionary
     *  \param[in]  ddict   Pointer to the predefined NumericTableDictionary
     */
    HomogenNumericTable( NumericTableDictionary *ddict ):
        NumericTable(ddict), _ptr(0)
    {
        _layout = aos;
    }

    /**
     *  Constructor for an empty Numeric Table with a predefined NumericTableDictionary
     *  \param[in]  ddictForHomogenNumericTable   Pointer to the predefined NumericTableDictionary
     */
    HomogenNumericTable( NumericTableDictionaryPtr ddictForHomogenNumericTable ):
        NumericTable(ddictForHomogenNumericTable), _ptr(0)
    {
        _layout = aos;
    }

    /**
     *  Constructor for a Numeric Table with user-allocated memory
     *  \param[in]  ptr            Pointer to and an array with a homogeneous data set
     *  \param[in]  nColumns       Number of columns in the table
     *  \param[in]  nRows          Number of rows in the table
     *  \DAAL_DEPRECATED
     */
    HomogenNumericTable( DataType *const ptr = 0, size_t nColumns = 0, size_t nRows = 0 ):
        NumericTable( nColumns, nRows ), _ptr(0)
    {
        _layout = aos;
        this->_status |= setArray( ptr, nRows );

        NumericTableFeature df;
        df.setType<DataType>();
        this->_status |= _ddict->setAllFeatures(df);
    }

    /**
     *  Constructor for a Numeric Table with user-allocated memory
     *  \param[in]  featuresEqual  Flag that makes all features in the Numeric Table Data Dictionary equal
     *  \param[in]  ptr            Pointer to and an array with a homogeneous data set
     *  \param[in]  nColumns       Number of columns in the table
     *  \param[in]  nRows          Number of rows in the table
     *  \DAAL_DEPRECATED
     */
    HomogenNumericTable( DictionaryIface::FeaturesEqual featuresEqual, DataType *const ptr = 0, size_t nColumns = 0, size_t nRows = 0):
        NumericTable( nColumns, nRows, featuresEqual ), _ptr(0)
    {
        _layout = aos;
        this->_status |= setArray( ptr, nRows );

        NumericTableFeature df;
        df.setType<DataType>();
        this->_status |= _ddict->setAllFeatures(df);
    }

    /**
     *  Constructor for a Numeric Table with user-allocated memory and filling the table with a constant
     *  \param[in]  ptr            Pointer to and an array with a homogeneous data set
     *  \param[in]  nColumns       Number of columns in the table
     *  \param[in]  nRows          Number of rows in the table
     *  \param[in]  constValue     Constant to initialize entries of the homogeneous numeric table
     *  \DAAL_DEPRECATED
     */
    HomogenNumericTable( DataType *const ptr, size_t nColumns, size_t nRows, const DataType &constValue ):
        NumericTable( nColumns, nRows ), _ptr(0)
    {
        _layout = aos;
        this->_status |= setArray( ptr, nRows );

        NumericTableFeature df;
        df.setType<DataType>();
        this->_status |= _ddict->setAllFeatures(df);
        this->_status |= assign(constValue);
    }

    /**
     *  Constructor for a Numeric Table with user-allocated memory and filling the table with a constant
     *  \param[in]  featuresEqual  Flag that makes all features in the Numeric Table Data Dictionary equal
     *  \param[in]  ptr            Pointer to and an array with a homogeneous data set
     *  \param[in]  nColumns       Number of columns in the table
     *  \param[in]  nRows          Number of rows in the table
     *  \param[in]  constValue     Constant to initialize entries of the homogeneous numeric table
     *  \DAAL_DEPRECATED
     */
    HomogenNumericTable( DictionaryIface::FeaturesEqual featuresEqual, DataType *const ptr, size_t nColumns, size_t nRows, const DataType &constValue ):
        NumericTable( nColumns, nRows, featuresEqual ), _ptr(0)
    {
        _layout = aos;
        this->_status |= setArray( ptr, nRows );

        NumericTableFeature df;
        df.setType<DataType>();
        this->_status |= _ddict->setAllFeatures(df);
        this->_status |= assign(constValue);
    }

    /**
     *  Constructor for a Numeric Table with memory allocation controlled via a flag
     *  \param[in]  nColumns                Number of columns in the table
     *  \param[in]  nRows                   Number of rows in the table
     *  \param[in]  memoryAllocationFlag    Flag that controls internal memory allocation for data in the numeric table
     *  \DAAL_DEPRECATED
     */
    HomogenNumericTable( size_t nColumns, size_t nRows, AllocationFlag memoryAllocationFlag ):
        NumericTable( nColumns, nRows ), _ptr(0)
    {
        _layout = aos;

        NumericTableFeature df;
        df.setType<DataType>();
        this->_status |= _ddict->setAllFeatures(df);

        if( memoryAllocationFlag == doAllocate ) this->_status |= allocateDataMemoryImpl();
    }

    /**
     *  Constructor for a Numeric Table with memory allocation controlled via a flag
     *  \param[in]  featuresEqual           Flag that makes all features in the Numeric Table Data Dictionary equal
     *  \param[in]  nColumns                Number of columns in the table
     *  \param[in]  nRows                   Number of rows in the table
     *  \param[in]  memoryAllocationFlag    Flag that controls internal memory allocation for data in the numeric table
     *  \DAAL_DEPRECATED
     */
    HomogenNumericTable( DictionaryIface::FeaturesEqual featuresEqual, size_t nColumns, size_t nRows, AllocationFlag memoryAllocationFlag ):
        NumericTable( nColumns, nRows, featuresEqual ), _ptr(0)
    {
        _layout = aos;

        NumericTableFeature df;
        df.setType<DataType>();
        this->_status |= _ddict->setAllFeatures(df);

        if( memoryAllocationFlag == doAllocate ) this->_status |= allocateDataMemoryImpl();
    }

    /**
     *  Constructor for a Numeric Table with memory allocation controlled via a flag and filling the table with a constant
     *  \param[in]  nColumns                Number of columns in the table
     *  \param[in]  nRows                   Number of rows in the table
     *  \param[in]  memoryAllocationFlag    Flag that controls internal memory allocation for data in the numeric table
     *  \param[in]  constValue              Constant to initialize entries of the homogeneous numeric table
     *  \DAAL_DEPRECATED
     */
    HomogenNumericTable( size_t nColumns, size_t nRows, NumericTable::AllocationFlag memoryAllocationFlag,
                         const DataType &constValue ):
        NumericTable( nColumns, nRows ), _ptr(0)
    {
        _layout = aos;

        NumericTableFeature df;
        df.setType<DataType>();

        this->_status |= _ddict->setAllFeatures(df);

        if( memoryAllocationFlag == doAllocate ) this->_status |= allocateDataMemoryImpl();

        this->_status |= assign(constValue);
    }

    /**
     *  Constructor for a Numeric Table with memory allocation controlled via a flag and filling the table with a constant
     *  \param[in]  featuresEqual           Flag that makes all features in the Numeric Table Data Dictionary equal
     *  \param[in]  nColumns                Number of columns in the table
     *  \param[in]  nRows                   Number of rows in the table
     *  \param[in]  memoryAllocationFlag    Flag that controls internal memory allocation for data in the numeric table
     *  \param[in]  constValue              Constant to initialize entries of the homogeneous numeric table
     *  \DAAL_DEPRECATED
     */
    HomogenNumericTable( DictionaryIface::FeaturesEqual featuresEqual, size_t nColumns, size_t nRows, NumericTable::AllocationFlag memoryAllocationFlag,
                         const DataType &constValue ):
        NumericTable( nColumns, nRows, featuresEqual ), _ptr(0)
    {
        _layout = aos;

        NumericTableFeature df;
        df.setType<DataType>();

        this->_status |= _ddict->setAllFeatures(df);

        if( memoryAllocationFlag == doAllocate ) { this->_status |= allocateDataMemoryImpl(); }

        this->_status |= assign(constValue);
    }

    virtual ~HomogenNumericTable()
    {
        freeDataMemoryImpl();
    }

    virtual int getSerializationTag() DAAL_C11_OVERRIDE
    {
        return data_feature_utils::getIndexNumType<DataType>() + SERIALIZATION_HOMOGEN_NT_ID;
    }

    /**
     *  Returns a pointer to a data set registered in a homogeneous Numeric Table
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
        freeDataMemoryImpl();

        _ptr = ptr;

        if(_ptr)
        {
            _memStatus = userAllocated;
        }
        else
        {
            _memStatus = notAllocated;
        }
        return services::Status();
    }

    services::Status setArray( DataType *const ptr, size_t nRows )
    {
        freeDataMemoryImpl();

        _ptr       = ptr;
        _obsnum    = nRows;

        if(_ptr)
        {
            _memStatus = userAllocated;
        }
        else
        {
            _memStatus = notAllocated;
        }
        return services::Status();
    }

    /**
     *  Fills a numeric table with a constant
     *  \param[in]  constValue  Constant to initialize entries of the homogeneous numeric table
     */
    services::Status assign(const DataType &constValue)
    {
        if( _memStatus == notAllocated )
            return services::Status(services::ErrorEmptyHomogenNumericTable);

        size_t nColumns = getNumberOfColumns();
        size_t nRows    = getNumberOfRows();

        size_t i;
        for( i = 0; i < nColumns * nRows; i++ )
        {
            _ptr[i] = constValue;
        }
        return services::Status();
    }

    /**
     *  Returns a pointer to the i-th row of a data set
     *  \param[in]  i  Index of the row
     *  \return Pointer to the i-th row
     */
    DataType *operator[](size_t i)
    {
        size_t nColumns = getNumberOfColumns();
        return _ptr + i * nColumns;
    }

    services::Status getBlockOfRows(size_t vector_idx, size_t vector_num, ReadWriteMode rwflag, BlockDescriptor<double> &block) DAAL_C11_OVERRIDE
    {
        return getTBlock<double>(vector_idx, vector_num, rwflag, block);
    }
    services::Status getBlockOfRows(size_t vector_idx, size_t vector_num, ReadWriteMode rwflag, BlockDescriptor<float> &block) DAAL_C11_OVERRIDE
    {
        return getTBlock<float>(vector_idx, vector_num, rwflag, block);
    }
    services::Status getBlockOfRows(size_t vector_idx, size_t vector_num, ReadWriteMode rwflag, BlockDescriptor<int> &block) DAAL_C11_OVERRIDE
    {
        return getTBlock<int>(vector_idx, vector_num, rwflag, block);
    }

    services::Status releaseBlockOfRows(BlockDescriptor<double> &block) DAAL_C11_OVERRIDE
    {
        return releaseTBlock<double>(block);
    }
    services::Status releaseBlockOfRows(BlockDescriptor<float> &block) DAAL_C11_OVERRIDE
    {
        return releaseTBlock<float>(block);
    }
    services::Status releaseBlockOfRows(BlockDescriptor<int> &block) DAAL_C11_OVERRIDE
    {
        return releaseTBlock<int>(block);
    }

    services::Status getBlockOfColumnValues(size_t feature_idx, size_t vector_idx, size_t value_num,
                                ReadWriteMode rwflag, BlockDescriptor<double> &block) DAAL_C11_OVERRIDE
    {
        return getTFeature<double>(feature_idx, vector_idx, value_num, rwflag, block);
    }
    services::Status getBlockOfColumnValues(size_t feature_idx, size_t vector_idx, size_t value_num,
                                ReadWriteMode rwflag, BlockDescriptor<float> &block) DAAL_C11_OVERRIDE
    {
        return getTFeature<float>(feature_idx, vector_idx, value_num, rwflag, block);
    }
    services::Status getBlockOfColumnValues(size_t feature_idx, size_t vector_idx, size_t value_num,
                                ReadWriteMode rwflag, BlockDescriptor<int> &block) DAAL_C11_OVERRIDE
    {
        return getTFeature<int>(feature_idx, vector_idx, value_num, rwflag, block);
    }

    services::Status releaseBlockOfColumnValues(BlockDescriptor<double> &block) DAAL_C11_OVERRIDE
    {
        return releaseTFeature<double>(block);
    }
    services::Status releaseBlockOfColumnValues(BlockDescriptor<float> &block) DAAL_C11_OVERRIDE
    {
        return releaseTFeature<float>(block);
    }
    services::Status releaseBlockOfColumnValues(BlockDescriptor<int> &block) DAAL_C11_OVERRIDE
    {
        return releaseTFeature<int>(block);
    }

    void serializeImpl  (InputDataArchive  *archive) DAAL_C11_OVERRIDE
    {serialImpl<InputDataArchive, false>( archive );}

    void deserializeImpl(OutputDataArchive *archive) DAAL_C11_OVERRIDE
    {serialImpl<OutputDataArchive, true>( archive );}

protected:
    DataType *_ptr;

    services::Status allocateDataMemoryImpl(daal::MemType type = daal::dram) DAAL_C11_OVERRIDE
    {
        freeDataMemoryImpl();

        size_t size = getNumberOfColumns() * getNumberOfRows();

        if( size == 0 )
        {
            return services::Status(getNumberOfColumns() == 0 ? services::ErrorIncorrectNumberOfFeatures :
                services::ErrorIncorrectNumberOfObservations);
        }

        _ptr = (DataType *)daal::services::daal_malloc( size * sizeof(DataType) );
        if( _ptr == 0 )
            return services::Status(services::ErrorMemoryAllocationFailed);

        _memStatus = internallyAllocated;
        return services::Status();
    }

    void freeDataMemoryImpl() DAAL_C11_OVERRIDE
    {
        if( getDataMemoryStatus() == internallyAllocated )
        {
            daal::services::daal_free(_ptr);
        }

        _ptr = 0;
        _memStatus = notAllocated;
    }

    template<typename Archive, bool onDeserialize>
    void serialImpl( Archive *archive )
    {
        NumericTable::serialImpl<Archive, onDeserialize>( archive );

        if( onDeserialize )
        {
            allocateDataMemoryImpl();
        }

        size_t size = getNumberOfColumns() * getNumberOfRows();

        archive->set( _ptr, size );
    }

private:
    DataType *internal_getBlockOfRows( size_t idx )
    {
        size_t _featnum = _ddict->getNumberOfFeatures();
        return _ptr + _featnum * idx;
    }
    DataType *internal_getBlockOfRows( size_t idx, size_t feat_idx )
    {
        size_t _featnum = _ddict->getNumberOfFeatures();
        return _ptr + _featnum * idx + feat_idx;
    }

    template<typename T1, typename T2>
    void internal_repack( size_t p, size_t n, T1 *src, T2 *dst )
    {
        if( IsSameType<T1, T2>::value )
        {
            if( src != (T1 *)dst )
            {
                daal::services::daal_memcpy_s(dst, n * p * sizeof(T1), src, n * p * sizeof(T1));
            }
        }
        else
        {
            size_t i, j;

            for(i = 0; i < n; i++)
            {
                for(j = 0; j < p; j++)
                {
                    dst[i * p + j] = static_cast<T2>(src[i * p + j]);
                }
            }
        }
    }

    template<typename T1, typename T2>
    void internal_set_col_repack( size_t p, size_t n, T1 *src, T2 *dst )
    {
        size_t i;

        for(i = 0; i < n; i++)
        {
            dst[i * p] = static_cast<T2>(src[i]);
        }
    }

protected:

    template <typename T>
    services::Status getTBlock( size_t idx, size_t nrows, int rwFlag, BlockDescriptor<T> &block )
    {
        size_t ncols = getNumberOfColumns();
        size_t nobs = getNumberOfRows();
        block.setDetails( 0, idx, rwFlag );

        if (idx >= nobs)
        {
            block.resizeBuffer( ncols, 0 );
            return services::Status();
        }

        nrows = ( idx + nrows < nobs ) ? nrows : nobs - idx;

        if( IsSameType<T, DataType>::value )
        {
            block.setPtr( (T *)internal_getBlockOfRows(idx), ncols, nrows );
        }
        else
        {
            if( !block.resizeBuffer( ncols, nrows ) )
                return services::Status(services::ErrorMemoryAllocationFailed);

            if( rwFlag & (int)readOnly )
            {
                DataType *location = internal_getBlockOfRows(idx);
                internal_repack<DataType, T>( ncols, nrows, location, block.getBlockPtr() );
            }
        }
        return services::Status();
    }

    template <typename T>
    services::Status releaseTBlock( BlockDescriptor<T> &block )
    {
        if(block.getRWFlag() & (int)writeOnly)
        {
            DataType *location = internal_getBlockOfRows( block.getRowsOffset() );
            size_t ncols = getNumberOfColumns();
            internal_repack<T, DataType>(ncols, block.getNumberOfRows(), block.getBlockPtr(), location);
        }
        block.setDetails( 0, 0, 0 );
        return services::Status();
    }

    template <typename T>
    services::Status getTFeature(size_t feat_idx, size_t idx, size_t nrows, int rwFlag, BlockDescriptor<T> &block)
    {
        size_t ncols = getNumberOfColumns();
        size_t nobs = getNumberOfRows();
        block.setDetails( feat_idx, idx, rwFlag );

        if (idx >= nobs)
        {
            block.resizeBuffer( 1, 0 );
            return services::Status();
        }

        nrows = ( idx + nrows < nobs ) ? nrows : nobs - idx;

        if( ( IsSameType<T, DataType>::value ) && (ncols == 1) )
        {
            block.setPtr( (T *)internal_getBlockOfRows(idx), 1, nrows );
        }
        else
        {
            if( !block.resizeBuffer( 1, nrows ) )
                return services::Status(services::ErrorMemoryAllocationFailed);

            if( rwFlag & (int)readOnly )
            {
                DataType *location = internal_getBlockOfRows(idx, feat_idx);
                T *buffer = block.getBlockPtr();
                for (size_t i = 0; i < nrows; i++)
                {
                    buffer[i] = static_cast<T>(location[i * ncols]);
                }
            }
        }
        return services::Status();
    }

    template <typename T>
    services::Status releaseTFeature( BlockDescriptor<T> &block )
    {
        if (block.getRWFlag() & (int)writeOnly)
        {
            size_t ncols = getNumberOfColumns();
            DataType *location = internal_getBlockOfRows(block.getRowsOffset(), block.getColumnsOffset());
            internal_set_col_repack<T, DataType>(ncols, block.getNumberOfRows(), block.getBlockPtr(), location);
        }
        block.setDetails( 0, 0, 0 );
        return services::Status();
    }

    services::Status setNumberOfColumnsImpl(size_t ncol) DAAL_C11_OVERRIDE
    {
        if( _ddict->getNumberOfFeatures() != ncol )
        {
            _ddict->resetDictionary();
            _ddict->setNumberOfFeatures(ncol);

            NumericTableFeature df;
            df.setType<DataType>();
            _ddict->setAllFeatures(df);
        }
        return services::Status();
    }

};
/** @} */
} // namespace interface1
using interface1::HomogenNumericTable;

}
} // namespace daal
#endif
