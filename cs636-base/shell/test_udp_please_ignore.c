#include "xinu.h"

shellcmd chats(int nargs, char * args[]){
    struct chat * makechat = (struct chat*) getbuf(ipv6bufpool);
    uint32 chatlength = sizeof(args)/sizeof(char);
    char chatbuff[chatlength];
    memset(chatbuff, NULLCH, chatlength);
    for(int32 i=0; i<chatlength;i++){

    }
    sendipv6pkt()

}
