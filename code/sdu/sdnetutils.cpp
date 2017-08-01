#include "SDuPCH.h"
#include "sdnetutils.h"
#include "sdstring.h"

#ifndef IN6ADDRSZ
#define IN6ADDRSZ   16
#endif

#ifndef INT16SZ
#define INT16SZ sizeof(int16)
#endif

#ifndef INADDRSZ
#define INADDRSZ    4
#endif

#ifndef __P
#define __P(x) x
#endif

namespace Crown
{

    static const char *SDInetNtop4 __P((const unsigned char *src, char *dst, size_t size));
#if SD_HAVE_IPV6
    static const char *SDInetNtop6 __P((const unsigned char *src, char *dst, size_t size));
#endif

    /* char *
    * SDNetInetNtop(af, src, dst, size)
    *	convert a network format address to presentation format.
    * return:
    *	pointer to presentation format address (`dst'), or nullptr (see errno).
    */
    const char * SDAPI
    SDNetInetNtop(int32 af, const void *src, char *dst, size_t size)
    {
        switch (af)
        {
        case AF_INET:
            return (SDInetNtop4((const unsigned char*)src, dst, size));
#if SD_HAVE_IPV6
        case AF_INET6:
            return (SDInetNtop6((const unsigned char*)src, dst, size));
#endif
        default:

            return nullptr;
        }
    }


