// Copyright (c) 2014, 2015 Ableton AG, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#pragma once

#if defined(__clang__)

  #define ABL_DISABLE_WARNINGS \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wc++98-compat-pedantic\"") \
    _Pragma("clang diagnostic ignored \"-Wc++98-compat\"") \
    _Pragma("clang diagnostic ignored \"-Wcast-align\"") \
    _Pragma("clang diagnostic ignored \"-Wconditional-uninitialized\"") \
    _Pragma("clang diagnostic ignored \"-Wconversion\"") \
    _Pragma("clang diagnostic ignored \"-Wcovered-switch-default\"") \
    _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"") \
    _Pragma("clang diagnostic ignored \"-Wdeprecated\"") \
    _Pragma("clang diagnostic ignored \"-Wdisabled-macro-expansion\"") \
    _Pragma("clang diagnostic ignored \"-Wdocumentation-unknown-command\"") \
    _Pragma("clang diagnostic ignored \"-Wdocumentation\"") \
    _Pragma("clang diagnostic ignored \"-Wduplicate-enum\"") \
    _Pragma("clang diagnostic ignored \"-Wexit-time-destructors\"") \
    _Pragma("clang diagnostic ignored \"-Wextra-semi\"") \
    _Pragma("clang diagnostic ignored \"-Wglobal-constructors\"") \
    _Pragma("clang diagnostic ignored \"-Wgnu-zero-variadic-macro-arguments\"") \
    _Pragma("clang diagnostic ignored \"-Wheader-hygiene\"") \
    _Pragma("clang diagnostic ignored \"-Wignored-qualifiers\"") \
    _Pragma("clang diagnostic ignored \"-Wimplicit-fallthrough\"") \
    _Pragma("clang diagnostic ignored \"-Wmissing-noreturn\"") \
    _Pragma("clang diagnostic ignored \"-Wmissing-prototypes\"") \
    _Pragma("clang diagnostic ignored \"-Wnon-virtual-dtor\"") \
    _Pragma("clang diagnostic ignored \"-Wold-style-cast\"") \
    _Pragma("clang diagnostic ignored \"-Wpacked\"") \
    _Pragma("clang diagnostic ignored \"-Wpadded\"") \
    _Pragma("clang diagnostic ignored \"-Wshadow\"") \
    _Pragma("clang diagnostic ignored \"-Wshift-sign-overflow\"") \
    _Pragma("clang diagnostic ignored \"-Wsign-compare\"") \
    _Pragma("clang diagnostic ignored \"-Wsign-conversion\"") \
    _Pragma("clang diagnostic ignored \"-Wswitch-enum\"") \
    _Pragma("clang diagnostic ignored \"-Wtautological-constant-out-of-range-compare\"") \
    _Pragma("clang diagnostic ignored \"-Wundef\"") \
    _Pragma("clang diagnostic ignored \"-Wundefined-reinterpret-cast\"") \
    _Pragma("clang diagnostic ignored \"-Wunreachable-code\"") \
    _Pragma("clang diagnostic ignored \"-Wunused-parameter\"") \
    _Pragma("clang diagnostic ignored \"-Wused-but-marked-unused\"") \
    _Pragma("clang diagnostic ignored \"-Wweak-vtables\"") \
    _Pragma("clang diagnostic ignored \"-Wzero-length-array\"")

  #define ABL_RESTORE_WARNINGS \
    _Pragma("clang diagnostic pop")

  #define ABL_DISABLE_PARAMETERIZED_GTEST_WARNINGS \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wmissing-prototypes\"") \
    _Pragma("clang diagnostic ignored \"-Wmissing-variable-declarations\"")

  /*! Annotate a function that the user must handle the return value of it.
   * Either because it's a bad idea to ignore it (e.g. with an error code) or
   * because you have to keep it (e.g. with boost::scoped_connections).
   *
   * @code
   * boost::signals2::connection connectToChangedSignal(T&& functor) REQUIRE_HANDLE_RESULT;
   * @endcode
   *
   * In case a user is really not interessted in the result the IGNORE_RESULT
   * macro can be handy.
   */
  #define REQUIRE_HANDLE_RESULT __attribute__((warn_unused_result))

#elif defined(_MSC_VER)

  /**
  * C4100: unreferenced formal parameter
  * C4127: conditional expression is constant
  * C4138: Closing-comment delimiter found outside of comment
  * C4200: nonstandard extension used : zero-sized array in struct/union
  * C4244: conversion from 'type1' to 'type2', possible loss of data
  * C4251: class 'type1' needs to have dll-interface to be used by clients of class 'type2'
  * C4510: default constructor could not be generated
  * C4512: assignment operator could not be generated
  * C4610: object 'class' can never be instantiated - user-defined constructor required
  * C4702: unreachable code
  * C4800: forcing value to bool 'true' or 'false' (performance warning)
  * C4913: user defined binary operator ',' exists but no overload could convert all operands, default built-in binary operator ',' used
  */
  #define ABL_DISABLE_WARNINGS \
    __pragma(warning(push)) \
    __pragma(warning(disable: 4100)) \
    __pragma(warning(disable: 4127)) \
    __pragma(warning(disable: 4138)) \
    __pragma(warning(disable: 4200)) \
    __pragma(warning(disable: 4244)) \
    __pragma(warning(disable: 4251)) \
    __pragma(warning(disable: 4510)) \
    __pragma(warning(disable: 4512)) \
    __pragma(warning(disable: 4610)) \
    __pragma(warning(disable: 4702)) \
    __pragma(warning(disable: 4800)) \
    __pragma(warning(disable: 4913))

  #define ABL_RESTORE_WARNINGS \
    __pragma(warning(pop))

  #define ABL_DISABLE_PARAMETERIZED_GTEST_WARNINGS \
    __pragma(warning(push))

  #define REQUIRE_HANDLE_RESULT

#else

  #define ABL_DISABLE_WARNINGS
  #define ABL_RESTORE_WARNINGS
  #define ABL_DISABLE_PARAMETERIZED_GTEST_WARNINGS

  #define REQUIRE_HANDLE_RESULT __attribute__((warn_unused_result))

#endif

/*!
 * Use this macro to show that the result of a function call (annotated with
 * REQUIRE_HANDLE_RESULT) is ignored.  This might make your intention clearer
 * to readers than assigning it to local variables or alike.  Use with care.
 *
 * @code
 * IGNORE_RESULT scanf("%s", &tmp);
 * @endcode
 */
#define IGNORE_RESULT (void)
