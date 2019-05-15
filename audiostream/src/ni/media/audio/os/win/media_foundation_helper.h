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

#ifdef WINVER
#undef WINVER
#endif

#define WINVER _WIN32_WINNT_WIN7

#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <windows.h>

#include <Propvarutil.h>
#include <Wmcodecdsp.h>

#include <initguid.h>

#include <mutex>

// Alac is only defined in widows10 sdk and higher
#ifndef MFAudioFormat_ALAC
DEFINE_MEDIATYPE_GUID( MFAudioFormat_ALAC, 0x6C61 );
#endif

// Flac is only defined in widows10 sdk and higher
#ifndef MFAudioFormat_FLAC
DEFINE_MEDIATYPE_GUID( MFAudioFormat_FLAC, 0xF1AC );
#endif

// windows header defines min / max macros.
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif


template <class Source>
class SyncronousByteStream : public IMFByteStream
{

    auto tell() -> std::streamsize
    {
        auto pos = m_source.seek( boost::iostreams::stream_offset( 0 ), BOOST_IOS::cur );
        return ( pos == std::streamsize{-1} ) ? m_size : pos;
    }

public:
    SyncronousByteStream( Source&& buffer )
    : m_source( std::move( buffer ) )
    {
        m_source.seek( boost::iostreams::stream_offset( 0 ), BOOST_IOS::end );
        m_size = tell();
        m_source.seek( boost::iostreams::stream_offset( 0 ), BOOST_IOS::beg );
    }

    HRESULT STDMETHODCALLTYPE Read( BYTE* buffer, ULONG toRead, ULONG* read )
    {
        std::lock_guard<std::mutex> lock( m_reading );

        try
        {
            *read = static_cast<ULONG>( m_source.read( reinterpret_cast<char*>( buffer ), toRead ) );
            return S_OK;
        }
        catch ( const std::system_error& )
        {
            return E_UNEXPECTED;
        }
        catch ( ... )
        {
            return E_FAIL;
        }
    }

    HRESULT STDMETHODCALLTYPE BeginRead( BYTE* buffer, ULONG toRead, IMFAsyncCallback* callback, IUnknown* state )
    {
        if ( !callback || !buffer )
            return E_INVALIDARG;

        IMFAsyncResult* result;

        ULONG read = 0;
        auto  hr   = Read( buffer, toRead, &read );

        auto readResult = new ReadResult( read );
        hr              = MFCreateAsyncResult( readResult, callback, state, &result );
        readResult->Release();

        if ( SUCCEEDED( hr ) )
        {
            result->SetStatus( S_OK );
            hr = MFInvokeCallback( result );
        }

        return hr;
    }

    HRESULT STDMETHODCALLTYPE EndRead( IMFAsyncResult* readResultCb, ULONG* read )
    {
        if ( !readResultCb )
            return E_INVALIDARG;

        IUnknown* unknown;
        if ( FAILED( readResultCb->GetObject( &unknown ) ) || !unknown )
            return E_INVALIDARG;

        auto readResult = static_cast<ReadResult*>( unknown );
        *read           = readResult->bytesRead();
        readResult->Release();

        HRESULT hr = readResultCb->GetStatus();

        readResultCb->Release();

        return hr;
    }

    HRESULT STDMETHODCALLTYPE BeginWrite( const BYTE*, ULONG, IMFAsyncCallback*, IUnknown* )
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Close()
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE EndWrite( IMFAsyncResult*, ULONG* )
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Flush()
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetCapabilities( DWORD* capabilities )
    {
        *capabilities = MFBYTESTREAM_IS_READABLE | MFBYTESTREAM_IS_SEEKABLE;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetCurrentPosition( QWORD* position )
    {
        std::lock_guard<std::mutex> lock( m_reading );
        *position = tell();

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetLength( QWORD* length )
    {
        std::lock_guard<std::mutex> lock( m_reading );
        *length = m_size;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE IsEndOfStream( BOOL* endOfStream )
    {
        std::lock_guard<std::mutex> lock( m_reading );
        *endOfStream = ( tell() >= m_size ) ? true : false;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Seek( MFBYTESTREAM_SEEK_ORIGIN seekOrigin,
                                    LONGLONG                 seekOffset,
                                    DWORD,
                                    QWORD* currentPosition )
    {
        std::lock_guard<std::mutex> lock( m_reading );

        auto pos = ( seekOrigin == msoBegin ) ? static_cast<std::streamsize>( 0 ) : tell();
        pos += seekOffset;

        *currentPosition =
            static_cast<QWORD>( m_source.seek( boost::iostreams::stream_offset( pos ), BOOST_IOS::beg ) );

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetCurrentPosition( QWORD position )
    {
        std::lock_guard<std::mutex> lock( m_reading );
        m_source.seek( boost::iostreams::stream_offset( position ), BOOST_IOS::beg );

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetLength( QWORD )
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Write( const BYTE*, ULONG, ULONG* )
    {
        return E_NOTIMPL;
    }

    ULONG STDMETHODCALLTYPE AddRef()
    {
        InterlockedIncrement( &m_references );
        return m_references;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void** objectPtr )
    {
        if ( !objectPtr )
            return E_POINTER;

        if ( riid == IID_IMFByteStream )
        {
            *objectPtr = static_cast<IMFByteStream*>( this );
        }
        else if ( riid == IID_IUnknown )
        {
            *objectPtr = static_cast<IUnknown*>( this );
        }
        else
        {
            *objectPtr = NULL;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        ULONG refCount = InterlockedDecrement( &m_references );

        if ( refCount == 0 )
            delete this;

        return refCount;
    }

private:
    class ReadResult : public IUnknown
    {
        ULONG m_references;
        ULONG m_bytesRead;

    public:
        ReadResult( ULONG bytesRead )
        : m_bytesRead( bytesRead )
        , m_references( 1 )
        {
        }

        ULONG STDMETHODCALLTYPE AddRef()
        {
            return InterlockedIncrement( &m_references );
        }

        ULONG STDMETHODCALLTYPE Release()
        {
            ULONG refCount = InterlockedDecrement( &m_references );

            if ( refCount == 0 )
                delete this;

            return refCount;
        }

        HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void** objectPtr )
        {
            if ( !objectPtr )
                return E_POINTER;

            if ( riid == IID_IUnknown )
            {
                *objectPtr = static_cast<IUnknown*>( this );
            }
            else
            {
                *objectPtr = NULL;
                return E_NOINTERFACE;
            }
            AddRef();
            return S_OK;
        }

        auto bytesRead() const
        {
            return m_bytesRead;
        }
    };

    Source          m_source;
    ULONG           m_references = 1;
    std::streamsize m_size       = 0;

    std::mutex m_reading;
};
