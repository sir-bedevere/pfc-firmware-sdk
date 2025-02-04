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
/// \version <Revision>: $Rev: 8866 $
///
/// \brief short description of the file contents
///
/// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef D_OS_ATOMIC_H
#define D_OS_ATOMIC_H

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include "OsCommon.h"


#define OS_ATOMIC_INIT(value) ((tOsAtomic) { value })

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct OsAtomic {
	i32 Value;
}tOsAtomic;

typedef struct OsAtomic64 {
	i64 Value;
}tOsAtomic64;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


static inline i32 OsAtomic_Fetch(tOsAtomic *atomic)
{
	return ((volatile i32)atomic->Value);
}

static inline i32 OsAtomic_AddAndFetch(tOsAtomic *atomic, i32 value)
{
	return __sync_add_and_fetch(&atomic->Value, value);
}

static inline i32 OsAtomic_SubAndFetch(tOsAtomic *atomic, i32 value)
{
	return __sync_sub_and_fetch(&atomic->Value, value);
}

static inline i32 OsAtomic_FetchAndAdd(tOsAtomic *atomic, i32 value)
{
	return __sync_fetch_and_add(&atomic->Value, value);
}

static inline i32 OsAtomic_FetchAndSub(tOsAtomic *atomic, i32 value)
{
	return __sync_fetch_and_sub(&atomic->Value, value);
}

static inline i32 OsAtomic_Inc(tOsAtomic *atomic)
{
	return OsAtomic_AddAndFetch(&atomic->Value, 1);
}

static inline i32 OsAtomic_Dec(tOsAtomic *atomic)
{
	return OsAtomic_SubAndFetch(&atomic->Value, 1);
}

#endif  // D_OS_ATOMIC_H
