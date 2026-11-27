//
// Copyright (c) 2017-2023 Native Instruments GmbH, Berlin
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
#include <ni/media/audio/ifvectorstream.h>

#include <ni/media/generators.h>
#include <ni/media/reference_test.h>
#include <ni/media/source_test.h>
#include <ni/media/statistics.h>
#include <ni/media/test_helper.h>

#include <boost/range/algorithm/generate.hpp>

#include <vector>


class mp4_source_test : public source_test
{
};

//----------------------------------------------------------------------------------------------------------------------

TEST_P( mp4_source_test, ifstream )
{
    auto stream = open_file_as<audio::ifstream>();
    reference_test( stream, file_name() );
}

//----------------------------------------------------------------------------------------------------------------------

TEST_P( mp4_source_test, ifvectorstream )
{
    auto stream = open_file_as<audio::ifvectorstream>();
    reference_test( stream, file_name() );
}

//----------------------------------------------------------------------------------------------------------------------

// These files were made from sin440.1.44100.44100.s16le.aac192.ffmpeg.m4a, whose encoder primed 1024 frames, by
// changing its edit list to play all of the media, removing its roll sample group, or both. Every platform has to
// decode them as Core Audio does.
namespace
{

constexpr size_t ffmpeg_priming = 1024;

audio::ifstream open_priming_test_file( const std::string& name )
{
    return audio::ifstream( ( test_files_input_path() / "mp4" / name ).string() );
}

// Expects each decoded frame to be the frame of the 440 Hz source sine that lies offset frames later.
void expect_sin440_at_offset( audio::ifstream& stream, std::ptrdiff_t offset )
{
    constexpr size_t source_frames = 44100;
    constexpr size_t first_frame   = 2048;
    constexpr size_t end_frame     = 42000;

    auto samples = std::vector<double>( stream.info().num_samples() );
    stream >> samples;
    ASSERT_GE( samples.size(), end_frame );

    auto generator = *make_generator<double>( "sin440", 1, stream.info().sample_rate() );
    auto source    = std::vector<double>( source_frames );
    boost::generate( source, generator );

    const auto decoded = std::vector<double>( samples.begin() + first_frame, samples.begin() + end_frame );
    const auto expected =
        std::vector<double>( source.begin() + first_frame + offset, source.begin() + end_frame + offset );
    EXPECT_GT( xcorr( decoded, expected ), 0.9998 );
}

// Without a roll sample group, as older ffmpeg versions write, Core Audio ignores the edit list. It assumes the
// standard 2112 frames of AAC priming, more than the encoder primed, and plays every decoded frame after them.
void expect_default_aac_priming( const std::string& name )
{
    constexpr size_t decoded_frames  = 45 * 1024;
    constexpr size_t assumed_priming = 2112;

    auto stream = open_priming_test_file( name );
    ASSERT_TRUE( stream.good() );
    EXPECT_EQ( stream.info().num_frames(), decoded_frames - assumed_priming );
    expect_sin440_at_offset( stream, std::ptrdiff_t( assumed_priming - ffmpeg_priming ) );
}

} // namespace

// An edit list that plays all of the media declares nothing. With a roll sample group, Core Audio plays all of it.
TEST( mp4_source_priming_test, edit_list_playing_all )
{
    auto stream = open_priming_test_file( "sin440.ffmpeg.edit_list_plays_all.m4a" );
    ASSERT_TRUE( stream.good() );
    EXPECT_EQ( stream.info().num_frames(), size_t( 45 * 1024 ) );
    expect_sin440_at_offset( stream, -std::ptrdiff_t( ffmpeg_priming ) );
}

TEST( mp4_source_priming_test, undeclared_aac_priming )
{
    expect_default_aac_priming( "sin440.ffmpeg.undeclared_priming.m4a" );
}

TEST( mp4_source_priming_test, edit_list_ignored_without_roll_group )
{
    expect_default_aac_priming( "sin440.ffmpeg.edit_list_without_roll_group.m4a" );
}

// These files were made from a 1 second 440 Hz sine encoded by Apple's afconvert, whose encoder primed 2112 frames, by
// setting its edit list to 1024 frames, its iTunSMPB tag to 1600 frames of priming and 1404 of padding, its roll sample
// group and its brand. Each expectation is how Core Audio decodes the file.
namespace
{

constexpr size_t apple_priming = 2112;

void expect_apple_file( const std::string& name, size_t num_frames, size_t applied_priming )
{
    auto stream = open_priming_test_file( name );
    ASSERT_TRUE( stream.good() );
    EXPECT_EQ( stream.info().num_frames(), num_frames );
    expect_sin440_at_offset( stream, std::ptrdiff_t( applied_priming ) - std::ptrdiff_t( apple_priming ) );
}

} // namespace

// Without a roll sample group, the edit list is ignored and the iTunSMPB tag decides.
TEST( mp4_source_priming_test, itunes_tag_with_edit_list )
{
    expect_apple_file( "sin440.apple.itunes_tag_and_edit_list.m4a", 44100, 1600 );
}

TEST( mp4_source_priming_test, edit_list_with_roll_group_overrides_itunes_tag )
{
    expect_apple_file( "sin440.apple.edit_list_with_roll_group_and_itunes_tag.m4a", 44100, 1024 );
}

// The iTunSMPB tag only counts in files of an iTunes brand, so this file falls back to the standard AAC priming.
TEST( mp4_source_priming_test, itunes_tag_ignored_without_itunes_brand )
{
    expect_apple_file( "sin440.apple.itunes_tag_without_itunes_brand.m4a", 44992, apple_priming );
}

//----------------------------------------------------------------------------------------------------------------------


INSTANTIATE_TEST_SUITE_P( reference_test,
                          mp4_source_test,
                          reference_files( audio::ifstream_info::container_type::mp4 ),
                          ParamToString{} );
