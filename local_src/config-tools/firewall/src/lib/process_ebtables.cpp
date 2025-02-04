//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process_ebtables.cpp
///
/// \brief Function processing ebtables configuration file.
///
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#include "process_ebtables.hpp"

#include "error.hpp"
#include "process.hpp"
#include "regex.hpp"
#include <libxml/parser.h>
#include <iostream>


namespace wago
{


static void set_mode(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& mode(argv[0]);

    if (!is_match_std(regex::rex_mode, mode))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode ethernet(get_node(ctx, "/f:firewall/f:ethernet"));

    if (!ethernet.is_empty())
        update_attribute(ethernet, "mode", mode);
}

static void set_log(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (5 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);
    const std::string& tag(argv[1]);
    const std::string& limit(argv[2]);
    const std::string& burst(argv[3]);
    const std::string& level(argv[4]);

    if (!is_match_std(regex::rex_onoff, state) ||
        !is_match_std(regex::rex_tag, tag) ||
        !is_match_opt(regex::rex_limit, limit) ||
        !is_match_opt(regex::rex_burst, burst) ||
        !is_match_std(regex::rex_level, level))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode log(get_node(ctx, "/f:firewall/f:ethernet/f:log"));

    if (!log.is_empty())
    {
        update_attribute(log, "state", state);
        update_attribute(log, "tag", tag);
        updrem_attribute(log, "limit", limit);
        updrem_attribute(log, "burst", burst);
        update_attribute(log, "level", level);
    }
}

static void set_if(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (2 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);
    const std::string& ifname(argv[1]);

    if (!is_match_std(regex::rex_ifstate, state) ||
        !is_match_std(regex::rex_ifname, ifname))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode interface(get_node(ctx,
                               "/f:firewall/f:ethernet/f:interfaces/f:interface[@if='" + ifname + "']",
                               false));

    if (interface.is_empty())
    {
        xmlnode interfaces(get_node(ctx, "/f:firewall/f:ethernet/f:interfaces"));
        interface = append_node(interfaces, "interface");

        append_attribute(interface, "if", ifname);
        append_attribute(interface, "state", state);
    }
    else
    {
        update_attribute(interface, "if", ifname);
        update_attribute(interface, "state", state);
    }
}

static void rem_if(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& ifname(argv[0]);

    if (!is_match_std(regex::rex_ifname, ifname))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode interface(get_node(ctx,
                               "/f:firewall/f:ethernet/f:interfaces/f:interface[@if='" + ifname + "']",
                               false));

    if (!interface.is_empty())
        remove_node(std::move(interface));
}

static void toggle_eproto(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);

    if (!is_match_std(regex::rex_onoff, state))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode protocols(get_node(ctx, "/f:firewall/f:ethernet/f:protocols"));

    if (!protocols.is_empty())
    {
        update_attribute(protocols, "state", state);
    }
}

static void add_eproto(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& eproto(argv[0]);

    if (!is_match_std(regex::rex_eproto, eproto))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode protocol(get_node(ctx,
                     "/f:firewall/f:ethernet/f:protocols/f:protocol[@eproto='" + eproto + "']",
                     false));

    if (protocol.is_empty())
    {
        xmlnode protocols(get_node(ctx, "/f:firewall/f:ethernet/f:protocols"));
        protocol = append_node(protocols, "protocol");

        append_attribute(protocol, "eproto", eproto);
    }
    else
    {
        update_attribute(protocol, "eproto", eproto);
    }
}

static void rem_eproto(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& eproto(argv[0]);

    if (!is_match_std(regex::rex_eproto, eproto))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode protocol(get_node(ctx,
                              "/f:firewall/f:ethernet/f:protocols/f:protocol[@eproto='" + eproto + "']",
                              false));

    if (!protocol.is_empty())
        remove_node(std::move(protocol));
}

static void add_host(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (3 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);
    const std::string& mac(argv[1]);
    const std::string& mask(argv[2]);

    if (!is_match_std(regex::rex_onoff, state) ||
        !is_match_std(regex::rex_ifmac, mac) ||
        !is_match_opt(regex::rex_ifmac_mask, mask))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode whitelist(get_node(ctx, "/f:firewall/f:ethernet/f:whitelist"));
    xmlnode host(append_node(whitelist, "host"));

    append_attribute(host, "state", state);
    append_attribute(host, "mac", mac);
    if ("-" != mask)
        append_attribute(host, "mask", mask);
}

static void upd_host(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (4 != argv.size())
        throw invalid_param_error();

    const std::string& index(argv[0]);
    const std::string& state(argv[1]);
    const std::string& mac(argv[2]);
    const std::string& mask(argv[3]);

    if (!is_match_std(regex::rex_number, index) ||
        !is_match_std(regex::rex_onoff, state) ||
        !is_match_std(regex::rex_ifmac, mac) ||
        !is_match_opt(regex::rex_ifmac_mask, mask))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode host(get_node(ctx, "/f:firewall/f:ethernet/f:whitelist/f:host[" + index + "]", false));

    if (!host.is_empty())
    {
        update_attribute(host, "state", state);
        update_attribute(host, "mac", mac);
        updrem_attribute(host, "mask", mask);
    }
}

static void rem_host(xmldoc& doc, const std::vector<std::string>& argv)
{
    remove(doc, argv, "/f:firewall/f:ethernet/f:whitelist/f:host");
}

void process_ebtables(xmldoc& doc, const std::string& cmd, const std::vector<std::string>& argv)
{
    if ("--set-mode" == cmd)
        set_mode(doc, argv);
    else if ("--set-log" == cmd)
        set_log(doc, argv);
    else if ("--set-if" == cmd)
        set_if(doc, argv);
    else if ("--rem-if" == cmd)
        rem_if(doc, argv);
    else if ("--toggle-eproto" == cmd)
        toggle_eproto(doc, argv);
    else if ("--add-eproto" == cmd)
        add_eproto(doc, argv);
    else if ("--rem-eproto" == cmd)
        rem_eproto(doc, argv);
    else if ("--add-host" == cmd)
        add_host(doc, argv);
    else if ("--upd-host" == cmd)
        upd_host(doc, argv);
    else if ("--rem-host" == cmd)
        rem_host(doc, argv);
    else
        throw invalid_param_error("Unrecognized ebtables option");
}


} // namespace wago

