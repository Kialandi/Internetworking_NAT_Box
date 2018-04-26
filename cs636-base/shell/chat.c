#include "xinu.h"
shellcmd chat(int nargs, char * args[]){
   kprintf("%s", args[1]);
}
