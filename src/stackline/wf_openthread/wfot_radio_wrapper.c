/*
 * Copyright (C) 2017 Rahul Jadhav <nyrahul@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU
 * General Public License v2. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     stackline
 * @{
 *
 * @file
 * @brief       Overrided Openthread interfaces by whitefield
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define _WFOT_RADIO_WRAPPER_C_

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include <openthread/config.h>
#include <openthread-core-config.h>
#include <assert.h>
#include <syslog.h>

#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/openthread.h>
#include "platform.h"
#include <openthread/platform/logging.h>

#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>

#include <commline/commline.h>  //Whitefield COMMLINE
#include <commline/cl_stackline_helpers.h>
#include "wfot_common.h"

#define IEEE802154_MIN_LENGTH   5
#define IEEE802154_MAX_LENGTH   127

static otRadioFrame sTransmitFrame;
static otRadioFrame sReceiveFrame;
static otError      sTransmitError;
static otError      sReceiveError;

static uint8_t sReceivePsdu[IEEE802154_MAX_LENGTH];
static uint8_t sTransmitPsdu[IEEE802154_MAX_LENGTH];

extern uint32_t NODE_ID;
static otRadioState sState = OT_RADIO_STATE_DISABLED;

#define ENTERING    printf("RADIO %s:%d...\r\n", __FUNCTION__, __LINE__)
extern "C" void __wrap_platformRadioInit(void)
{
    extern void __real_platformRadioInit(void);
    ENTERING;
    if(cl_init(MTYPE(STACKLINE, NODE_ID-1), CL_ATTACHQ)!=CL_SUCCESS) {
        ERROR("commline init failed, exiting...\n");
        exit(1);
    }
    sTransmitFrame.mLength = 0;
    sTransmitFrame.mPsdu   = sTransmitPsdu;
    sReceiveFrame.mLength  = 0;
    sReceiveFrame.mPsdu    = sReceivePsdu;

    openlog("wfot", LOG_PID, LOG_USER);
    setlogmask(LOG_UPTO(LOG_DEBUG));

    INFO("Whitefield commline init success\r\n");
    return __real_platformRadioInit();
}

extern "C" void __wrap_platformRadioDeinit(void)
{
    extern void __real_platformRadioDeinit(void);
    ENTERING;
    return __real_platformRadioDeinit();
}

void dump_pcap(const uint8_t *buf, int buflen)
{
    extern void *pcap_init(const char *fname);
    extern void pcap_write(void *handle, const uint8_t *buf, int buflen);
    static void *handle=NULL;

    if(!handle) {
        char fname[512];
        snprintf(fname, sizeof(fname), "openthread_%04x.pcap", NODE_ID-1);
        handle = pcap_init(fname);
    }
    pcap_write(handle, buf, buflen);
}

void prn_buffer(const char *str, const uint8_t *buf, const int buflen)
{
    int i;
    if(str) {
        printf("Len:%d %s:\r\n", buflen, str);
    }
    for(i=1;i<=buflen;i++) {
        printf("%02x ", buf[i-1]);
        if(!(i%16)) {
            printf("\r\n");
            continue;
        } else if(!(i%8)) {
            printf("\t");
            continue;
        }
    }
    printf("\r\n");
}

int commline_sendto(const uint8_t *buf, int buflen, int flags)
{
    DEFINE_MBUF(mbuf);
    memcpy(mbuf->buf, buf, buflen);
    mbuf->len    = buflen;
    mbuf->src_id = NODE_ID-1;   //-1 since OT NODE_ID = WF NODEID-1
    mbuf->flags  = flags;
    mbuf->dst_id = CL_DSTID_MACHDR_PRESENT;   //Openthread already prepares its 802.15.4 machdr; For cmds this field is NA
    INFO("sending packet len=%d, src_id=%d flags=%d\n", buflen, mbuf->src_id, flags);
    return cl_sendto_q(MTYPE(AIRLINE, CL_MGR_ID), mbuf, mbuf->len + sizeof(msg_buf_t));
}

void setChannel(uint8_t aChannel)
{
}

extern "C" otError __wrap_otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    otError error = OT_ERROR_INVALID_STATE;
    (void)aInstance;
        
    if (sState == OT_RADIO_STATE_RECEIVE)
    {       
        error          = OT_ERROR_NONE;
        sState         = OT_RADIO_STATE_TRANSMIT;
        sTransmitError = OT_ERROR_NONE;

        setChannel(aFrame->mChannel);
        otPlatRadioTxStarted(aInstance, aFrame);
        if(CL_SUCCESS != commline_sendto(aFrame->mPsdu, aFrame->mLength, 0)) {
            ERROR("commline_sendto FAILED\n");
            sTransmitError = OT_ERROR_CHANNEL_ACCESS_FAILURE;
        }
    } 
    else 
    {
        ERROR("INVALID sstate:%d, DATA WONT BE TXed. Len:%d\n", sState, aFrame->mLength);
        prn_buffer("FAILEDTX frame!!", aFrame->mPsdu, aFrame->mLength);
    }

    return error;
}

extern "C" void __wrap_otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    uint8_t buf[OT_EXT_ADDRESS_SIZE];
    int i;

    ENTERING;
    cl_get_id2longaddr(NODE_ID, buf, OT_EXT_ADDRESS_SIZE);
    for(i=0; i<OT_EXT_ADDRESS_SIZE; i++) 
    {
        aIeeeEui64[i] = buf[7-i];
    }
    prn_buffer("GetIeeeEui64", aIeeeEui64, 8);
    return;
}

extern "C" otRadioFrame *__wrap_otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    //extern otRadioFrame *__real_otPlatRadioGetTransmitBuffer(otInstance *aInstance);
    (void)aInstance;
    return &sTransmitFrame;
}

extern "C" void __wrap_otPlatRadioSetExtendedAddress(otInstance *aInstance,
                    const otExtAddress *aExtAddress)
{
    char buf[256];
    int buflen=0;
//    extern void __real_otPlatRadioSetExtendedAddress(otInstance *aInstance,
 //                   const otExtAddress *aExtAddress);
    ENTERING;
    buflen += snprintf(buf, sizeof(buf), "cmd_802154_set_ext_addr");
    for(int i=0;i<OT_EXT_ADDRESS_SIZE;i++) {
        buflen += snprintf(buf+buflen, sizeof(buf)-buflen, ":%02x", 
            aExtAddress->m8[7-i]);
    }
    INFO("setting extended address to [%s]\n", buf);
    commline_sendto((uint8_t *)buf, buflen, MBUF_IS_CMD | MBUF_DO_NOT_RESPOND);
    //return __real_otPlatRadioSetExtendedAddress(aInstance, aExtAddress);
    return;
}

extern "C" void __wrap_otPlatRadioSetPanId(otInstance *aInstance,
                    uint16_t panID)
{
    char buf[64];
    int buflen;
//    extern void __real_otPlatRadioSetPanId(otInstance *aInstance,
 //                   uint16_t panID);
    ENTERING;
    INFO("set pan id:%x\n", panID);
    buflen = snprintf(buf, sizeof(buf), "cmd_802154_set_panid:%d", panID);
    //TODO Send this to the Airline and set the pan ID
    commline_sendto((uint8_t *)buf, buflen, MBUF_IS_CMD | MBUF_DO_NOT_RESPOND);
    //return __real_otPlatRadioSetPanId(aInstance, panID);
    return;
}

extern "C" void __wrap_otPlatRadioSetShortAddress(otInstance *aInstance,
                    uint16_t aShortAddress)
{
    char strbuf[256];
    int len;

    len = snprintf(strbuf, sizeof(strbuf), "%s:%d", "cmd_802154_set_short_addr", aShortAddress);
//    extern void __real_otPlatRadioSetShortAddress(otInstance *aInstance,
 //                   uint16_t aShortAddress);
    INFO("Set Short Addr: %04x\n", aShortAddress);
    commline_sendto((uint8_t *)strbuf, len, MBUF_IS_CMD | MBUF_DO_NOT_RESPOND);
    //return __real_otPlatRadioSetShortAddress(aInstance, aShortAddress);
    return;
}

extern "C" otError __wrap_otPlatRadioEnable(otInstance *aInstance)
{
    ENTERING;
    if (!otPlatRadioIsEnabled(aInstance))
    {   
        //otLogDebgPlat(aInstance, "State=OT_RADIO_STATE_SLEEP", NULL);
        sState = OT_RADIO_STATE_SLEEP;
    }
                                            
    return OT_ERROR_NONE;
}

extern "C" otError __wrap_otPlatRadioDisable(otInstance *aInstance)
{
    ENTERING;
    if (otPlatRadioIsEnabled(aInstance))
    {   
        //otLogDebgPlat(aInstance, "State=OT_RADIO_STATE_DISABLED", NULL);
        sState = OT_RADIO_STATE_DISABLED;
    }
    return OT_ERROR_NONE;
}

void enableReceiver(void)
{   
    INFO("enableReceiver IS NOT HANDLED\n");
}

void disableReceiver(void)
{
    INFO("disableReceiver is NOT HANDLED\n");
}

extern "C" otError __wrap_otPlatRadioSleep(otInstance *aInstance)
{       
    otError error = OT_ERROR_INVALID_STATE;

    if (sState == OT_RADIO_STATE_SLEEP || sState == OT_RADIO_STATE_RECEIVE)
    {   
        //otLogDebgPlat(aInstance, "State=OT_RADIO_STATE_SLEEP", NULL);
        error  = OT_ERROR_NONE;
        sState = OT_RADIO_STATE_SLEEP;
        disableReceiver();
    }
        
    return error;
}   

extern "C" bool __wrap_otPlatRadioIsEnabled(otInstance *aInstance)
{
    (void)aInstance;
    ENTERING;
    return (sState != OT_RADIO_STATE_DISABLED) ? true : false;
}

extern "C" otError __wrap_otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    otError error = OT_ERROR_INVALID_STATE;

    if (sState != OT_RADIO_STATE_DISABLED)
    {   
        //otLogDebgPlat(aInstance, "State=OT_RADIO_STATE_RECEIVE", NULL);
    
        error  = OT_ERROR_NONE;
        sState = OT_RADIO_STATE_RECEIVE;
        setChannel(aChannel);
        sReceiveFrame.mChannel = aChannel;
        enableReceiver();
    }

    return error;
}

extern "C" void __wrap_otPlatRadioReceiveDone(otInstance *aInstance,
                otRadioFrame *aFrame, otError aError)
{
    //extern void __real_otPlatRadioReceiveDone(otInstance *aInstance,
    //                otRadioFrame *aFrame, otError aError);
    ENTERING;
    //return __real_otPlatRadioReceiveDone(aInstance, aFrame, aError);
    return;
}

extern "C" void __wrap_otPlatRadioTxStarted(otInstance *aInstance, otRadioFrame *aFrame)
{
    //extern void __real_otPlatRadioTxStarted(otInstance *aInstance, otRadioFrame *aFrame);
    ENTERING;
    //return __real_otPlatRadioTxStarted(aInstance, aFrame);
    return;
}

extern "C" otRadioCaps __wrap_otPlatRadioGetCaps(otInstance *aInstance)
{
    ENTERING;
    return OT_RADIO_CAPS_NONE;
}

extern int uds_send(char *buf, int buflen);
extern "C" otError __wrap_otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    extern otError __real_otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength);
    uds_send((char*)aBuf, aBufLength);
    return __real_otPlatUartSend(aBuf, aBufLength);
}

void handleFrame(otInstance *aInstance, msg_buf_t *mbuf)
{
    if(!IN_RANGE(mbuf->len, IEEE802154_MIN_LENGTH, IEEE802154_MAX_LENGTH)) {
        ERROR("handleFrame: mbuf len:%d not in range!\n", mbuf->len);
        return;
    }
#if OPENTHREAD_ENABLE_RAW_LINK_API
    // Timestamp
    sReceiveFrame.mMsec = otPlatAlarmMilliGetNow();
    sReceiveFrame.mUsec = 0; // Don't support microsecond timer for now.
#endif
    memcpy(sReceiveFrame.mPsdu, mbuf->buf, mbuf->len);
    sReceiveFrame.mLength = mbuf->len;
    sReceiveFrame.mRssi   = mbuf->info.sig.rssi;
    sReceiveFrame.mLqi    = mbuf->info.sig.lqi;
    prn_buffer("RX Frame", sReceiveFrame.mPsdu, sReceiveFrame.mLength);

#if OPENTHREAD_ENABLE_DIAG
    if (otPlatDiagModeGet()) 
    {
        otPlatDiagRadioReceiveDone(aInstance, &sReceiveFrame, sReceiveError);
    } 
    else
#endif
    {
        otPlatRadioReceiveDone(aInstance, &sReceiveFrame, sReceiveError);
    }
}

extern void sl_handle_cmd(msg_buf_t *mbuf);
void wfHandleCommlineEvent(otInstance *aInstance)
{
    DEFINE_MBUF(mbuf);
    int ret;

    ret = cl_recvfrom_q(MTYPE(STACKLINE, NODE_ID-1), mbuf, sizeof(mbuf_buf), CL_FLAG_NOWAIT);
    if(0 == mbuf->len) {
        return;
    }
    if(mbuf->flags & MBUF_IS_CMD) {
        INFO("Handle CMD\n");
        sl_handle_cmd(mbuf);
        return;
    }
    if(mbuf->flags & MBUF_IS_ACK) {
        sState = OT_RADIO_STATE_RECEIVE;
        INFO("Got an ACK from airline\n");
        otPlatRadioTxDone(aInstance, &sTransmitFrame, &sReceiveFrame, sTransmitError);
        return;
    }

    INFO("rcvd pkt len:%d on commline ret:%d\n", mbuf->len, ret);
    handleFrame(aInstance, mbuf);
}

