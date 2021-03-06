/******************************************************************************
 * Copyright (c) 2015, Bradley J Chambers (brad.chambers@gmail.com)
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

#pragma once

#include <pdal/Filter.hpp>

#include <memory>
#include <map>
#include <string>

namespace pdal
{

class Options;
class PointBuffer;
class PointContext;

typedef std::shared_ptr<PointBuffer> PointBufferPtr;
typedef PointContext PointContextRef;

struct Range
{
    double min;
    double max;
};

#define RANGEDOCS "Pass only points given a dimension/range."
class PDAL_DLL RangeFilter : public pdal::Filter
{
public:
    SET_STAGE_NAME("filters.range", RANGEDOCS)
    SET_STAGE_LINK("http://pdal.io/stages/filters.range.html")

    RangeFilter() : Filter()
    {}

private:
    std::map<std::string, Range> m_name_map;
    std::map<Dimension::Id::Enum, Range> m_dimensions_map;

    virtual void processOptions(const Options&options);
    virtual void ready(PointContextRef ctx);
    virtual PointBufferSet run(PointBufferPtr buf);

    RangeFilter& operator=(const RangeFilter&); // not implemented
    RangeFilter(const RangeFilter&); // not implemented
};

} // namespace pdal
