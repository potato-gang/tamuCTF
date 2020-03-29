#!/usr/bin/env python3

import sys
import base64
import dpkt

c = 0
i = 0
b64 = ""

for ts, pkt in dpkt.pcap.Reader(open("netlogs.pcap", "rb")):
    c += 1
    ethpkt = dpkt.ethernet.Ethernet(pkt)
        
    if not isinstance(ethpkt.data, dpkt.ip.IP):
        continue
    
    ip = ethpkt.data
    
    if ip.p != dpkt.ip.IP_PROTO_UDP or not isinstance(ip.data, dpkt.udp.UDP):
        continue
        
    udp = ip.data
    
    if udp.dport != 53:
        continue
        
    dns = dpkt.dns.DNS(udp.data)
        
    if len(dns.qd) != 1:
        continue
        
    i += 1
    b64 += (dns.qd[0].name[:-len("-tamu.1e100.net")] + "\n")
    
with open("stage0", "w") as out:
    out.write(b64)
    
if i != 37991:
    print(f"{abs(37991-i)} lines off", file=sys.stderr)
    
