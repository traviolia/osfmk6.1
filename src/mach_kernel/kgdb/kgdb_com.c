/*
 * Copyright (c) 1995, 1994, 1993, 1992, 1991, 1990  
 * Open Software Foundation, Inc. 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of ("OSF") or Open Software 
 * Foundation not be used in advertising or publicity pertaining to 
 * distribution of the software without specific, written prior permission. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL OSF BE LIABLE FOR ANY 
 * SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING 
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE 
 * 
 */
/*
 * OSF Research Institute MK6.1 (unencumbered) 1/31/1995
 */

#include <kern/misc_protos.h>
#include <kgdb/kgdb_defs.h>

char kgdb_pkt[PBUFSIZ];		/* In/Out packet buffer */
char kgdb_data[PBUFSIZ/2];	/* In/Out data area */

/*
 * Forward prototypes
 */
void kgdb_decode(
	char	*src,
	char	*dst,
	int	*len,
	char	**next);

int _kgdb_fromhex(
	int	val);

/*
 * kgdb_putpkt - format and send a command packet to remote gdb.
 *
 */
void
kgdb_putpkt(
	kgdb_cmd_pkt_t	*reply)
{
	int		i;
	unsigned char	checksum = 0;
	char		*pkt = kgdb_pkt;
	int		ch;
	char		*buf;	
	int		len;
	
	KGDB_DEBUG(("kgdb_putpkt: cmd 0x%x data addr 0x%x count 0x%x\n", 
		     reply->k_cmd, reply->k_data, reply->k_size));

	/*
	 * Check the size
	 */
	if ((reply->k_size * 2 + 1) > (PBUFSIZ - 5)) {
		kgdb_panic("kgdb_putpkt: packet too large");
		/* NOTREACHED */
	}

	/*
	 * Encapsulate the data into the packet buffer with checksum
	 */
	*pkt++ = STX;
	if (reply->k_cmd != 0) {
		*pkt++ = reply->k_cmd;
		checksum += reply->k_cmd;
	}

	if (reply->k_size) {
		buf = (char *) reply->k_data;
		for (i = 0; i < reply->k_size; i++) {
			char	c;
	
			c = kgdb_tohex((buf[i] >> 4) & 0xF);
			*pkt++ = c;
			checksum += c;
			c = kgdb_tohex(buf[i] & 0xF);
			*pkt++ = c;
			checksum += c;
		}
	}

	*pkt++ = ETX;
	*pkt++ = kgdb_tohex((checksum >> 4) & 0xF);
	*pkt++ = kgdb_tohex(checksum & 0xF);
	*pkt = '\0';

	/*
	 * Send the packet and wait for an acknowledgement
	 */
	len = pkt - kgdb_pkt;
	do {
		KGDB_DEBUG(("kgdb_putpkt:  send packet "));
		for (i = 0; i < len; i++) {
			KGDB_DEBUG(("%c", kgdb_pkt[i]));
		}
		KGDB_DEBUG(("\n"));
		for (i = 0; i < len; i++) {
		    	cnputc(kgdb_pkt[i]);
		}
		if ((ch = cngetc()) != ACK)
			delay(1000000);
	} while (ch != ACK);
}

/*
 * kgdb_getpkt - receive and parse a remote gdb command packet
 */
