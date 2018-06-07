.. _sockets-echo-client:

Socket Echo Client Stress Example
#######################

Overview
********

The sockets/echo_client_stress/socket_echo_client sample application for Zephyr implements a simple
Echo client using a BSD Sockets compatible API. The purpose of this
sample is to debug the network risk condition issue in net_if_send_data

Requirements
************

STM32 NUCLEO-F429ZI board, linker and PC
I only test on nucleo_f429zi and mps2_an521 board (now zephyr master doesn't have mps2_an521 support).

Building and Running
********************

Build the Zephyr version of the application like this:

before build the sample, make sure you have configured the correct IP address in
proj.conf

cmake .. -DBOARD=nucleo_f429zi
make

Running application
=================================

1. start the echo server.
   on PC, run the echo_server_ipv4.py:
    python echo_server_ipv4.py
   and the echo server ocupies port 5555
2. run the sample on the board.
   the board runs case automatic

After several minutes you will get the following log:

[net/pkt] [DBG] net_pkt_frag_insert_debug: (0x200044fc): pkt 0x200067f8 frag 0x2000804c (net_ipv4_create_raw:40)
[net/pkt] [DBG] net_pkt_get_reserve_data_debug: (0x200044fc): TDATA (tx_bufs) [38] frag 0x2000807c reserve 14 ref 1 (prepare_segment():446)
[net/pkt] [DBG] net_pkt_frag_add_debug: (0x200044fc): pkt 0x200067f8 frag 0x2000807c (prepare_segment:458)
[net/pkt] [DBG] net_pkt_compact: (0x200044fc): Compacting data in pkt 0x200067f8
[net/pkt] [DBG] net_pkt_frag_del_debug: (0x200044fc): pkt 0x200067f8 parent 0x2000804c frag 0x2000807c ref 1 (net_pkt_compact:1162)
[net/tcp] [DBG] net_tcp_trace: (0x200044fc): [0x20004fb8] pkt 0x200067f8 src 41845 dst 5555 seq 0xde36278b (3728091019) ack 0x513887f1 (1362659313/536) flags uAprsf win 744 chk 0x90be
[net/tcp] [DBG] print_send_info: (0x200044fc): ACK sent to 192.168.1.100 port 5555
[net/ethernet] [DBG] net_eth_fill_header: (0x200044fc): iface 0x2001ca00 src 00:80:E1:49:11:DA dst 54:E1:AD:B7:FF:F0 type 0x800 len 40
[net/ethernet] [DBG] ethernet_send: (0x200044fc): Found ARP entry, sending pkt 0x200067f8 to iface 0x2001ca00
[net/if] [DBG] net_if_queue_tx: (0x200044fc): TC 1 with prio 1 pkt 0x200067f8
[net/core] [DBG] processing_data: (0x200044fc): Consumed pkt 0x20006630
[net/pkt] [DBG] net_pkt_print: (0x200044fc): TX 4 RX 19 RDATA 35 TDATA 39
[net/if] [DBG] net_if_tx: (0x2000493c): Processing (pkt 0x200067f8, data len 40, prio 1) network packet
[net/pkt] [DBG] net_pkt_unref_debug: (0x2000493c): TX [4] pkt 0x200067f8 ref 0 frags 0x2000804c (eth_tx():123)
[net/pkt] [DBG] net_pkt_unref_debug: (0x2000493c): TDATA (tx_bufs) [39] frag 0x2000804c ref 0 frags 0x00000000 (eth_tx():123)
[net/pkt] [DBG] net_pkt_frag_unref_debug: (0x2000493c): TDATA (tx_bufs) [39] frag 0x2000804c ref 0 (net_pkt_unref_debug():802)
[net/if] [DBG] net_if_tx: (0x2000493c): Calling context send cb 0x200039e4 token 0x00000000 status 0
4): iface 0x2001ca00 src 00:80:E1:49:11:DA dst 54:E1:AD:B7:FF:F0 type 0x800 len 6
[net/ethernet] [DBG] ethernet_send: (0x20005784): Found ARP entry, sending pkt 0x200067f8 to iface 0x2001ca00
[net/if] [DBG] net_if_queue_tx: (0x20005784): TC 1 with prio 1 pkt 0x200067f8
[net/if] [ERR] net_if_tx: {assert: 'pkt->ref != 0' failed}


