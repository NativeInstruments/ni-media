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

#include "istream_read.test.h"

INSTANTIATE_TYPED_TEST_SUITE_P( Uint8ToAll, istream_read_test, make_istream_read_test_t<uint8_t> );
INSTANTIATE_TYPED_TEST_SUITE_P( Uint16ToAll, istream_read_test, make_istream_read_test_t<uint16_t> );
INSTANTIATE_TYPED_TEST_SUITE_P( Uint32ToAll, istream_read_test, make_istream_read_test_t<uint32_t> );
INSTANTIATE_TYPED_TEST_SUITE_P( Uint64ToAll, istream_read_test, make_istream_read_test_t<uint64_t> );
