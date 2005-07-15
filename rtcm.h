/*
 * Structures for interpreting words in an RTCM-104 message (after
 * parity checking and removing inversion). RTCM104 is an obscure and
 * complicated serial protocol used for broadcasting pseudorange
 * corrections from differential-GPS reference stations. This header
 * is part of the GPSD package: see <http://gpsd.berlios.de> for more.
 *
 * The RTCM words are 30-bit words.  We will lay them into memory into
 * 30-bit (low-end justified) chunks.  To write them out we will write
 * 5 Magnavox-format bytes where the low 6-bits of the byte are 6-bits
 * of the 30-word msg.
 */


typedef /*@unsignedintegraltype@*/ unsigned int RTCMWORD;

#define	ZCOUNT_SCALE	0.6	/* sec */
#define	PCSMALL		0.02	/* metres */
#define	PCLARGE		0.32	/* metres */
#define	RRSMALL		0.002	/* metres/sec */
#define	RRLARGE		0.032	/* metres/sec */

#define XYZ_SCALE	0.01	/* metres */
#define DXYZ_SCALE	0.1	/* metres */
#define	LA_SCALE	90.0/32767.0	/* degrees */
#define	LO_SCALE	180.0/32767.0	/* degrees */
#define	FREQ_SCALE	0.1	/* kHz */
#define	FREQ_OFFSET	190.0	/* kHz */
#define CNR_OFFSET	24	/* dB */
#define TU_SCALE	5	/* minutes */

struct rtcm_msghw1 {			/* header word 1 */
    uint            parity:6;
    uint            refstaid:10;	/* reference station ID */
    uint            msgtype:6;		/* RTCM message type */
    uint            preamble:8;		/* fixed at 01100110 */
    uint            _pad:2;
};

struct rtcm_msghw2 {			/* header word 2 */
    uint            parity:6;
    uint            stathlth:3;		/* station health */
    uint            frmlen:5;
    uint            sqnum:3;
    uint            zcnt:13;
    uint            _pad:2;
};

struct rtcm_msg1w3 {			/* msg 1 word 3 */
    uint            parity:6;
    int             pc1:16;
    uint            satident1:5;	/* satellite ID */
    uint            udre1:2;
    uint            scale1:1;
    uint            _pad:2;
};

struct rtcm_msg1w4 {			/* msg 1 word 4 */
    uint            parity:6;
    uint            satident2:5;	/* satellite ID */
    uint            udre2:2;
    uint            scale2:1;
    uint            issuedata1:8;
    int             rangerate1:8;
    uint            _pad:2;
};

struct rtcm_msg1w5 {			/* msg 1 word 5 */
    uint            parity:6;
    int             rangerate2:8;
    int             pc2:16;
    uint            _pad:2;
};


struct rtcm_msg1w6 {			/* msg 1 word 6 */
    uint            parity:6;
    int             pc3_h:8;
    uint            satident3:5;	/* satellite ID */
    uint            udre3:2;
    uint            scale3:1;
    uint            issuedata2:8;
    uint            _pad:2;
};

struct rtcm_msg1w7 {			/* msg 1 word 7 */
    uint            parity:6;
    uint            issuedata3:8;
    int             rangerate3:8;
    uint            pc3_l:8;		/* NOTE: uint for low byte */
    uint            _pad:2;
};

struct rtcm_msghdr {
    struct rtcm_msghw1   w1;
    struct rtcm_msghw2   w2;
};

struct rtcm_msg1 {
    struct rtcm_msghw1   w1;
    struct rtcm_msghw2   w2;

    struct rtcm_msg1w3   w3;		/* clump #1 of 5-corrections each */
    struct rtcm_msg1w4   w4;
    struct rtcm_msg1w5   w5;
    struct rtcm_msg1w6   w6;
    struct rtcm_msg1w7   w7;

    struct rtcm_msg1w3   w8;		/* clump #2 of 5-corrections each */
    struct rtcm_msg1w4   w9;
    struct rtcm_msg1w5   w10;
    struct rtcm_msg1w6   w11;
    struct rtcm_msg1w7   w12;

    struct rtcm_msg1w3   w13;		/* clump #2 of 5-corrections each */
    struct rtcm_msg1w4   w14;
    struct rtcm_msg1w5   w15;
    struct rtcm_msg1w6   w16;
    struct rtcm_msg1w7   w17;
};

/*  
 * From the RCTM104 standard:
 *
 * "The 30 bit words (as opposed to 32 bit words) coupled with a 50 Hz
 * transmission rate provides a convenient timing capability where the
 * times of word boundaries are a rational multiple of 0.6 seconds."
 *
 * "Each frame is N+2 words long, where N is the number of message data
 * words. For example, a filler message (type 6 or 34) with no message
 * data will have N=0, and will consist only of two header words. The
 * maximum number of data words allowed by the format is 31, so that
 * the longest possible message will have a total of 33 words."
 */
#define RTCM_WORDS_MAX	33

struct rtcm_ctx {
    bool            locked;
    int             curr_offset;
    RTCMWORD        curr_word;
    RTCMWORD        buf[RTCM_WORDS_MAX];
    unsigned int    bufindex;
};

#define RTCM_NO_SYNC	(struct rtcm_msghdr *)0
#define RTCM_SYNC	(struct rtcm_msghdr *)-1

#define RTCM_ERRLEVEL_BASE	5

extern void rtcm_init(/*@out@*/struct rtcm_ctx *);
extern /*@null@*//*@observer@*/ struct rtcm_msghdr *rtcm_decode(struct rtcm_ctx *, unsigned int);
extern void rtcm_dump(struct rtcm_msghdr *, /*@out@*/char[], size_t);

/* end */
