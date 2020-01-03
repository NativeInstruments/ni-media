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

#pragma once


#include <ni/media/generators/modulated_sine.h>

#include <boost/optional.hpp>

#include <functional>
#include <map>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
template <class Value>
inline std::map<std::string, std::function<Value()>> generator_map( size_t num_channels, size_t sample_rate )
{
    return {{"sin440", modulated_sine<Value>( num_channels, sample_rate, Value( 440 ) )}, //
            {"sin_mod440", modulated_sine<Value>( num_channels, sample_rate, Value( 440 ), Value( 23 ), Value( 16 ) )}};
}

//----------------------------------------------------------------------------------------------------------------------

template <class Value>
inline boost::optional<std::function<Value()>> make_generator( const std::string& generator_name,
                                                               size_t             num_channels,
                                                               size_t             sample_rate )
{
    const auto map = generator_map<Value>( num_channels, sample_rate );
    const auto it  = map.find( generator_name );

    return ( it != map.end() ) ? boost::make_optional( it->second ) : boost::none;
}
