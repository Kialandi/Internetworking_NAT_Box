#include <xinu.h>

//accepts a string and converts to hex at buf
//Note: ignores ':'

int32 charToHex(byte * buf, char * string) {

    while (*string != 0) {
        //kprintf("char: %c\n", *string);
        if (*string == ':') {
            string++;
            continue;
        }
        if (*string >= '0' && *string <= '9')
            *buf = (*string - '0') << 4;
        else if (*string >= 'A' && *string <= 'Z')
            *buf = (*string - 'A' + 10) << 4;
        else if (*string >= 'a' && *string <= 'z')
            *buf = (*string - 'a' + 10) << 4;
        else
            return 1;
        
        string++;

        if (*string >= '0' && *string <= '9')
            *buf |= *string - '0';
        else if (*string >= 'A' && *string <= 'Z')
            *buf |= *string - 'A' + 10;
        else if (*string >= 'a' && *string <= 'z')
            *buf |= *string - 'a' + 10;
        else
            return 1;
        
        string++;
        buf++;
    }
    return 0;
}
