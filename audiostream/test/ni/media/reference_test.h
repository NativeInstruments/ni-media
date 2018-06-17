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

#include <ni/media/generators.h>
#include <ni/media/statistics.h>
#include <ni/media/test_helper.h>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/range/algorithm/generate.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/tokenizer.hpp>

#include <algorithm>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace detail
{

//----------------------------------------------------------------------------------------------------------------------

inline auto format_to_string( const pcm::format& fmt )
{
    std::ostringstream tmp;
    tmp << fmt;
    return tmp.str();
}

//----------------------------------------------------------------------------------------------------------------------

inline auto parse_reference_file( const std::string& file_name )
{
    const auto separator = boost::char_separator<char>{"."};
    const auto tokens    = boost::tokenizer<boost::char_separator<char>>{file_name, separator};
    return boost::copy_range<std::vector<std::string>>( tokens );
}

template <class T>
inline constexpr auto epsilon( const pcm::format& fmt )
{
    if ( fmt.number() == pcm::floating_point && fmt.bitwidth() == pcm::_32bit )
        return static_cast<T>( std::numeric_limits<float>::epsilon() );
    else if ( fmt.number() == pcm::floating_point && fmt.bitwidth() == pcm::_64bit )
        return static_cast<T>( std::numeric_limits<double>::epsilon() );
    else if ( fmt.number() != pcm::floating_point )
        return static_cast<T>( 2 ) / ( 1ull << fmt.bitwidth() );

    throw std::runtime_error( "Unknown format" );
}


} // namespace detail

//----------------------------------------------------------------------------------------------------------------------

template <typename Stream>
void reference_test( Stream&& stream, const boost::filesystem::path& file )
{
    using value_type = double;

    const auto file_name = file.filename().string();
    const auto tokens    = detail::parse_reference_file( file_name );
    ASSERT_GE( tokens.size(), size_t( 4 ) ) << "Unable to parse reference filename: " << file_name;

    const auto info = stream.info();

    size_t num_channels;
    ASSERT_NO_THROW( num_channels = std::stoul( tokens[1] ) ) << "Invalid token for num_channels: " << tokens[1];
    EXPECT_EQ( info.num_channels(), num_channels );

    size_t sample_rate;
    ASSERT_NO_THROW( sample_rate = std::stoul( tokens[2] ) ) << "Invalid token for sample_rate: " << tokens[2];
    EXPECT_EQ( info.sample_rate(), sample_rate );

    size_t num_frames;
    ASSERT_NO_THROW( num_frames = std::stoul( tokens[3] ) ) << "Invalid token for num_frames: " << tokens[3];
    EXPECT_EQ( info.num_frames(), num_frames );

    if ( info.lossless() )
    {
        auto format = detail::format_to_string( info.format() );
        EXPECT_EQ( format, tokens[4] );
    }

    auto generator_name = tokens[0];
    auto generator      = make_generator<value_type>( generator_name, info.num_channels(), info.sample_rate() );

    ASSERT_TRUE( generator != boost::none ) << "Invalid generator: " << generator_name;

    auto samples = std::vector<value_type>( info.num_samples() );
    stream >> samples;

    auto signal = std::vector<value_type>( info.num_samples() );
    boost::generate( signal, *generator );

    EXPECT_EQ( samples.size(), signal.size() );

    if ( info.lossless() )
    {
        // we set the lower bound of epsilon to 1e-12 (approx 40 bit precision) because std math implementation used in
        // the generators produce sligthly different results depending on the compiler / platform.
        auto epsilon = std::max( value_type( 1e-12 ), detail::epsilon<value_type>( info.format() ) );
        EXPECT_LE( max_difference( signal, samples ), epsilon );
    }
    else
    {
        EXPECT_GT( xcorr( samples, signal ), 0.9998 );
        EXPECT_LT( dc_offset( signal, samples ), 0.0003 );
    }
}
