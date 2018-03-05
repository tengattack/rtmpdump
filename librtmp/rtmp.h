#ifndef __RTMP_H__
#define __RTMP_H__
/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *      Copyright (C) 2008-2009 Andrej Stepanchuk
 *      Copyright (C) 2009-2010 Howard Chu
 *
 *  This file is part of librtmp.
 *
 *  librtmp is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 2.1,
 *  or (at your option) any later version.
 *
 *  librtmp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with librtmp see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *  http://www.gnu.org/copyleft/lgpl.html
 */

#if !defined(NO_CRYPTO) && !defined(CRYPTO) && !defined(RTMP_NO_CRYPTO)
#define CRYPTO
#endif

#include <errno.h>
#include <stdint.h>
#include <stddef.h>

#include "amf.h"

#ifdef RTMP_EXPORTS 
#define RTMPAPI __declspec(dllexport)
#else
#define RTMPAPI __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define RTMP_LIB_VERSION	0x020300	/* 2.3 */

#define RTMP_FEATURE_HTTP	0x01
#define RTMP_FEATURE_ENC	0x02
#define RTMP_FEATURE_SSL	0x04
#define RTMP_FEATURE_MFP	0x08	/* not yet supported */
#define RTMP_FEATURE_WRITE	0x10	/* publish, not play */
#define RTMP_FEATURE_HTTP2	0x20	/* server-side rtmpt */

#define RTMP_PROTOCOL_UNDEFINED	-1
#define RTMP_PROTOCOL_RTMP      0
#define RTMP_PROTOCOL_RTMPE     RTMP_FEATURE_ENC
#define RTMP_PROTOCOL_RTMPT     RTMP_FEATURE_HTTP
#define RTMP_PROTOCOL_RTMPS     RTMP_FEATURE_SSL
#define RTMP_PROTOCOL_RTMPTE    (RTMP_FEATURE_HTTP|RTMP_FEATURE_ENC)
#define RTMP_PROTOCOL_RTMPTS    (RTMP_FEATURE_HTTP|RTMP_FEATURE_SSL)
#define RTMP_PROTOCOL_RTMFP     RTMP_FEATURE_MFP

#define RTMP_DEFAULT_CHUNKSIZE	128

/* needs to fit largest number of bytes recv() may return */
#define RTMP_BUFFER_CACHE_SIZE (16*1024)

#define	RTMP_CHANNELS	65600

  extern const char RTMPProtocolStringsLower[][7];
  extern const AVal RTMP_DefaultFlashVer;
  extern int RTMP_ctrlC;

  RTMPAPI uint32_t RTMP_GetTime(void);

/*      RTMP_PACKET_TYPE_...                0x00 */
#define RTMP_PACKET_TYPE_CHUNK_SIZE         0x01
/*      RTMP_PACKET_TYPE_...                0x02 */
#define RTMP_PACKET_TYPE_BYTES_READ_REPORT  0x03
#define RTMP_PACKET_TYPE_CONTROL            0x04
#define RTMP_PACKET_TYPE_SERVER_BW          0x05
#define RTMP_PACKET_TYPE_CLIENT_BW          0x06
/*      RTMP_PACKET_TYPE_...                0x07 */
#define RTMP_PACKET_TYPE_AUDIO              0x08
#define RTMP_PACKET_TYPE_VIDEO              0x09
/*      RTMP_PACKET_TYPE_...                0x0A */
/*      RTMP_PACKET_TYPE_...                0x0B */
/*      RTMP_PACKET_TYPE_...                0x0C */
/*      RTMP_PACKET_TYPE_...                0x0D */
/*      RTMP_PACKET_TYPE_...                0x0E */
#define RTMP_PACKET_TYPE_FLEX_STREAM_SEND   0x0F
#define RTMP_PACKET_TYPE_FLEX_SHARED_OBJECT 0x10
#define RTMP_PACKET_TYPE_FLEX_MESSAGE       0x11
#define RTMP_PACKET_TYPE_INFO               0x12
#define RTMP_PACKET_TYPE_SHARED_OBJECT      0x13
#define RTMP_PACKET_TYPE_INVOKE             0x14
/*      RTMP_PACKET_TYPE_...                0x15 */
#define RTMP_PACKET_TYPE_FLASH_VIDEO        0x16

