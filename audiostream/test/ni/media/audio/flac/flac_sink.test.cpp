//
// Copyright (c) 2017 Native Instruments GmbH, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <ni/media/audio/ifstream.h>

#include <ni/media/audio/ofstream.h>
#include <ni/media/reference_test.h>
#include <ni/media/sink_test.h>


class flac_sink_test : public sink_test
{
};

//----------------------------------------------------------------------------------------------------------------------

TEST_P( flac_sink_test, ofstream )
{
    auto ifs = audio::ifstream( input_name() );

    audio::ofstream_info info;
    info.format( ifs.info().format() );
    info.num_channels( ifs.info().num_channels() );
    info.sample_rate( ifs.info().sample_rate() );

    {
        audio::ofstream ofs( output_name(), info );
        auto            buffer = std::vector<double>( ifs.info().num_samples() );
        ifs >> buffer;
        ofs << buffer;
    }

    reference_test( audio::ifstream( output_name() ), output_name() );
}

//----------------------------------------------------------------------------------------------------------------------


INSTANTIATE_TEST_SUITE_P( reference_test,
                          flac_sink_test,
                          reference_files( audio::ifstream_info::container_type::flac ) );
