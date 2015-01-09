/******************************************************************************
* Copyright (c) 2014, Howard Butler (howard@hobu.co)
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


#include <boost/algorithm/string.hpp>

#ifdef PDAL_HAVE_LAZPERF
#include <laz-perf/common/common.hpp>
#include <laz-perf/compressor.hpp>
#include <laz-perf/decompressor.hpp>

#include <laz-perf/encoder.hpp>
#include <laz-perf/decoder.hpp>
#include <laz-perf/formats.hpp>
#include <laz-perf/las.hpp>
#endif

#include <pdal/Dimension.hpp>
#include <pdal/PointContext.hpp>
#include <pdal/PointBuffer.hpp>
#include <pdal/Compression.hpp>
#include <pdal/Charbuf.hpp>



#include <map>
#include <vector>

namespace pdal
{


namespace compression
{


namespace CompressionType
{

enum Enum
{
    None = 0,
    Ght = 1,
    Dimensional = 2,
    Lazperf = 3,
    Unknown = 256
};

} // namespace CompressionType


template <typename CompressionStream> inline void Compress(std::vector<Dimension> dims,
             size_t pointSize,
              const char* start_pos,
              const char* end_pos,
              CompressionStream& output,
              CompressionType::Enum ctype,
              PointId start,
              PointId end)
{

#ifdef PDAL_HAVE_LAZPERF
    using namespace laszip;
    using namespace laszip::formats;

    typedef encoders::arithmetic<CompressionStream> EncoderType;

    EncoderType encoder(output);
    auto compressor = make_dynamic_compressor(encoder);
    for (auto di = dims.begin(); di != dims.end(); ++di)
    {
        dimension::Interpretation t = di->getInterpretation();
        size_t s = di->getByteSize();
        if (t == dimension::SignedInteger)
        {
            if (s == 4)
            {
                compressor->template add_field<int>();
            }
            else if (s == 2)
            {
                compressor->template add_field<short>();
            }
            else if (s == 1)
            {
                compressor->template add_field<char>();
            }
            else if (s == 8)
            {
                compressor->template add_field<int>();
                compressor->template add_field<int>();
            }
            else
                throw pdal_error("Dimension is signed type but size != 8, 4, 2, 1!");
        }
        else if (t == dimension::UnsignedInteger)
        {
            if (s == 4)
            {
                compressor->template add_field<unsigned int>();
            }
            else if (s == 2)
            {
                compressor->template add_field<unsigned short>();
            }
            else if (s == 1)
            {
                compressor->template add_field<unsigned char>();
            }
            else if (s == 8)
            {
                compressor->template add_field<unsigned int>();
                compressor->template add_field<unsigned int>();
            }
            else
                throw pdal_error("Dimension is type Unsigned but size != 8, 4, 2, 1!");
        }
        else if (t == dimension::Float)
        {
            if (s == 8)
            {
                compressor->template add_field<unsigned int>();
                compressor->template add_field<unsigned int>();
//                 compressor->template add_field<las::gpstime>();
            }
            else if (s == 4)
            {
                compressor->template add_field<int>();
            }
            else
                throw pdal_error("Dimension is type Float but size != 8 or size!= 4!");
        }
        else
        {

            std::ostringstream oss;
            oss << "Unhandled compression for dimension of type '" << di->getInterpretationName() << "' for dimension '" << di->getName() << "'";
            throw pdal_error(oss.str());
        }

    }

    const char* pos = start_pos;
    while (pos != end_pos)
    {
        compressor->compress(pos);
        pos += pointSize;
    }

    encoder.done();
#endif
}



template <typename CompressionStream> inline std::vector<uint8_t> Decompress(Schema* schema,
                                                                       CompressionStream& strm,
                                                                       size_t howMany,
                                                                       CompressionType::Enum ctype)
{
    std::vector<uint8_t> output;
#ifdef PDAL_HAVE_LAZPERF
    using namespace laszip;
    using namespace laszip::formats;

    typedef decoders::arithmetic<CompressionStream> DecoderType;


    DecoderType decoder(strm);
    auto decompressor = make_dynamic_decompressor(decoder);
    schema::index_by_index const& dims = schema->getDimensions().get<schema::index>();

    for (auto di = dims.begin(); di != dims.end(); ++di)
    {
        dimension::Interpretation t = di->getInterpretation();
        size_t s = di->getByteSize();
        if (t == dimension::SignedInteger)
        {
            if (s == 4)
            {
                decompressor->template add_field<int>();
            }
            else if (s == 2)
            {
                decompressor->template add_field<short>();
            }
            else if (s == 1)
            {
                decompressor->template add_field<char>();
            }
            else if (s == 8)
            {
                decompressor->template add_field<int>();
                decompressor->template add_field<int>();
            }
            else
                throw pdal_error("Dimension is type signed but size != 8, 4, 2, 1!");
        }
        else if (t == dimension::UnsignedInteger)
        {
            if (s == 4)
            {
                decompressor->template add_field<unsigned int>();
            }
            else if (s == 2)
            {
                decompressor->template add_field<unsigned short>();
            }
            else if (s == 1)
            {
                decompressor->template add_field<unsigned char>();
            }
            else if (s == 8)
            {
                decompressor->template add_field<unsigned int>();
                decompressor->template add_field<unsigned int>();
            }
            else
                throw pdal_error("Dimension is type Unsigned but size != 8, 4, 2, 1!");
        }
        else if (t == dimension::Float)
        {
            if (s == 8)
            {
                decompressor->template add_field<unsigned int>();
                decompressor->template add_field<unsigned int>();
//                 decompressor->template add_field<las::gpstime>();
            }
            else if (s == 4)
            {
                decompressor->template add_field<int>();
            }
            else
                throw pdal_error("Dimension is type Double but size != 8!");
        }
        else
        {

            std::ostringstream oss;
            oss << "Unhandled compression for dimension of type '" << di->getInterpretationName() << "' for dimension '" << di->getName() << "'";
            throw pdal_error(oss.str());
        }

    }

    size_t point_size = schema->getByteSize();
    size_t byte_len = howMany * point_size;
    output.resize(byte_len);

    uint8_t* pos = output.data();

    uint8_t* end_pos = pos + (byte_len);
    while (pos != end_pos)
    {
        decompressor->decompress((char*)pos);
        pos+=point_size;
    }

#endif

    return output;
//     Charbuf charstreambuf(output, output.size());
//     std::istream istrm(&charstreambuf);
//
//     PointBufferPtr b = PointBufferPtr(new PointBuffer(istrm, ctx, 0, howMany));
//     return b;
//

}

} // compression
} // namespace pdal


