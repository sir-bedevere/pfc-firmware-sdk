//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process_iptables.cpp
/// 
/// \brief Function processing iptables configuration file.
/// 
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#include "process_iptables.hpp"

#include "error.hpp"
#include "process.hpp"
#include "regex.hpp"
#include <libxml/parser.h>


namespace wago
{


static void set_climits(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (5 != argv.size())
        throw invalid_param_error();

    const std::string& total(argv[0]);
    const std::string& limit(argv[1]);
    const std::string& burst(argv[2]);
    const std::string& tcp(argv[3]);
    const std::string& udp(argv[4]);

    if (!is_match_opt(regex::rex_conn_count, total) ||
        !is_match_opt(regex::rex_limit, limit) ||
        !is_match_opt(regex::rex_burst, burst) ||
        !is_match_opt(regex::rex_limit, tcp) ||
        !is_match_opt(regex::rex_limit, udp) ||
        ("-" == limit && "-" != burst))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));

    xmlnode climits(get_node(ctx, "/f:firewall/f:ipv4/f:climits"));

    updrem_attribute(climits, "total", total);
    updrem_attribute(climits, "limit", limit);
    updrem_attribute(climits, "burst", burst);
    updrem_attribute(climits, "tcp", tcp);
    updrem_attribute(climits, "udp", udp);
}

static void set_echo(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (4 != argv.size())
        throw invalid_param_error();

    const std::string& policy(argv[0]);
    const std::string& limit(argv[1]);
    const std::string& burst(argv[2]);
    const std::string& bprot(argv[3]);

    if (!is_match_std(regex::rex_policy, policy) ||
        !is_match_opt(regex::rex_limit, limit) ||
        !is_match_opt(regex::rex_burst, burst) ||
        ("-" == limit && "-" != burst) ||
        !is_match_std(regex::rex_onoff, bprot))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));

    xmlnode echo(get_node(ctx, "/f:firewall/f:ipv4/f:echo"));

    updrem_attribute(echo, "policy", policy);
    updrem_attribute(echo, "limit", limit);
    updrem_attribute(echo, "burst", burst);
    update_attribute(echo, "broadcast_protection", bprot);
}

static void set_echo_if(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (4 != argv.size())
        throw invalid_param_error();

    const std::string& policy(argv[0]);
    const std::string& ifname(argv[1]);
    const std::string& limit(argv[2]);
    const std::string& burst(argv[3]);

    if (!is_match_std(regex::rex_policy, policy) ||
        !is_match_std(regex::rex_ifname, ifname) ||
        !is_match_opt(regex::rex_limit, limit) ||
        !is_match_opt(regex::rex_burst, burst) ||
        ("-" == limit && "-" != burst))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode request(get_node(ctx, "/f:firewall/f:ipv4/f:echo/f:request[@if='" + ifname + "']", false));

    if (request.is_empty())
    {
        xmlnode echo(get_node(ctx, "/f:firewall/f:ipv4/f:echo"));
        request = append_node(echo, "request");

        append_attribute(request, "policy", policy);
        append_attribute(request, "if", ifname);
        if ("-" != limit)
            append_attribute(request, "limit", limit);
        if ("-" != burst)
            append_attribute(request, "burst", burst);
    }
    else
    {
        update_attribute(request, "policy", policy);
        updrem_attribute(request, "limit", limit);
        updrem_attribute(request, "burst", burst);
    }
}

static void rem_echo_if(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& ifname(argv[0]);

    if (!is_match_std(regex::rex_ifname, ifname))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode request(get_node(ctx, "/f:firewall/f:ipv4/f:echo/f:request[@if='" + ifname + "']", false));

    if (!request.is_empty())
        remove_node(std::move(request));
}

static void set_forward(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);

    if (!is_match_std(regex::rex_onoff, state))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));

    xmlnode forward(get_node(ctx, "/f:firewall/f:ipv4/f:forward"));

    update_attribute(forward, "state", state);
}

