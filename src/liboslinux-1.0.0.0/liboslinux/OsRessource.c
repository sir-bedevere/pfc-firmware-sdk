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
/// \version <Revision>: $Rev: 4892 $
///
/// \brief short description of the file contents
///
/// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include "OsRessource.h"

#include <string.h>
#include <memory.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------



void OsRessource_SetName(tOsRessource *ressource,
                         const char* name)
{
   strncpy(ressource->Name, name, MAX_OS_RESSOURCE_NAME_LENGTH);
   /* in case the supplied string is too long,
    * make sure the result gets zero terminated. */
   ressource->Name[MAX_OS_RESSOURCE_NAME_LENGTH] = '\0';
}

void OsRessource_GetName(tOsRessource *ressource,
                         size_t bufferLength,
                         char *name)
{
   strncpy(name, ressource->Name ,bufferLength);
   name[bufferLength - 1] ='\0';
}

