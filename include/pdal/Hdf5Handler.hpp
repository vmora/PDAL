/******************************************************************************
* Copyright (c) 2014, Connor Manning, connor@hobu.co
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#ifndef INCLUDED_PDAL_HDF5_HANDLER_HPP
#define INCLUDED_PDAL_HDF5_HANDLER_HPP

#include "H5Cpp.h"

#include <boost/scoped_ptr.hpp>
#include <boost/cstdint.hpp>

#include <pdal/pdal_error.hpp>

#include <vector>
#include <string>
#include <map>

namespace pdal
{

namespace hdf5
{
    struct Hdf5ColumnData
    {
        Hdf5ColumnData(const std::string& name, const H5::PredType predType)
            : name(name)
            , predType(predType)
        { }

        const std::string name;
        const H5::PredType predType;
    };
}

class hdf5_error : public pdal_error
{
public:
    hdf5_error(std::string const& msg)
        : pdal_error(msg)
    { }
};

class Hdf5Handler
{
public:
    Hdf5Handler();
    ~Hdf5Handler() { }

    void initialize(
            const std::string& filename,
            const std::vector<hdf5::Hdf5ColumnData>& columns);
    void close();

    boost::uint64_t getNumPoints() const;

    void getColumnEntries(
            void* data,
            const std::string& dataSetName,
            boost::uint64_t numEntries,
            boost::uint64_t offset) const;

private:
    struct ColumnData
    {
        ColumnData(
                H5::PredType predType,
                H5::DataSet dataSet,
                H5::DataSpace dataSpace)
            : predType(predType)
            , dataSet(dataSet)
            , dataSpace(dataSpace)
        { }

        ColumnData(H5::PredType predType)
            : predType(predType)
            , dataSet()
            , dataSpace()
        { }

        H5::PredType predType;
        H5::DataSet dataSet;
        H5::DataSpace dataSpace;
    };

    unsigned long long getColumnNumEntries(const std::string& dataSetName) const;
    const ColumnData& getColumnData(const std::string& dataSetName) const;

    boost::scoped_ptr<H5::H5File> m_h5File;
    boost::uint64_t m_numPoints;

    std::map<std::string, ColumnData> m_columnDataMap;
};

} // namespace pdal

#endif // INCLUDED_PDAL_HDF5_HANDLER_HPP