static void set_forward_link(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (3 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);
    const std::string& if1(argv[1]);
    const std::string& if2(argv[2]);

    if (!is_match_std(regex::rex_onoff, state) ||
        !is_match_std(regex::rex_ifname, if1) ||
        !is_match_std(regex::rex_ifname, if2))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode link;

    link = get_node(ctx,
                    "/f:firewall/f:ipv4/f:forward/f:fw/f:link[@if1='" + if1 + "' and @if2='" + if2 + "']",
                    false);
    if (link.is_empty())
        link = get_node(ctx,
                        "/f:firewall/f:ipv4/f:forward/f:fw/f:link[@if1='" + if2 + "' and @if2='" + if1 + "']",
                        false);

    if (link.is_empty())
    {
        xmlnode fw(get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:fw"));
        link = append_node(fw, "link");

        append_attribute(link, "state", state);
        append_attribute(link, "if1", if1);
        append_attribute(link, "if2", if2);
    }
    else
        update_attribute(link, "state", state);
}

static void rem_forward_link(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (2 != argv.size())
        throw invalid_param_error();

    const std::string& if1(argv[0]);
    const std::string& if2(argv[1]);

    if (!is_match_std(regex::rex_ifname, if1) ||
        !is_match_std(regex::rex_ifname, if2))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode link;

    link = get_node(ctx,
                    "/f:firewall/f:ipv4/f:forward/f:fw/f:link[@if1='" + if1 + "' and @if2='" + if2 + "']",
                    false);
    if (link.is_empty())
        link = get_node(ctx,
                        "/f:firewall/f:ipv4/f:forward/f:fw/f:link[@if1='" + if2 + "' and @if2='" + if1 + "']",
                        false);

    if (!link.is_empty())
        remove_node(std::move(link));
}

static void set_masq(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& ifname(argv[0]);

    if (!is_match_std(regex::rex_ifname, ifname))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode masq(get_node(ctx,
                          "/f:firewall/f:ipv4/f:forward/f:snat/f:masquerade[@if='" + ifname + "']",
                          false));

    if (masq.is_empty())
    {
        xmlnode snat(get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:snat"));
        masq = append_node(snat, "masquerade");

        append_attribute(masq, "if", ifname);
    }
}

static void rem_masq(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& ifname(argv[0]);

    if (!is_match_std(regex::rex_ifname, ifname))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));

    if ( "all" == argv[0] )
    {
        int const count = get_node_count(ctx, "/f:firewall/f:ipv4/f:forward/f:snat/f:masquerade");

        // Delete all nodes.
        for(int i = count; i > 0; i--)
        {
            std::vector<std::string> arg_index(1, std::to_string(i));
            remove(doc, arg_index, "/f:firewall/f:ipv4/f:forward/f:snat/f:masquerade");
        }
    }
    else
    {
      xmlnode masq(get_node(ctx,
                            "/f:firewall/f:ipv4/f:forward/f:snat/f:masquerade[@if='" + ifname + "']",
                            false));

      if (!masq.is_empty())
          remove_node(std::move(masq));
    }
}

static void add_pfw(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (7 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);
    const std::string& ifname(argv[1]);
    const std::string& proto(argv[2]);
    const std::string& dst_ip(argv[3]);
    const std::string& dst_port(argv[4]);
    const std::string& fw_ip(argv[5]);
    const std::string& fw_port(argv[6]);

    if (!is_match_std(regex::rex_onoff, state) ||
        !is_match_opt(regex::rex_ifname, ifname) ||
        !is_match_std(regex::rex_tcpudp, proto) ||
        !is_match_opt(regex::rex_ip4, dst_ip) ||
        !is_match_opt(regex::rex_port_range, dst_port) ||
        !is_match_opt(regex::rex_ip4, fw_ip) ||
        !is_match_opt(regex::rex_port_range, fw_port) ||
        ("-" == dst_ip && "-" == dst_port) ||
        ("-" == fw_ip && "-" == fw_port))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));

    xmlnode filter(get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:dnat"));
    int count;

    // Prevent adding duplicate entries.
    {
      std::string const ifname_proto_xpath = ("-" != ifname) ? ("/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port[@if='" +
                                                                ifname + "' and @proto='" + proto) : "";
      std::string const dst_ip_xpath = ("-" != dst_ip)? ("' and @dst_ip='" + dst_ip) : "";
      std::string const dst_port_xpath = ("-" != dst_port)? ("' and @dst_port='" + dst_port) : "";
      std::string const fw_ip_xpath = ("-" != fw_ip)? ("' and @fw_ip='" + fw_ip) : "";
      std::string const fw_port_xpath = ("-" != fw_port)? ("' and @fw_port='" + fw_port) : "";

      count = get_node_count(ctx, ifname_proto_xpath + dst_ip_xpath + dst_port_xpath + fw_ip_xpath +fw_port_xpath+"']");
    }

    if ( 0 == count )
    {
      xmlnode rule(append_node(filter, "fw_port"));

      append_attribute(rule, "state", state);
      if ("-" != ifname)
          append_attribute(rule, "if", ifname);
          append_attribute(rule, "proto", proto);
      if ("-" != dst_ip)
          append_attribute(rule, "dst_ip", dst_ip);
      if ("-" != dst_port)
          append_attribute(rule, "dst_port", dst_port);
      if ("-" != fw_ip)
          append_attribute(rule, "fw_ip", fw_ip);
      if ("-" != fw_port)
          append_attribute(rule, "fw_port", fw_port);
    }
}

