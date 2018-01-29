#include <xinu.h>

syscall bootipv6() {
    //TODO: zero out the 3 fields

    //Kathy's functions to fill out
    //TODO: link-local unicast, SNM, and MAC SNM
    //TODO: sanity check for the output of functions
    //if (bad) return SYSERR;
    //tell hardware to listen to MAC SNM
    control(ETHER0, ETH_CTRL_ADD_MCAST, (int32)mac_snm, 0);

    //TODO; Send solicitation message
    ipv6bootstrap = 1;
    sendipv6pkt();
    
    //TODO: receive router advertisement -- probably in netin
    //TODO: parse router advertisement -- probably in netin
    //maybe add a wait call here or a timer 
    
    //TODO: accept router offer -- only if offer received
    
    //TODO: remember to use hton for integers going to the network

    return OK;
}
