/*
 * sdnagent.{cc,hh} -- An agent for the wireless sdn system
 * Yanhe Liu <yanhe.liu@cs.helsinki.fi>
 *
 * Copyright (c) 2013 University of Helsinki
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#ifndef CLICK_SDNAGENT_HH
#define CLICK_SDNAGENT_HH

#include <click/element.hh>
#include <click/hashtable.hh> 
#include <click/etheraddress.hh>
#include <click/ipaddress.hh>
#include <click/ewma.hh>
#include <click/timer.hh>

#include "dhcp/leasetable.hh"

#define UDP_CLIENT_PORT 7755
#define UDP_AGENT_PORT 7766

CLICK_DECLS

/*
=c
SdnAgent

=s sdnagent
wireless sdn local agent -- monitoring client traffic

=d
Acts as an agent for the sdn controller

=a
to be added...
*/

typedef RateEWMAX<RateEWMAXParameters<10, 4> > byte_rate_t;

class SdnAgent : public Element {
  public:
    SdnAgent();
    ~SdnAgent();

    // From Click
    const char *class_name() const  { return "SdnAgent"; }
    const char *port_count() const  { return "6/3"; }
    const char *processing() const  { return PUSH; }
    
    int initialize(ErrorHandler *); // initialize element
    int configure(Vector<String> &, ErrorHandler *);
    void run_timer(Timer *timer);
    void push(int port, Packet *p);

    // client info
    class Client {
      public:
        EtherAddress _mac;  // mac address
        IPAddress _ipaddr;  // ipv4 address
        
        int _ping_lost_num;
    
        // rate not used anymore
        // float _byte_up_rate;   // client rate
        // float _byte_down_rate;
        // long _byte_up_count;
        // long _byte_down_count;
    };

    void add_client(EtherAddress eth, IPAddress ip);
    // void remove_client();

  private:  
    EtherAddress _mac;  // mac address
    IPAddress _ipaddr;  // ipv4 control iface address
    IPAddress _ap_ipaddr;    // hostapd iface address
    Timer _timer;
    int _interval;
    uint32_t _count;         // ping keep alive packet count
    uint16_t _icmp_sequence;
    // byte_rate_t _byte_up_rate;   // agent overall rate
    // byte_rate_t _byte_down_rate;

    // dhcp
    void send_dhcp_ack_or_nak(int port, Packet *p);
    Packet *make_ack_packet(Packet *p, Lease *lease);
    Packet *make_nak_packet(Packet *p, Lease *lease);
    DHCPLeaseTable *_leases;  // dhcp lease pool

    // communicate with client
    void push_udp_to_client(Packet *p_in, IPAddress ip_dst, 
                            EtherAddress eth_dst, int port);

    // client disconnect
    void disconnect_responder(struct click_wifi *w);
    
    // client keep alive ping test
    Packet *make_ping_request(IPAddress ip_dst, EtherAddress eth_dst);
    
    // client info
    HashTable<EtherAddress, Client> _client_table;

    // for calling external shell script
    String _cmd;
};

CLICK_ENDDECLS
#endif