static void upd_pfw(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (8 != argv.size())
        throw invalid_param_error();

    const std::string& index(argv[0]);
    const std::string& state(argv[1]);
    const std::string& ifname(argv[2]);
    const std::string& proto(argv[3]);
    const std::string& dst_ip(argv[4]);
    const std::string& dst_port(argv[5]);
    const std::string& fw_ip(argv[6]);
    const std::string& fw_port(argv[7]);

    if (!is_match_std(regex::rex_number, index) ||
        !is_match_std(regex::rex_onoff, state) ||
        !is_match_opt(regex::rex_ifname, ifname) ||
        !is_match_std(regex::rex_tcpudp, proto) ||
        !is_match_opt(regex::rex_ip4, dst_ip) ||
        !is_match_opt(regex::rex_port_range, dst_port) ||
        !is_match_opt(regex::rex_ip4, fw_ip) ||
        !is_match_opt(regex::rex_port_range, fw_port) ||
        ("-" == dst_ip && "-" == dst_port) ||
        ("-" == fw_ip && "-" == fw_port))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));

    xmlnode fwp(get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port[" + index + "]", false));

    if (fwp.is_empty())
        return;

    update_attribute(fwp, "state", state);
    updrem_attribute(fwp, "if", ifname);
    update_attribute(fwp, "proto", proto);
    updrem_attribute(fwp, "dst_ip", dst_ip);
    updrem_attribute(fwp, "dst_port", dst_port);
    updrem_attribute(fwp, "fw_ip", fw_ip);
    updrem_attribute(fwp, "fw_port", fw_port);
}

static void rem_pfw(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
          throw invalid_param_error();

    if ( "all" == argv[0] )
    {
      xmlctx ctx(get_ctx(doc));
      int const count = get_node_count(ctx, "/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port");

      // Delete all nodes.
      for(int i = count; i > 0; i--)
      {
        std::vector<std::string> arg_index(1, std::to_string(i));
        remove(doc, arg_index, "/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port");
      }
    }
    else
    {
      remove(doc, argv, "/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port");
    }
}

static void set_open_if(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (2 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);
    const std::string& ifname(argv[1]);

    if (!is_match_std(regex::rex_onoff, state) ||
        !is_match_std(regex::rex_ifname, ifname))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode interface;

    interface = get_node(ctx, "/f:firewall/f:ipv4/f:input/f:open/f:interface[@if='" + ifname + "']", false);

    if (interface.is_empty())
    {
        xmlnode open(get_node(ctx, "/f:firewall/f:ipv4/f:input/f:open"));
        interface = append_node(open, "interface");

        append_attribute(interface, "state", state);
        append_attribute(interface, "if", ifname);
    }
    else
        update_attribute(interface, "state", state);
}

static void rem_open_if(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (1 != argv.size())
        throw invalid_param_error();

    const std::string& ifname(argv[0]);

    if (!is_match_std(regex::rex_ifname, ifname))
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));
    xmlnode interface;

    interface = get_node(ctx, "/f:firewall/f:ipv4/f:input/f:open/f:interface[@if='" + ifname + "']", false);

    if (!interface.is_empty())
        remove_node(std::move(interface));
}

static void add_filter(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (10 != argv.size())
        throw invalid_param_error();

    const std::string& state(argv[0]);
    const std::string& ifname(argv[1]);
    const std::string& proto(argv[2]);
    const std::string& src_ip(argv[3]);
    const std::string& src_mask(argv[4]);
    const std::string& src_port(argv[5]);
    const std::string& dst_ip(argv[6]);
    const std::string& dst_mask(argv[7]);
    const std::string& dst_port(argv[8]);
    const std::string& policy(argv[9]);

    if (!is_match_std(regex::rex_onoff, state) ||
        !is_match_opt(regex::rex_ifname, ifname) ||
        !is_match_std(regex::rex_tcpudp, proto) ||
        !is_match_opt(regex::rex_ip4, src_ip) ||
        !is_match_opt(regex::rex_ip4mask, src_mask) ||
        !is_match_opt(regex::rex_port_range, src_port) ||
        !is_match_opt(regex::rex_ip4, dst_ip) ||
        !is_match_opt(regex::rex_ip4mask, dst_mask) ||
        !is_match_opt(regex::rex_port_range, dst_port) ||
        !is_match_std(regex::rex_policy, policy) ||
        ("-" == proto && ("-" != src_port || "-" != dst_port)) ||
        ("-" == src_ip && "-" != src_mask) ||
        ("-" == dst_ip && "-" != dst_mask))
        throw invalid_param_error();

    if ("-" == ifname &&
        "-" == src_ip && "-" == src_port &&
        "-" == dst_ip && "-" == dst_port)
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));

    xmlnode filter(get_node(ctx, "/f:firewall/f:ipv4/f:input/f:filter"));
    xmlnode rule(append_node(filter, "rule"));

    append_attribute(rule, "state", state);
    if ("-" != ifname)
        append_attribute(rule, "if", ifname);
    if ("-" != proto)
        append_attribute(rule, "proto", proto);
    if ("-" != src_ip)
        append_attribute(rule, "src_ip", src_ip);
    if ("-" != src_mask)
        append_attribute(rule, "src_mask", src_mask);
    if ("-" != src_port)
        append_attribute(rule, "src_port", src_port);
    if ("-" != dst_ip)
        append_attribute(rule, "dst_ip", dst_ip);
    if ("-" != dst_mask)
        append_attribute(rule, "dst_mask", dst_mask);
    if ("-" != dst_port)
        append_attribute(rule, "dst_port", dst_port);
    append_attribute(rule, "policy", policy);
}

