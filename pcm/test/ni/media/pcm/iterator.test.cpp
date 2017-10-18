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

#include <ni/media/pcm/iterator.h>

#include <gtest/gtest.h>

#include <type_traits>

TEST( pcm_iterator_test, const_mutable_convertible )
{
    using Vec       = std::vector<char>;
    using Iter      = pcm::iterator<float, Vec::iterator>;
    using ConstIter = pcm::iterator<float, Vec::const_iterator>;

    // is_convertible<From,To>
    EXPECT_TRUE( ( std::is_convertible<Iter, ConstIter>::value ) );
    EXPECT_FALSE( ( std::is_convertible<ConstIter, Iter>::value ) );
}

TEST( pcm_iterator_test, const_mutable_assignable )
{
    using Vec       = std::vector<char>;
    using Iter      = pcm::iterator<float, Vec::iterator>;
    using ConstIter = pcm::iterator<float, Vec::const_iterator>;

    // is_assignable<To,From>
    EXPECT_TRUE( ( std::is_assignable<ConstIter&, Iter>::value ) );
    EXPECT_FALSE( ( std::is_assignable<Iter&, ConstIter>::value ) );
}

TEST( pcm_iterator_test, const_mutable_comparable )
{
    using Vec       = std::vector<char>;
    using Iter      = pcm::iterator<float, Vec::iterator>;
    using ConstIter = pcm::iterator<float, Vec::const_iterator>;

    {
        // equal after assignment
        Iter      iter;
        ConstIter const_iter;
        const_iter = iter;

        auto eql = iter == const_iter;
        EXPECT_TRUE( eql );

        auto eqr = const_iter == iter;
        EXPECT_TRUE( eqr );
    }

    {
        // equal after construction
        Iter      iter;
        ConstIter const_iter = iter;

        auto eql = iter == const_iter;
        EXPECT_TRUE( eql );

        auto eqr = const_iter == iter;
        EXPECT_TRUE( eqr );
    }
}

TEST( pcm_iterator_test, compiletime_to_runtime_convertible )
{
    using Vec              = std::vector<char>;
    using RuntimeIter      = pcm::iterator<float, Vec::iterator>;
    using RuntimeConstIter = pcm::iterator<float, Vec::const_iterator>;

    using CompileTimeFormat    = pcm::format::tag<pcm::signed_integer, pcm::_16bit>;
    using CompiletimeIter      = pcm::iterator<float, Vec::iterator, CompileTimeFormat>;
    using CompiletimeConstIter = pcm::iterator<float, Vec::const_iterator, CompileTimeFormat>;

    // is_convertible<From,To>
    EXPECT_TRUE( ( std::is_convertible<CompiletimeIter, RuntimeIter>::value ) );
    EXPECT_TRUE( ( std::is_convertible<CompiletimeIter, RuntimeConstIter>::value ) );
    EXPECT_FALSE( ( std::is_convertible<CompiletimeConstIter, RuntimeIter>::value ) );
    EXPECT_TRUE( ( std::is_convertible<CompiletimeConstIter, RuntimeConstIter>::value ) );
}

TEST( pcm_iterator_test, runtime_to_compiletime_convertible )
{
    using Vec              = std::vector<char>;
    using RuntimeIter      = pcm::iterator<float, Vec::iterator>;
    using RuntimeConstIter = pcm::iterator<float, Vec::const_iterator>;

    using CompileTimeFormat    = pcm::format::tag<pcm::signed_integer, pcm::_16bit>;
    using CompiletimeIter      = pcm::iterator<float, Vec::iterator, CompileTimeFormat>;
    using CompiletimeConstIter = pcm::iterator<float, Vec::const_iterator, CompileTimeFormat>;

    // is_convertible<From,To>
    EXPECT_TRUE( ( std::is_convertible<RuntimeIter, CompiletimeIter>::value ) );
    EXPECT_TRUE( ( std::is_convertible<RuntimeIter, CompiletimeConstIter>::value ) );
    EXPECT_FALSE( ( std::is_convertible<RuntimeConstIter, CompiletimeIter>::value ) );
    EXPECT_TRUE( ( std::is_convertible<RuntimeConstIter, CompiletimeConstIter>::value ) );
}

TEST( pcm_iterator_test, runtime_to_compiletime_assignable )
{
    using Vec              = std::vector<char>;
    using RuntimeIter      = pcm::iterator<float, Vec::iterator>;
    using RuntimeConstIter = pcm::iterator<float, Vec::const_iterator>;

    using CompileTimeFormat    = pcm::format::tag<pcm::signed_integer, pcm::_16bit>;
    using CompiletimeIter      = pcm::iterator<float, Vec::iterator, CompileTimeFormat>;
    using CompiletimeConstIter = pcm::iterator<float, Vec::const_iterator, CompileTimeFormat>;

    // is_assignable<To,From>
    EXPECT_TRUE( ( std::is_assignable<CompiletimeIter&, RuntimeIter>::value ) );
    EXPECT_TRUE( ( std::is_assignable<CompiletimeConstIter&, RuntimeIter>::value ) );
    EXPECT_FALSE( ( std::is_assignable<CompiletimeIter&, RuntimeConstIter>::value ) );
    EXPECT_TRUE( ( std::is_assignable<CompiletimeConstIter&, RuntimeConstIter>::value ) );
}

TEST( pcm_iterator_test, compiletime_to_runtime_assignable )
{
    using Vec              = std::vector<char>;
    using RuntimeIter      = pcm::iterator<float, Vec::iterator>;
    using RuntimeConstIter = pcm::iterator<float, Vec::const_iterator>;

    using CompileTimeFormat    = pcm::format::tag<pcm::signed_integer, pcm::_16bit>;
    using CompiletimeIter      = pcm::iterator<float, Vec::iterator, CompileTimeFormat>;
    using CompiletimeConstIter = pcm::iterator<float, Vec::const_iterator, CompileTimeFormat>;

    // is_assignable<To,From>
    EXPECT_TRUE( ( std::is_assignable<RuntimeIter&, CompiletimeIter>::value ) );
    EXPECT_TRUE( ( std::is_assignable<RuntimeConstIter&, CompiletimeIter>::value ) );
    EXPECT_FALSE( ( std::is_assignable<RuntimeIter&, CompiletimeConstIter>::value ) );
    EXPECT_TRUE( ( std::is_assignable<RuntimeConstIter&, CompiletimeConstIter>::value ) );
}
