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

#include <gtest/gtest.h>

#include <ni/media/audio/ifstream.h>
#include <ni/media/read_interlaced_test.h>
#include <ni/media/source_test.h>
#include <ni/media/test_helper.h>

#include <boost/predef.h>

#include <exception>
#include <string>


//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ifstream, default_constructor )
{
    audio::ifstream is;

    EXPECT_EQ( -1, is.tellg() );
    EXPECT_TRUE( is.bad() );
}

//----------------------------------------------------------------------------------------------------------------------


TEST( ni_media_audio_ifstream, string_constructor )
{
    EXPECT_THROW( audio::ifstream is( "/fake.aiff" ), std::exception );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ifstream, string_container_constructor_correct_extension )
{
    EXPECT_THROW( audio::ifstream is( "/fake.aiff", audio::ifstream_info::container_type::aiff ), std::exception );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ifstream, string_container_constructor_wrong_extension )
{
    EXPECT_THROW( audio::ifstream is( "/fake.aiff", audio::ifstream_info::container_type::wav ), std::exception );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ifstream, move_constructor )
{
    audio::ifstream is1;

    audio::ifstream is2( std::move( is1 ) );

    EXPECT_EQ( -1, is2.tellg() );
    EXPECT_TRUE( is2.bad() );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ifstream, move_assignment )
{
    audio::ifstream is1, is2;
    is2 = std::move( is1 );

    EXPECT_EQ( -1, is2.tellg() );
    EXPECT_TRUE( is2.bad() );
}

//----------------------------------------------------------------------------------------------------------------------

class ifstream_test : public source_test
{
protected:
    const size_t num_frames = 840; // = 2 * 3 * 4 * 5 * 7 (i.e. dividable by all n <= 8 )
};

//----------------------------------------------------------------------------------------------------------------------

TEST_P( ifstream_test, read_begining_of_stream_twice )
{
    auto stream    = open_file_as<audio::ifstream>();
    auto frame_pos = 0u;

#if BOOST_OS_WINDOWS
    // This test randomly fails for mp3s on windows so we disable it untill we have a fix.
    if ( stream.info().container() == audio::ifstream_info::container_type::mp3 )
        return;
#endif

    read_interlaced_test( stream, num_frames, frame_pos );
}

//----------------------------------------------------------------------------------------------------------------------

TEST_P( ifstream_test, read_from_frame_67_twice )
{
    auto stream    = open_file_as<audio::ifstream>();
    auto frame_pos = 67;

#if BOOST_OS_WINDOWS
    // This test randomly fails for mp3s on windows so we disable it untill we have a fix.
    if ( stream.info().container() == audio::ifstream_info::container_type::mp3 )
        return;
#endif

    read_interlaced_test( stream, num_frames, frame_pos );
}

//----------------------------------------------------------------------------------------------------------------------

TEST_P( ifstream_test, read_middle_of_stream_twice )
{
    auto stream    = open_file_as<audio::ifstream>();
    auto frame_pos = stream.info().num_frames() / 2;

#if BOOST_OS_WINDOWS
    // This test randomly fails for mp3s on windows so we disable it untill we have a fix.
    if ( stream.info().container() == audio::ifstream_info::container_type::mp3 )
        return;
#endif
    read_interlaced_test( stream, num_frames, frame_pos );
}

//----------------------------------------------------------------------------------------------------------------------

TEST_P( ifstream_test, read_past_eof_twice )
{
    auto stream = open_file_as<audio::ifstream>();

#if BOOST_OS_WINDOWS
    // This test randomly fails for mp3s on windows so we disable it untill we have a fix.
    if ( stream.info().container() == audio::ifstream_info::container_type::mp3 )
        return;
#endif

    // 5 seconds before end of stream or shorter if stream is shorter
    auto num_frames_from_end = std::min( stream.info().num_frames(), size_t( 5 * stream.info().sample_rate() ) );

    auto num_samples = ( 2 * num_frames_from_end ) * stream.info().num_channels();

    auto frame_pos = stream.info().num_frames() - num_frames_from_end;

    std::vector<float> first_pass( num_samples ), second_pass( num_samples );

    stream.frame_seekg( frame_pos );
    ASSERT_TRUE( stream.good() );

    stream >> first_pass;

    EXPECT_TRUE( stream.eof() );
    ASSERT_FALSE( stream.fail() );

    stream.frame_seekg( frame_pos );
    ASSERT_TRUE( stream.good() );

    stream >> second_pass;

    EXPECT_TRUE( stream.eof() );
    ASSERT_FALSE( stream.fail() );

    EXPECT_TRUE( boost::equal( first_pass, second_pass ) );
}

//----------------------------------------------------------------------------------------------------------------------

TEST_P( ifstream_test, read_begining_of_stream_interlaced_by_67_frames_forward )
{
    auto stream       = open_file_as<audio::ifstream>();
    auto frame_pos    = 0;
    auto frame_offset = 67;
    auto num_blocks   = 16;

#if BOOST_OS_WINDOWS
    // This test randomly fails for mp3s on windows so we disable it untill we have a fix.
    if ( stream.info().container() == audio::ifstream_info::container_type::mp3 )
        return;
#endif

// TODO: fix AUDIOCHAP-73
#if BOOST_OS_MACOS || BOOST_OS_IOS
    if ( stream.info().container() == audio::ifstream_info::container_type::mp3 )
        return;
#endif
    // TODO: fix AUDIOCHAP-76
    if ( stream.info().container() == audio::ifstream_info::container_type::wma )
        return;

    read_interlaced_test( stream, num_frames, frame_pos, frame_offset, num_blocks );
}

//----------------------------------------------------------------------------------------------------------------------

TEST_P( ifstream_test, read_begining_of_stream_interlaced_by_67_frames_backward )
{
    auto stream       = open_file_as<audio::ifstream>();
    auto frame_pos    = 67;
    auto frame_offset = -frame_pos;
    auto num_blocks   = 16;

#if BOOST_OS_WINDOWS
    // This test randomly fails for mp3s on windows so we disable it untill we have a fix.
    if ( stream.info().container() == audio::ifstream_info::container_type::mp3 )
        return;
#endif

// TODO: fix AUDIOCHAP-73
#if BOOST_OS_MACOS || BOOST_OS_IOS
    if ( stream.info().container() == audio::ifstream_info::container_type::mp3 )
        return;
#endif

    // TODO: fix AUDIOCHAP-76
    if ( stream.info().container() == audio::ifstream_info::container_type::wma )
        return;

    read_interlaced_test( stream, num_frames, frame_pos, frame_offset, num_blocks );
}

//----------------------------------------------------------------------------------------------------------------------


INSTANTIATE_TEST_SUITE_P( reference_files, ifstream_test, reference_files(), ParamToString{} );
INSTANTIATE_TEST_SUITE_P( user_files, ifstream_test, user_files() );
