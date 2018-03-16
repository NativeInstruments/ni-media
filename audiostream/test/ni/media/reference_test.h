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

#pragma once

#include <ni/media/audio/iotools.h>
#include <ni/media/pcm/iterator.h>
#include <ni/media/pcm/range/converted.h>

#include <ni/media/signals.h>
#include <ni/media/statistics.h>
#include <ni/media/test_helper.h>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/range/algorithm/generate.hpp>
#include <boost/tokenizer.hpp>

#include <algorithm>
#include <map>
#include <string>
#include <type_traits>
#include <vector>


namespace detail
{

//----------------------------------------------------------------------------------------------------------------------

inline audio::ifstream_info parse_reference_file( const std::string& file_name )
{
    audio::ifstream_info info;

    const auto separator = boost::char_separator<char>{"."};
    const auto tokens    = boost::tokenizer<boost::char_separator<char>>{file_name, separator};

    auto it = tokens.begin();
    if ( it == tokens.end() )
        return info;
    // first token is the generator - ignore

    ++it;
    if ( it == tokens.end() )
        return info;
    try
    {
        info.num_channels( std::stoi( *it ) );
    }
    catch ( const std::invalid_argument& )
    {
    }

    ++it;
    if ( it == tokens.end() )
        return info;
    try
    {
        info.sample_rate( std::stoi( *it ) );
    }
    catch ( const std::invalid_argument& )
    {
    }

    ++it;
    if ( it == tokens.end() )
        return info;
    try
    {
        info.num_frames( std::stoi( *it ) );
    }
    catch ( const std::invalid_argument& )
    {
    }

    return info;
}

//----------------------------------------------------------------------------------------------------------------------

inline std::string parse_generator( const std::string& file_name )
{
    const auto separator = boost::char_separator<char>{"."};
    const auto tokens    = boost::tokenizer<boost::char_separator<char>>{file_name, separator};

    if ( tokens.begin() != tokens.end() )
        return *tokens.begin();

    return {};
}

//----------------------------------------------------------------------------------------------------------------------

inline std::map<std::string, sin440_mod> generator_map( size_t num_channels, float sample_rate )
{
    return {{"sin440", sin440_mod( num_channels, sample_rate, 0 )},
            {"sin_mod440", sin440_mod( num_channels, sample_rate, 1 )}};
}

//----------------------------------------------------------------------------------------------------------------------

inline boost::optional<sin440_mod> get_generator( const std::string& generator_name,
                                                  size_t             num_channels,
                                                  size_t             sample_rate )
{
    const auto map = generator_map( num_channels, (float) sample_rate );
    const auto it  = map.find( generator_name );
    if ( it != map.end() )
        return it->second;

    return boost::none;
}

} // detail

//----------------------------------------------------------------------------------------------------------------------

template <typename Stream>
void reference_test( Stream&& stream, const boost::filesystem::path& file )
{
    const auto file_name = file.filename().string();

    auto info_expected = detail::parse_reference_file( file_name );
    auto info_actual   = stream.info();

    EXPECT_EQ( info_expected.num_channels(), info_actual.num_channels() );
    EXPECT_EQ( info_expected.sample_rate(), info_actual.sample_rate() );
    EXPECT_EQ( info_expected.num_frames(), info_actual.num_frames() );

    auto generator_name = detail::parse_generator( file_name );
    auto generator = detail::get_generator( generator_name, info_actual.num_channels(), info_actual.sample_rate() );

    ASSERT_TRUE( generator != boost::none ) << "Invalid generator: " << generator_name;

    auto samples = std::vector<float>( info_actual.num_samples() );
    stream >> samples;

    auto signal = std::vector<float>( info_actual.num_samples() );
    boost::generate( signal, *generator );

    EXPECT_EQ( samples.size(), signal.size() );

    EXPECT_GT( xcorr( samples, signal ), 0.9997 );

    EXPECT_LT( mean_difference( signal, samples ), 0.005 );
}
