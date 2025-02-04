//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project common-header (PTXdist package libcommonheader).
//
// Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_Assertion.cc
///
///  \brief    Test for assertion helper macros.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include "wc/preprocessing.h"
#include "test_Assertion.h"
#include "c-modules/test_AssertionMod.h"
#include "cpp-modules/test_AssertionMod.hpp"

#ifdef __cplusplus // C++
#include <cassert>
#else // C
#include <assert.h>
#endif

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
void wc_Fail(char const * const szReason,
             char const * const szFile,
             char const * const szFunction,
             int const line)
{
  EXPECT_STREQ(WC_ASSERT_PREFIX WC_STR(ONE) "==" WC_STR(ZERO), szReason);
#ifndef DISABLE_WC_FAIL_SOURCE
  EXPECT_NE(nullptr, szFile);
  EXPECT_NE('\0', szFile[0]);
  EXPECT_NE(nullptr, szFunction);
  EXPECT_NE('\0', szFunction[0]);
  EXPECT_LT(0U, line);
#endif

  // Trigger standard assert
  ASSERT_DEATH_IF_SUPPORTED(assert(false), "");
}


TEST(Assert, SucceedC)
{
  TriggerSuccessC();
}


TEST(Assert, FailC_DeathTest)
{
#ifndef NDEBUG
  ASSERT_NO_FATAL_FAILURE(TriggerFailC());
#endif
}


TEST(Assert, SucceedCPP)
{
  TriggerSuccessCPP();
}


TEST(Assert, FailCPP_DeathTest)
{
#ifndef NDEBUG
  ASSERT_NO_FATAL_FAILURE(TriggerFailCPP());
#endif
}


//---- End of source file ------------------------------------------------------

