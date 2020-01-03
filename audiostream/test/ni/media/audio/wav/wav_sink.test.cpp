//
// Copyright (c) 2017-2019 Native Instruments GmbH, Berlin
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
#include <ni/media/audio/wav/wav_ofstream.h>
#include <ni/media/reference_test.h>

#include <ni/media/sink_test.h>

//----------------------------------------------------------------------------------------------------------------------

class wav_sink_test : public sink_test
{
};

//----------------------------------------------------------------------------------------------------------------------

TEST_P( wav_sink_test, ofstream )
{
    auto is = audio::ifstream( input_name() );

    audio::wav_ofstream_info info;
    info.format( is.info().format() );
    info.num_channels( is.info().num_channels() );
    info.sample_rate( is.info().sample_rate() );

    {
        audio::ofstream os( output_name(), info );
        auto            buffer = std::vector<double>( is.info().num_samples() );
        is >> buffer;
        os << buffer;
    }

    reference_test( audio::ifstream( output_name() ), output_name() );
}

INSTANTIATE_TEST_CASE_P( reference_test, wav_sink_test, reference_files( audio::ifstream_info::container_type::wav ) );

//----------------------------------------------------------------------------------------------------------------------