void
kgdb_getpkt(
	kgdb_cmd_pkt_t	*req)
{
	int		c;
	char		*pkt;
	unsigned char	checksum;
	unsigned char	csum1;
	unsigned char	csum2;

	while(TRUE) {

		/*
		 * Wait for start of packet character
		 */
		c = 0;
	    	while (c != STX) {
			c = cngetc();
	    	}

		/*
		 * Start of packet detected
		 */
		checksum = 0;
		pkt = kgdb_pkt;

		while (TRUE) {
			c = cngetc();
			if (c == ETX) {
				break;
			}
			if (c == STX) {
				kgdb_panic("kgdb_getpkt:  protocol violation");
				/* NOTREACHED */
			}
			if (pkt >= (kgdb_pkt + PBUFSIZ - 1)) {
				kgdb_panic("kgdb_getpkt:  protocol violation");
				/* NOTREACHED */
			}
			*pkt++ = c;
			checksum += c;
		}

		/*
		 * End of packet detected, terminate string and
		 * verify the checksum
		 */
		*pkt = 0;
		csum1 = kgdb_fromhex(cngetc());
		csum2 = kgdb_fromhex(cngetc());
		if ((checksum & 0xFF) == ((csum1 << 4) + csum2)) {
			break;
		}

		/*
		 * Bad checksum, acknowledge and try again.
		 */
		KGDB_DEBUG(("kgdb_getpkt:  bad checksum got 0x%x wanted 0x%x\n",
			   (csum1 << 4) + csum2, checksum &0xFF));	
		KGDB_DEBUG(("%s\n", kgdb_pkt));
		cnputc(NAK);
	}

	/*
	 * Got a good packet, send an acknowledgement
	 */
	KGDB_DEBUG(("kgdb_getpkt:  sending ACK\n"));
	KGDB_DEBUG(("%s\n", kgdb_pkt));
	cnputc(ACK);

	/*
	 * Parse the packet
	 */
	pkt = kgdb_pkt;
	req->k_cmd = *pkt++;
	req->k_data = (vm_offset_t) kgdb_data;
	switch (req->k_cmd) {
	    case '?':
	    case 'g':
	    case 'v':
		break;
	    case 'G':
		kgdb_decode(pkt, (char *) req->k_data, &req->k_size, &pkt);
		if (req->k_size != REGISTER_BYTES) {
		    KGDB_DEBUG(("kgdb_getpkt: reg count got 0x%s wanted 0x%x\n",
			        req->k_size, REGISTER_BYTES));
		    kgdb_panic("kgdb_getpkt: reg count");
		    /* NOTREACHED */
		}
		break;
	    case 'm':
		kgdb_decode(pkt, (char *) &req->k_addr, (int *) 0, &pkt);
		kgdb_decode(++pkt, (char *) &req->k_size, (int *) 0, &pkt);
		break;
	    case 'M':
		kgdb_decode(pkt, (char *) &req->k_addr, (int *) 0, &pkt);
		kgdb_decode(++pkt, (char *) &req->k_size, (int *) 0, &pkt);
		kgdb_decode(++pkt, (char *) req->k_data, &req->k_size, &pkt);
		break;
	    case 'c':
		if (*pkt != 0)
		    kgdb_decode(pkt, (char *) &req->k_addr, (int *) 0, &pkt);
		else	
		    req->k_addr = KGDB_CMD_NO_ADDR;
		break;
	    case 's':
		if (*pkt != 0)
		    kgdb_decode(pkt, (char *) &req->k_addr, (int *) 0, &pkt);
		else	
		    req->k_addr = KGDB_CMD_NO_ADDR;
		break;
	    default:
		KGDB_DEBUG(("kgdb_getpkt:  bad cmd 0x%x\n", *pkt));
		kgdb_panic("kgdb_getpkt: bad cmd\n");
		/* NOTREACHED */
		break;
	}
	KGDB_DEBUG(("kgdb_getpkt: cmd %c addr 0x%x size 0x%x data 0x%x *data 0x%x\n", 
	      req->k_cmd, req->k_addr, req->k_size, req->k_data, *((int *) req->k_data)));
}

/*
 * kgdb_decode - convert a stream of ascii hex digits into an array of integers,
 *		 halting when the first non-hex digit is encountered.
 */
void
kgdb_decode(
	char	*src,
	char	*dst,
	int	*len,
	char	**next)
{
	char 		*p = src;
	int		nib1;
	int		nib2;

	while (TRUE) {
		nib1 = _kgdb_fromhex(*p++);
		if (nib1 == -1) {
			--p;
			break;
		}
		nib2 = _kgdb_fromhex(*p++);
		if (nib2 == -1) {	
		    KGDB_DEBUG(("kgdb_decode:  bad nibble 0x%x\n", *--p));
		    kgdb_panic("kgdb_decode: bad nibble\n");
		    /* NOTREACHED */
		}
		*dst++ = (nib1 * 16) + nib2;
	}
	if (len)
		*len = (p - src)/2;
	*next = p;	
}

int
kgdb_fromhex(
	int	val)
{
	int	v;

	if ((v = _kgdb_fromhex(val)) == -1) {
		KGDB_DEBUG(("kgdb_fromhex:  bad digit 0x%x\n", val));
		kgdb_panic("kgdb_fromhex:  invalid hex digit");
		/* NOTREACHED */
		return(-1);	/* Pacify ANSI C */
	}
	else {
		return(v);
	}
}

int
_kgdb_fromhex(
	int	val)
{
	if (val >= '0' && val <= '9') {
		return (val - '0');
	}
	else if (val >= 'a' && val <= 'f') {
		return(val - 'a' + 10);
	}
	return(-1);
}

int
kgdb_tohex(
	int	val)
{
	if (val < 10) {
		return(val + '0');
	}
	else {
		return(val + 'a' - 10);
	}
}



