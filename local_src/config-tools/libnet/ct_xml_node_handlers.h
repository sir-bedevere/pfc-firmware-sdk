// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef __XML_NODE_HANDLERS_H__
#define __XML_NODE_HANDLERS_H__

#include <libxml/tree.h>


int ct_xml_apply_update_function(xmlNode * curNode, void *data);

#endif