static void upd_filter(xmldoc& doc, const std::vector<std::string>& argv)
{
    if (10 != argv.size())
        throw invalid_param_error();

    const std::string& index(argv[0]);
    const std::string& state(argv[1]);
    const std::string& ifname(argv[2]);
    const std::string& proto(argv[3]);
    const std::string& src_ip(argv[4]);
    const std::string& src_mask(argv[5]);
    const std::string& src_port(argv[6]);
    const std::string& dst_ip(argv[7]);
    const std::string& dst_mask(argv[8]);
    const std::string& dst_port(argv[9]);

    if (!is_match_std(regex::rex_number, index) ||
        !is_match_std(regex::rex_onoff, state) ||
        !is_match_opt(regex::rex_ifname, ifname) ||
        !is_match_std(regex::rex_tcpudp, proto) ||
        !is_match_opt(regex::rex_ip4, src_ip) ||
        !is_match_opt(regex::rex_ip4mask, src_mask) ||
        !is_match_opt(regex::rex_port_range, src_port) ||
        !is_match_opt(regex::rex_ip4, dst_ip) ||
        !is_match_opt(regex::rex_ip4mask, dst_mask) ||
        !is_match_opt(regex::rex_port_range, dst_port) ||
        ("-" == proto && ("-" != src_port || "-" != dst_port)) ||
        ("-" == src_ip && "-" != src_mask) ||
        ("-" == dst_ip && "-" != dst_mask))
        throw invalid_param_error();

    if ("-" == ifname &&
        "-" == src_ip && "-" == src_port &&
        "-" == dst_ip && "-" == dst_port)
        throw invalid_param_error();

    xmlctx ctx(get_ctx(doc));

    xmlnode rule(get_node(ctx, "/f:firewall/f:ipv4/f:input/f:filter/f:rule[" + index + "]", false));

    if (rule.is_empty())
        return;

    update_attribute(rule, "state", state);
    updrem_attribute(rule, "if", ifname);
    updrem_attribute(rule, "proto", proto);
    updrem_attribute(rule, "src_ip", src_ip);
    updrem_attribute(rule, "src_mask", src_mask);
    updrem_attribute(rule, "src_port", src_port);
    updrem_attribute(rule, "dst_ip", dst_ip);
    updrem_attribute(rule, "dst_mask", dst_mask);
    updrem_attribute(rule, "dst_port", dst_port);
}

static void rem_filter(xmldoc& doc, const std::vector<std::string>& argv)
{
    remove(doc, argv, "/f:firewall/f:ipv4/f:input/f:filter/f:rule");
}

void process_iptables(xmldoc& doc, const std::string& cmd, const std::vector<std::string>& argv)
{
    if ("--set-climits" == cmd)
        set_climits(doc, argv);
    else if ("--set-echo" == cmd)
        set_echo(doc, argv);
    else if ("--set-echo-if" == cmd)
        set_echo_if(doc, argv);
    else if ("--rem-echo-if" == cmd)
        rem_echo_if(doc, argv);
    else if ("--set-forward" == cmd)
        set_forward(doc, argv);
    else if ("--set-forward-link" == cmd)
        set_forward_link(doc, argv);
    else if ("--rem-forward-link" == cmd)
        rem_forward_link(doc, argv);
    else if ("--set-masq" == cmd)
        set_masq(doc, argv);
    else if ("--rem-masq" == cmd)
        rem_masq(doc, argv);
    else if ("--add-pfw" == cmd)
        add_pfw(doc, argv);
    else if ("--upd-pfw" == cmd)
        upd_pfw(doc, argv);
    else if ("--rem-pfw" == cmd)
        rem_pfw(doc, argv);
    else if ("--set-open-if" == cmd)
        set_open_if(doc, argv);
    else if ("--rem-open-if" == cmd)
        rem_open_if(doc, argv);
    else if ("--add-filter" == cmd)
        add_filter(doc, argv);
    else if ("--upd-filter" == cmd)
        upd_filter(doc, argv);
    else if ("--rem-filter" == cmd)
        rem_filter(doc, argv);
    else
        throw invalid_param_error();
}


} // namespace wago

