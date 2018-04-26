#include "xinu.h"
shellcmd chat(int nargs, char * args[]){

    sendipv6pkt(CHAT, allnMACmulti, allnIPmulti, args);

}