#define RTMP_MAX_HEADER_SIZE 18

#define RTMP_PACKET_SIZE_LARGE    0
#define RTMP_PACKET_SIZE_MEDIUM   1
#define RTMP_PACKET_SIZE_SMALL    2
#define RTMP_PACKET_SIZE_MINIMUM  3

  typedef struct RTMPChunk
  {
    int c_headerSize;
    int c_chunkSize;
    char *c_chunk;
    char c_header[RTMP_MAX_HEADER_SIZE];
  } RTMPChunk;

  typedef struct RTMPPacket
  {
    uint8_t m_headerType;
    uint8_t m_packetType;
    uint8_t m_hasAbsTimestamp;	/* timestamp absolute or relative? */
    int m_nChannel;
    uint32_t m_nTimeStamp;	/* timestamp */
    int32_t m_nInfoField2;	/* last 4 bytes in a long header */
    uint32_t m_nBodySize;
    uint32_t m_nBytesRead;
    RTMPChunk *m_chunk;
    char *m_body;
  } RTMPPacket;

  typedef struct RTMPSockBuf
  {
    int sb_socket;
    int sb_size;		/* number of unprocessed bytes in buffer */
    char *sb_start;		/* pointer into sb_pBuffer of next byte to process */
    char sb_buf[RTMP_BUFFER_CACHE_SIZE];	/* data read from socket */
    int sb_timedout;
    void *sb_ssl;
  } RTMPSockBuf;

  RTMPAPI void RTMPPacket_Reset(RTMPPacket *p);
  RTMPAPI void RTMPPacket_Dump(RTMPPacket *p);
  RTMPAPI int RTMPPacket_Alloc(RTMPPacket *p, uint32_t nSize);
  RTMPAPI void RTMPPacket_Free(RTMPPacket *p);

