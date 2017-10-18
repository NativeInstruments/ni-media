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

#include <ni/media/pcm/format.h>

#include <ni/media/pcm/detail/tuple_to_array.h>

#include <boost/algorithm/cxx11/one_of.hpp>

namespace pcm
{


namespace
{

//----------------------------------------------------------------------------------------------------------------------

format to_format( number n, bitwidth b, endian e )
{
    auto it = boost::find_if( formats(), [n, b, e]( const format& f ) {
        return get_endian( f ) == e && get_number( f ) == n && get_bitwidth( f ) == b;
    } );

    if ( it == formats().end() )
        throw std::runtime_error( "Invalid format" );

    return *it;
}

//----------------------------------------------------------------------------------------------------------------------

bitwidth to_bitwidth( size_t bw )
{
    switch ( bw )
    {
        case 8:
            return _8bit;
        case 16:
            return _16bit;
        case 24:
            return _24bit;
        case 32:
            return _32bit;
        case 64:
            return _64bit;
        default:
            throw std::runtime_error( "Invalid bitwidth" );
    };
}

//----------------------------------------------------------------------------------------------------------------------

number to_number( bool is_signed, bool is_integer )
{
    if ( is_integer )
        return is_signed ? signed_integer : unsigned_integer;
    else if ( is_signed )
        return floating_point;

    throw std::runtime_error( "Invalid number" );
}

} // anonymous

//----------------------------------------------------------------------------------------------------------------------

format::format( pcm::number n, pcm::bitwidth b, pcm::endian e )
: format( to_format( n, b, e ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

format::format( number n, size_t bits, endian e )
: format( n, to_bitwidth( bits ), e )
{
}

//----------------------------------------------------------------------------------------------------------------------

format::format( bool is_signed, bool is_integer, size_t bits, endian e )
: format( to_number( is_signed, is_integer ), bits, e )
{
}

//----------------------------------------------------------------------------------------------------------------------

auto formats() -> const std::array<format, std::tuple_size<format::tags>::value>&
{
    static auto const values = detail::tuple_to_array<format>( format::tags{} );
    return values;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Enum>
Enum get( const format& f )
{
    static auto const values = detail::tuple_to_array<Enum>( format::tags() );
    return values.at( get_index( f ) );
}

//----------------------------------------------------------------------------------------------------------------------

auto get_number( const format& f ) -> number
{
    return get<number>( f );
}

//----------------------------------------------------------------------------------------------------------------------

auto get_bitwidth( const format& f ) -> bitwidth
{
    return get<bitwidth>( f );
}

//----------------------------------------------------------------------------------------------------------------------

auto get_endian( const format& f ) -> endian
{
    return get<endian>( f );
}

//----------------------------------------------------------------------------------------------------------------------

auto get_index( const format& f ) -> size_t
{
    return f.m_index;
}

//----------------------------------------------------------------------------------------------------------------------

bool is_valid( const format& f )
{
    return boost::algorithm::one_of_equal( formats(), f );
}

//----------------------------------------------------------------------------------------------------------------------

auto make_format( number n, bitwidth b, endian e ) -> format
{
    return {n, b, e};
}

//----------------------------------------------------------------------------------------------------------------------


} // namespace pcm
