//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file ${file_name}
///
/// \version <Revision>: $Rev: 6632 $
///
/// \brief short description of the file contents
///
/// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
extern "C"
{
#include "OsMemory.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(OsMemory)
{
    void setup()
    {
      OsMemory_Create();
    }

    void teardown()
    {
       OsMemory_Destroy();
    }
};

TEST(OsMemory, Create)
{
  FAIL("Start here");
}

