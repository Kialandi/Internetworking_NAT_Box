Project Description


  Commercial vendors use the term "wireless router"; for a device that
connects to the Internet, obtains one globally-valid IP address, and then
allows other devices to connect through it.  The other devices are given
temporary addresses that are not globally-valid, and NAT software
translates addresses as packets pass through it.  As a result, devices
behind the NAT box use standard protocols to obtain an IP address, and
they think that they have been assigned a valid address.  They can send
packets out on the Internet and receive replies.

   You will build a NAT box (wireless router) that uses IPv6. For the
project, the Xinu lab Ethernet will be our Internet connection.  Your
NAT box will attach to the lab Ethernet, use standard IPv6 protocols,
and obtain a valid IPv6 address.  Your NAT box will then allow hosts
on two other networks to use standard IPv6 protocols to obtain their
own address.  For testing, you may also choose to configure a host on
the lab network.


                   Lab Ethernet -- Internet connection (net 0)
------------------------------------------------------------------------
                               |                        |
                               |                      ----
                               |                     |    |
                               |                      ----
                         -------------            host on lab Ethernet
                        |             |
                        |   Your NAT  |
                        |     box     |
                        |             |
                         -------------
                           |       |
                           |       |
                           |       |
        Othernet 1 (net 1) |       | Othernet 2 (net 2)
      -----------------------    -----------------------
            |         |                 |         |
          ----      ----              ----      ----
         |    | ...|    |            |    | ...|    |
          ----      ----              ----      ----
        hosts on Othernet 1         hosts on Othernet 1


What you will find in the base code:

   In addition to sources for Xinu (Galileo version), there is code that
provides three network interfaces.  A network is accessed through the
if_tab array.  Entries in the array are:

	0 - The lab Ethernet
	1 - An Othernet1 (an emulated network that is Ethernet-like)
	2 - An Othernet2 (an emulated network that is Ethernet-like)

The emulated networks are like an Ethernet in the sense that the frame
format, maximum payload (1500 octets) and header fields are exactly the
same as Ethernet.  Some of the MAC addressing differs.  Each interface
has a unique 48-bit MAC address as usual.   In place of the all-1s
broadcast address, however, Othernet hardware uses a special broadcast
address.  When your code starts running, the interface table will already
be initialized.  You will find a 48-bit MAC address for each network
interface in the if_tab entry for the interface.

The computer's MAC address on the lab Ethernet is in

	if_tab[0].if_macucast

the Othernet 1 interface MAC address is in:

	field if_tab[1].if_macucast

and the Othernet 2 interface MAC address is in:

	if_tab[2].if_macucast

Similarly, the MAC broadcast address for interface X is in

	if_tab[X].if_macbcast

For the lab Ethernet, the MAC broadcast address is all 1's, but for the
Othernets, it is no.  So, instead of filling in all 1's, always obtain the
broadcast address from the interface table.

One Code Base To Rule Them All

   Because many pieces of code are used for both host protocols and NAT
protocols, there is only one code base.  It can run as a host or as a
NAT box.  When you build your code, you will build it all in one code
base, and then have a few run-time checks to see how to process packets.

   You are given initialization code that configures the interface table
at startup.  When the system boots, the user is prompted for a Bind ID
(the ID you were assigned in class), and then asked whether the system
should be configured to  be a host or a router.  If a host, the user must
specify the interface (a host on the lab Ethernet, a host or Othernet 1,
or a host of Othernet 2).  You can create multiple instances of hosts on
the three networks (just use cs-console to grab a new Xinu backend, and
download the image into to.  The initialization code does *not* handle
IPv6 for you -- you have to write that code.  You will need to use IPv6
protocols to obtain an IPv6 address for each network, and put each IPv6
address in the interface table.  The only thing the initialization code
does for you is generate the necessary MAC addresses and set field
if_state in each interface to indicate whether that interface is "up" or
"down".  For a host, only one interface will be "up"; for a wireless
router, all interfaces will be "up".  The initialization code also sets
global variable ifprime to the index of the primary interface (0 for a
wireless router and 0, 1 or 2 for a host).