    /* const char *
    * SDInetNtop4(src, dst, size)
    *	format an IPv4 address, more or less like inet_ntoa()
    * return:
    *	`dst' (as a const)
    * notes:
    *	(1) uses no statics
    *	(2) takes a u_char* not an in_addr as input

    */
    static const char * 
    SDInetNtop4(const unsigned char *src, char *dst, size_t size)
    {
        const size_t MIN_SIZE = 16; /* space for 255.255.255.255\0 */
        int32 n = 0;
        char *next = dst;

        if (size < MIN_SIZE)
        {
            return nullptr;
        }
        do
        {
            unsigned char u = *src++;
            if (u > 99)
            {
                *next++ = '0' + u/100;
                u %= 100;
                *next++ = '0' + u/10;
                u %= 10;
            }
            else if (u > 9)
            {
                *next++ = '0' + u/10;
                u %= 10;
            }
            *next++ = '0' + u;
            *next++ = '.';
            n++;
        }
        while (n < 4);
        *--next = 0;
        return dst;
    }



#if SD_HAVE_IPV6
    /* const char *
    * SDInetNtop6(src, dst, size)
    *	convert IPv6 binary address into presentation (printable) format
    */
    static const char *
    SDInetNtop6(const unsigned char *src, char *dst, size_t size)
    {
        /*
        * Note that int32_t and int16_t need only be "at least" large enough
        * to contain a value of the specified size.  On some systems, like
        * Crays, there is no such thing as an integer variable with 16 bits.
        * Keep this in mind if you think this function should have been coded
        * to use pointer overlays.  All the world's not a VAX.
        */
        char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
        struct
        {
            int32 base, len;
        } best = {-1, 0}, cur = {-1, 0};
        uint32 words[IN6ADDRSZ / INT16SZ];
        int32 i;
        const unsigned char *next_src, *src_end;
        uint32 *next_dest;

        /*
        * Preprocess:
        *	Copy the input (bytewise) array into a wordwise array.
        *	Find the longest run of 0x00's in src[] for :: shorthanding.
        */
        next_src = src;
        src_end = src + IN6ADDRSZ;
        next_dest = words;
        i = 0;
        do
        {
            uint32 next_word = (uint32)*next_src++;
            next_word <<= 8;
            next_word |= (uint32)*next_src++;
            *next_dest++ = next_word;

            if (next_word == 0)
            {
                if (cur.base == -1)
                {
                    cur.base = i;
                    cur.len = 1;
                }
                else
                {
                    cur.len++;
                }
            }
            else
            {
                if (cur.base != -1)
                {
                    if (best.base == -1 || cur.len > best.len)
                    {
                        best = cur;
                    }
                    cur.base = -1;
                }
            }

            i++;
        }
        while (next_src < src_end);

        if (cur.base != -1)
        {
            if (best.base == -1 || cur.len > best.len)
            {
                best = cur;
            }
        }
        if (best.base != -1 && best.len < 2)
        {
            best.base = -1;
        }

        /*
        * Format the result.
        */
        tp = tmp;
        for (i = 0; i < (IN6ADDRSZ / INT16SZ);)
        {
            /* Are we inside the best run of 0x00's? */
            if (i == best.base)
            {
                *tp++ = ':';
                i += best.len;
                continue;
            }
            /* Are we following an initial run of 0x00s or any real hex? */
            if (i != 0)
            {
                *tp++ = ':';
            }
            /* Is this address an encapsulated IPv4? */
            if (i == 6 && best.base == 0 &&
                    (best.len == 6 || (best.len == 5 && words[5] == 0xffff)))
            {
                if (!SDInetNtop4(src+12, tp, sizeof tmp - (tp - tmp)))
                {
                    return (nullptr);
                }
                tp += strlen(tp);
                break;
            }
            tp += SDSnprintf(tp, sizeof tmp - (tp - tmp), "%x", words[i]);
            i++;
        }
        /* Was it a trailing run of 0x00's? */
        if (best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
        {
            *tp++ = ':';
        }
        *tp++ = '\0';

        /*
        * Check for overflow, copy, and we're done.
        */
        if ((size_t)(tp - tmp) > size)
        {
            return (nullptr);
        }
        strcpy(dst, tmp);
        return (dst);
    }

#endif // HAS_IPV6





    /*
    * WARNING: Don't even consider trying to compile this on a system where
    * sizeof(int32) < 4.  sizeof(int32) > 4 is fine; all the world's not a VAX.
    */

    static bool SDInetPton4 __P((const char *src, unsigned char *dst));
#if SD_HAVE_IPV6
    static bool SDInetPton6 __P((const char *src, unsigned char *dst));
#endif

    /* int32
    * inet_pton(af, src, dst)
    *	convert from presentation format (which usually means ASCII printable)
    *	to network format (which is usually some kind of binary format).
    * return:
    *	1 if the address was valid for the specified address family
    *	0 if the address wasn't valid (`dst' is untouched in this case)
    *	-1 if some other error occurred (`dst' is untouched in this case, too)
    */
    bool SDAPI SDNetInetPton(int32 af, const char *src, void *dst)
    {
        switch (af)
        {
        case AF_INET:
            return (SDInetPton4(src, (unsigned char*)dst));
#if SD_HAVE_IPV6
        case AF_INET6:
            return (SDInetPton6(src, (unsigned char*)dst));
#endif
        default:
            ;
        }
        return false;
    }

    /* int32
    * SDInetPton4(src, dst)
    *	like inet_aton() but without all the hexadecimal and shorthand.
    * return:
    *	1 if `src' is a valid dotted quad, else 0.
    * notice:
    *	does not touch `dst' unless it's returning 1.
    * author:
    *	Paul Vixie, 1996.
    */
    static bool SDInetPton4(const char *src, unsigned char *dst)
    {
        static const char digits[] = "0123456789";
        int32 saw_digit, octets, ch;
        unsigned char tmp[INADDRSZ], *tp;

        saw_digit = 0;
        octets = 0;
        *(tp = tmp) = 0;
        while ((ch = *src++) != '\0')
        {
            const char *pch;

            if ((pch = SDStrchr(digits, ch)) != nullptr)
            {
                uint32 value = *tp * 10 + (uint32)(pch - digits);

                if (value > 255)
                    return (0);
                *tp = value;
                if (! saw_digit)
                {
                    if (++octets > 4)
                        return (0);
                    saw_digit = 1;
                }
            }
            else if (ch == '.' && saw_digit)
            {
                if (octets == 4)
                    return (0);
                *++tp = 0;
                saw_digit = 0;
            }
            else
                return false;
        }
        if (octets < 4)
            return false;

        memcpy(dst, tmp, INADDRSZ);
        return true;
    }

#if SD_HAVE_IPV6
    /* int32
    * SDInetPton6(src, dst)
    *	convert presentation level address to network order binary form.
    * return:
    *	1 if `src' is a valid [RFC1884 2.2] address, else 0.
    * notice:
    *	(1) does not touch `dst' unless it's returning 1.
    *	(2) :: in a full address is silently ignored.
    * credit:
    *	inspired by Mark Andrews.
    * author:
    *	Paul Vixie, 1996.
    */
    static bool
    SDInetPton6(const char *src, unsigned char *dst)
    {
        static const char xdigits_l[] = "0123456789abcdef",
                                        xdigits_u[] = "0123456789ABCDEF";
        unsigned char tmp[IN6ADDRSZ], *tp, *endp, *colonp;
        const char *xdigits, *curtok;
        int32 ch, saw_xdigit;
        uint32 val;

        memset((tp = tmp), '\0', IN6ADDRSZ);
        endp = tp + IN6ADDRSZ;
        colonp = nullptr;
        /* Leading :: requires some special handling. */
        if (*src == ':')
            if (*++src != ':')
                return (0);
        curtok = src;
        saw_xdigit = 0;
        val = 0;
        while ((ch = *src++) != '\0')
        {
            const char *pch;

            if ((pch = strchr((xdigits = xdigits_l), ch)) == nullptr)
                pch = strchr((xdigits = xdigits_u), ch);
            if (pch != nullptr)
            {
                val <<= 4;
                val |= (pch - xdigits);
                if (val > 0xffff)
                    return (0);
                saw_xdigit = 1;
                continue;
            }
            if (ch == ':')
            {
                curtok = src;
                if (!saw_xdigit)
                {
                    if (colonp)
                        return (0);
                    colonp = tp;
                    continue;
                }
                if (tp + INT16SZ > endp)
                    return (0);
                *tp++ = (unsigned char) (val >> 8) & 0xff;
                *tp++ = (unsigned char) val & 0xff;
                saw_xdigit = 0;
                val = 0;
                continue;
            }
            if (ch == '.' && ((tp + INADDRSZ) <= endp) &&
                    SDInetPton4(curtok, tp) > 0)
            {
                tp += INADDRSZ;
                saw_xdigit = 0;
                break;	/* '\0' was seen by SDInetPton4(). */
            }
            return (0);
        }
        if (saw_xdigit)
        {
            if (tp + INT16SZ > endp)
                return false;
            *tp++ = (unsigned char) (val >> 8) & 0xff;
            *tp++ = (unsigned char) val & 0xff;
        }
        if (colonp != nullptr)
        {
            /*
            * Since some memmove()'s erroneously fail to handle
            * overlapping regions, we'll do the shift by hand.
            */
            const int32 n = tp - colonp;

            for (int32 i = 1; i <= n; i++)
            {
                endp[- i] = colonp[n - i];
                colonp[n - i] = 0;
            }
            tp = endp;
        }
        if (tp != endp)
            return false;
        memcpy(dst, tmp, IN6ADDRSZ);
        return true;
    }
#endif

    int32 SDAPI SDGetIpFromName(const char * name)
    {
        struct hostent* host = gethostbyname(name);
        if (host == nullptr || host->h_addr_list[0] == nullptr)
        {
            return 0;
        }
        struct in_addr * addr = (struct in_addr *)host->h_addr_list[0];
#if (defined(WIN32) || defined(WIN64))
        return addr->S_un.S_addr;
#else
        return addr->s_addr;
#endif
    }

    std::string SDAPI SDGetHostName()
    {
        static char szHostName[256];
        int32 ret = gethostname(szHostName,sizeof(szHostName));
        if (ret == 0)
        {
            return szHostName;
        }
        return "";
    }

    SDHostent* SDAPI SDGetHostByName(const char * name )
    {
        return gethostbyname(name);
    }

    int32 SDAPI SDGetLocalIp(unsigned long ip[], int32 count)
    {
        int32 ipcount = 0;

#if (defined(WIN32) || defined(WIN64))
        char hostname[1024];
        if (gethostname(hostname, sizeof(hostname)) != 0)
        {
            return 0;
        }

        LPHOSTENT lphost = gethostbyname(hostname);
        if (lphost == nullptr)
        {
            return 0;
        }

        struct in_addr *addr;
        while(lphost->h_addr_list[ipcount] != nullptr && count-- > 0)
        {
            addr=(struct in_addr *)lphost->h_addr_list[ipcount];
            ip[ipcount++] = addr->S_un.S_addr;
        }

#else
        int32 fd;
        int32 interface;
        struct ifreq buf[10];

        struct ifconf ifc;

        if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            return 0;
        }

        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (char*) buf;
        if (ioctl(fd, SIOCGIFCONF, (char *) &ifc))
        {
            return 0;
        }

        interface = ifc.ifc_len / sizeof(struct ifreq);
        interface = interface < 10 ? interface : 10;
        int32 iplocal = SDInetAddr("127.0.0.1");
        while (interface-- > 0 && count-- > 0)
        {
            /*Get IP of the net card */
            if ((ioctl(fd, SIOCGIFADDR, (char *) &buf[interface])))
            {
                continue;
            }

            long tmp_ip = ((struct sockaddr_in*)(&buf[interface].ifr_addr))->sin_addr.s_addr;
            if(tmp_ip == iplocal)
            {
                continue;
            }
            //printf("%s\n", inet_ntoa(((struct sockaddr_in*)(&buf[interface].ifr_addr))->sin_addr));
            ip[ipcount++] = tmp_ip;
        }
#endif
        return ipcount;
    }
    


 
#ifdef __linux__
	// 设置文件句柄数限制
	// 返回值如下：
	//  0: success
	// -1: getrlimit failed
	// -2: setrlimit failed
	int32 SetFileHandleLimit(const uint32 dwSetLimit)
	{
		rlimit rl;
		int ret = getrlimit(RLIMIT_NOFILE, &rl);
		if (-1 == ret)
		{
			return -1;
		}
		
		
		rl.rlim_cur = dwSetLimit;
		rl.rlim_max = dwSetLimit;
		ret = setrlimit(RLIMIT_NOFILE, &rl);
		if (-1 == ret)
		{
			return -2;
		}
		else
		{
			getrlimit(RLIMIT_NOFILE, &rl);
		}
		return 0;
#else
	int32 SetFileHandleLimit(const uint32 /*dwSetLimit*/)
	{
		return 0;
#endif
	}    

    std::string GetIpStr( uint32 ip )
    {
        uint32 ip1 = ip/0x1000000;
        ip -= ip1 * 0x1000000;
        uint32 ip2 = ip/0x10000;
        ip -= ip2 * 0x10000;
        uint32 ip3 = ip/0x100;
        ip -= ip3 * 0x100;
        uint32 ip4 = ip;

        char ipStr[100];
        SDSnprintf(ipStr, 100, "%d.%d.%d.%d", ip4, ip3, ip2, ip1);
        return ipStr;
    }


}
