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

#include <boost/icl/interval_set.hpp>
#include <boost/icl/right_open_interval.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/positioning.hpp>

#include <ni/media/audio/ifstream_info.h>
#include <gst/gst.h>

namespace detail
{
  template<typename T, size_t s> struct RingBuffer;
}

class gstreamer_file_source
{
  public:
    //----------------------------------------------------------------------------------------------------------------------

    using offset_type = boost::iostreams::stream_offset;

    using FrameRange = boost::icl::right_open_interval<offset_type>;
    using FrameRangeSet = boost::icl::interval_set<FrameRange::domain_type, std::less, FrameRange>;

    template<typename MfType>
      using MfTypePtr = std::unique_ptr<MfType, std::function<void( MfType* )>>;

    //----------------------------------------------------------------------------------------------------------------------

    explicit gstreamer_file_source(const std::string& path, audio::ifstream_info::container_type container, size_t stream = 0);
    ~gstreamer_file_source();

    audio::ifstream_info info() const
    {
      return m_info;
    }

    std::streampos seek(offset_type, BOOST_IOS::seekdir );
    std::streamsize read( char*, std::streamsize );

    private:
    static void init_gstreamer();
    static pcm::runtime_format create_runtime_format(GstStructure* caps_struct);
    static pcm::number_type gst_format_char_to_number_type(const gchar format);

    void setup_source(const std::string& path, audio::ifstream_info::container_type container);
    GstElement* prepare_pipeline(const std::string& path);
    void preroll_pipeline();
    void fill_format_info(GstStructure *caps_struct, audio::ifstream_info::container_type container);

    static void onPadAdded(GstElement* element, GstPad* pad, GstElement* sink);

    audio::ifstream_info m_info;
    using GUnref = void(*)(gpointer);
    template<typename T> using tGstPtr = std::unique_ptr<T, GUnref>;
    tGstPtr<GstElement> m_pipeline;

    using RingBuffer = detail::RingBuffer<char, 8192>;
    std::unique_ptr<RingBuffer> m_ring_buffer;
    int64_t m_position = 0;
  };