#define RTMPPacket_IsReady(a)	((a)->m_nBytesRead == (a)->m_nBodySize)

  typedef struct RTMP_LNK
  {
    AVal hostname;
    AVal sockshost;

    AVal playpath0;	/* parsed from URL */
    AVal playpath;	/* passed in explicitly */
    AVal tcUrl;
    AVal swfUrl;
    AVal pageUrl;
    AVal app;
    AVal auth;
    AVal flashVer;
    AVal subscribepath;
    AVal usherToken;
    AVal token;
    AVal pubUser;
    AVal pubPasswd;
    AMFObject extras;
    int edepth;

    int seekTime;
    int stopTime;

#define RTMP_LF_AUTH	0x0001	/* using auth param */
#define RTMP_LF_LIVE	0x0002	/* stream is live */
#define RTMP_LF_SWFV	0x0004	/* do SWF verification */
#define RTMP_LF_PLST	0x0008	/* send playlist before play */
#define RTMP_LF_BUFX	0x0010	/* toggle stream on BufferEmpty msg */
#define RTMP_LF_FTCU	0x0020	/* free tcUrl on close */
#define RTMP_LF_FAPU	0x0040	/* free app on close */
    int lFlags;

    int swfAge;

    int protocol;
    int timeout;		/* connection timeout in seconds */

    int pFlags;			/* unused, but kept to avoid breaking ABI */

    unsigned short socksport;
    unsigned short port;

#ifdef CRYPTO
#define RTMP_SWF_HASHLEN	32
    void *dh;			/* for encryption */
    void *rc4keyIn;
    void *rc4keyOut;

    uint32_t SWFSize;
    uint8_t SWFHash[RTMP_SWF_HASHLEN];
    char SWFVerificationResponse[RTMP_SWF_HASHLEN+10];
#endif
  } RTMP_LNK;

  /* state for read() wrapper */
  typedef struct RTMP_READ
  {
    char *buf;
    char *bufpos;
    unsigned int buflen;
    uint32_t timestamp;
    uint8_t dataType;
    uint8_t flags;
#define RTMP_READ_HEADER	0x01
#define RTMP_READ_RESUME	0x02
#define RTMP_READ_NO_IGNORE	0x04
#define RTMP_READ_GOTKF		0x08
#define RTMP_READ_GOTFLVK	0x10
#define RTMP_READ_SEEKING	0x20
    int8_t status;
#define RTMP_READ_COMPLETE	-3
#define RTMP_READ_ERROR	-2
#define RTMP_READ_EOF	-1
#define RTMP_READ_IGNORE	0

    /* if bResume == TRUE */
    uint8_t initialFrameType;
    uint32_t nResumeTS;
    char *metaHeader;
    char *initialFrame;
    uint32_t nMetaHeaderSize;
    uint32_t nInitialFrameSize;
    uint32_t nIgnoredFrameCounter;
    uint32_t nIgnoredFlvFrameCounter;
  } RTMP_READ;

  typedef struct RTMP_METHOD
  {
    AVal name;
    int num;
  } RTMP_METHOD;

  typedef struct RTMP
  {
    int m_inChunkSize;
    int m_outChunkSize;
    int m_nBWCheckCounter;
    int m_nBytesIn;
    int m_nBytesInSent;
    int m_nBufferMS;
    int m_stream_id;		/* returned in _result from createStream */
    int m_mediaChannel;
    uint32_t m_mediaStamp;
    uint32_t m_pauseStamp;
    int m_pausing;
    int m_nServerBW;
    int m_nClientBW;
    uint8_t m_nClientBW2;
    uint8_t m_bPlaying;
    uint8_t m_bClosing;
    uint8_t m_bSendEncoding;
    uint8_t m_bSendCounter;

    int m_numInvokes;
    int m_numCalls;
    RTMP_METHOD *m_methodCalls;	/* remote method calls queue */

    int m_channelsAllocatedIn;
    int m_channelsAllocatedOut;
    RTMPPacket **m_vecChannelsIn;
    RTMPPacket **m_vecChannelsOut;
    int *m_channelTimestamp;	/* abs timestamp of last packet */

    double m_fAudioCodecs;	/* audioCodecs for the connect packet */
    double m_fVideoCodecs;	/* videoCodecs for the connect packet */
    double m_fEncoding;		/* AMF0 or AMF3 */

    double m_fDuration;		/* duration of stream in seconds */

    int m_msgCounter;		/* RTMPT stuff */
    int m_polling;
    int m_resplen;
    int m_unackd;
    AVal m_clientID;

    RTMP_READ m_read;
    RTMPPacket m_write;
    RTMPSockBuf m_sb;
    RTMP_LNK Link;
  } RTMP;

  RTMPAPI int RTMP_ParseURL(const char *url, int *protocol, AVal *host,
		     unsigned int *port, AVal *playpath, AVal *app);

  RTMPAPI void RTMP_ParsePlaypath(AVal *in, AVal *out);
  RTMPAPI void RTMP_SetBufferMS(RTMP *r, int size);
  RTMPAPI void RTMP_UpdateBufferMS(RTMP *r);

  RTMPAPI int RTMP_SetOpt(RTMP *r, const AVal *opt, AVal *arg);
  RTMPAPI int RTMP_SetupURL(RTMP *r, char *url);
  RTMPAPI void RTMP_SetupStream(RTMP *r, int protocol,
			AVal *hostname,
			unsigned int port,
			AVal *sockshost,
			AVal *playpath,
			AVal *tcUrl,
			AVal *swfUrl,
			AVal *pageUrl,
			AVal *app,
			AVal *auth,
			AVal *swfSHA256Hash,
			uint32_t swfSize,
			AVal *flashVer,
			AVal *subscribepath,
			AVal *usherToken,
			int dStart,
			int dStop, int bLiveStream, long int timeout);

  RTMPAPI int RTMP_Connect(RTMP *r, RTMPPacket *cp);
  struct sockaddr;
  RTMPAPI int RTMP_Connect0(RTMP *r, struct sockaddr *svc);
  RTMPAPI int RTMP_Connect1(RTMP *r, RTMPPacket *cp);
  RTMPAPI int RTMP_Serve(RTMP *r);
  RTMPAPI int RTMP_TLS_Accept(RTMP *r, void *ctx);

  RTMPAPI int RTMP_ReadPacket(RTMP *r, RTMPPacket *packet);
  RTMPAPI int RTMP_SendPacket(RTMP *r, RTMPPacket *packet, int queue);
  RTMPAPI int RTMP_SendChunk(RTMP *r, RTMPChunk *chunk);
  RTMPAPI int RTMP_IsConnected(RTMP *r);
  RTMPAPI int RTMP_Socket(RTMP *r);
  RTMPAPI int RTMP_IsTimedout(RTMP *r);
  RTMPAPI double RTMP_GetDuration(RTMP *r);
  RTMPAPI int RTMP_ToggleStream(RTMP *r);

  RTMPAPI int RTMP_ConnectStream(RTMP *r, int seekTime);
  RTMPAPI int RTMP_ReconnectStream(RTMP *r, int seekTime);
  RTMPAPI void RTMP_DeleteStream(RTMP *r);
  RTMPAPI int RTMP_GetNextMediaPacket(RTMP *r, RTMPPacket *packet);
  RTMPAPI int RTMP_ClientPacket(RTMP *r, RTMPPacket *packet);

  RTMPAPI void RTMP_Init(RTMP *r);
  RTMPAPI void RTMP_Close(RTMP *r);
  RTMPAPI RTMP *RTMP_Alloc(void);
  RTMPAPI void RTMP_Free(RTMP *r);
  RTMPAPI void RTMP_EnableWrite(RTMP *r);

  RTMPAPI void *RTMP_TLS_AllocServerContext(const char* cert, const char* key);
  RTMPAPI void RTMP_TLS_FreeServerContext(void *ctx);

  RTMPAPI int RTMP_LibVersion(void);
  RTMPAPI void RTMP_UserInterrupt(void);	/* user typed Ctrl-C */

  RTMPAPI int RTMP_SendCtrl(RTMP *r, short nType, unsigned int nObject,
		     unsigned int nTime);

  /* caller probably doesn't know current timestamp, should
   * just use RTMP_Pause instead
   */
  RTMPAPI int RTMP_SendPause(RTMP *r, int DoPause, int dTime);
  RTMPAPI int RTMP_Pause(RTMP *r, int DoPause);

  RTMPAPI int RTMP_FindFirstMatchingProperty(AMFObject *obj, const AVal *name,
				      AMFObjectProperty * p);

  RTMPAPI int RTMPSockBuf_Fill(RTMPSockBuf *sb);
  RTMPAPI int RTMPSockBuf_Send(RTMPSockBuf *sb, const char *buf, int len);
  RTMPAPI int RTMPSockBuf_Close(RTMPSockBuf *sb);

  RTMPAPI int RTMP_SendCreateStream(RTMP *r);
  RTMPAPI int RTMP_SendSeek(RTMP *r, int dTime);
  RTMPAPI int RTMP_SendServerBW(RTMP *r);
  RTMPAPI int RTMP_SendClientBW(RTMP *r);
  RTMPAPI void RTMP_DropRequest(RTMP *r, int i, int freeit);
  RTMPAPI int RTMP_Read(RTMP *r, char *buf, int size);
  RTMPAPI int RTMP_Write(RTMP *r, const char *buf, int size);

/* hashswf.c */
  RTMPAPI int RTMP_HashSWF(const char *url, unsigned int *size, unsigned char *hash,
		   int age);

#ifdef __cplusplus
};
#endif

#endif
