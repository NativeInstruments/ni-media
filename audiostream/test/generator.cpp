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

#include <ni/media/generators.h>

#include <ni/media/audio/wav/wav_ofstream.h>

#include <boost/range/algorithm/generate.hpp>

#include <boost/program_options.hpp>
#include <cstddef>
#include <iostream>

int main( int argc, const char* argv[] )
{
    namespace po = boost::program_options;

    size_t      sample_rate    = 44100;
    size_t      num_channels   = 2;
    size_t      num_frames     = 0;
    std::string generator_name = "sin440";
    std::string format_name    = "f32ne";
    std::string output_name    = "./test.wav";


    try
    {
        po::options_description desc{"Options"};
        desc.add_options()( "help,h", "Help screen" )                                        //
            ( "rate,r", po::value<size_t>( &sample_rate ), "Samplerate" )                    //
            ( "channels,c", po::value<size_t>( &num_channels ), "Numnber of channels" )      //
            ( "length,l", po::value<size_t>( &num_frames ), "Length in frames" )             //
            ( "generator,g", po::value<std::string>( &generator_name ), "Signal generator" ) //
            ( "format,f", po::value<std::string>( &format_name ), "Pcm format" )             //
            ( "output,o", po::value<std::string>( &output_name ), "Output filename" );

        po::variables_map vm;
        po::store( po::parse_command_line( argc, argv, desc ), vm );
        po::notify( vm );

        if ( vm.count( "help" ) )
            std::cout << desc << '\n';
    }
    catch ( const po::error& ex )
    {
        std::cerr << ex.what() << '\n';
    }

    if ( num_frames == 0 )
        num_frames = sample_rate; // 1 second;


    try
    {
        audio::wav_ofstream_info info;
        info.format( format_name );
        info.num_channels( num_channels );
        info.sample_rate( sample_rate );

        using value_type = double;

        auto generator = make_generator<value_type>( generator_name, num_channels, sample_rate );
        auto samples   = std::vector<value_type>( num_channels * num_frames );
        boost::generate( samples, *generator );

        std::cout << "writing file: " << output_name << std::endl;

        audio::wav_ofstream os( output_name, info );
        os << samples;
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Failed writing file: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
