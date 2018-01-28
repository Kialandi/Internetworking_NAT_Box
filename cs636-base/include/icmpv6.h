/* icmpv6.h - definintions for the Internet Control Message Protocol */


enum errormsg{
    UNREACHABLE,
    TOO_BIG,
    TIME_EXCEEDED,
    PARAM_PROB,
    PRIVATE_EXP1,
    PRIVATE_EXP2
};

enum info{
    ECHOREQ,
    ECHOREP,
    PRIVATE_EXP1,
    PRIVATE_EXP2
}

enum itype{
    NSOLICIT,
    NADVERTISE,
    RSOLICIT,
    RADVERTISE
}

typedef struct {
    unsigned int icmpv6src:    128; /*source addr*/
    unsigned int icmpv6dst:    128; /*dest addr*/
    unsigned int icmpv6len:    32;  /*len of icmp msg */
    unsigned int zero:         24;  /*init w zeros */
    unsigned int next:         8;   /* nxt header */
} icmpv6hdr;

typedef struct{
   unsigned int itype:      8; /*type code */
   unsigned int code:       8; /* icmp code */
   unsigned int cksum:      16; /* checksum */
   unsigned int message:    32; /* icmp msg */
} icmpv6msg;

