#define _WFOT_RADIO_WRAPPER_C_

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include <openthread/config.h>
#include <openthread-core-config.h>
#include <assert.h>

#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/openthread.h>
#include <openthread/platform/platform.h>
#include <openthread/platform/logging.h>

#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>

#include <commline/commline.h>  //Whitefield COMMLINE
#include "wfot_common.h"

extern uint32_t NODE_ID;

#define ENTERING    printf("%s:%d...\r\n", __FUNCTION__, __LINE__)
extern "C" void __wrap_platformRadioInit(void)
{
    extern void __real_platformRadioInit(void);
    ENTERING;
    if(cl_init(CL_ATTACHQ)!=CL_SUCCESS) {
        ERROR("commline init failed, exiting...\n");
        exit(1);
    }
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
        snprintf(fname, sizeof(fname), "openthread_%04x.pcap", NODE_ID);
        handle = pcap_init(fname);
    }
    pcap_write(handle, buf, buflen);
}

void prn_buffer(const char *str, uint8_t *buf, int buflen)
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
    dump_pcap(buf, buflen);
}

void commline_sendto(const uint8_t *buf, int buflen)
{
    DEFINE_MBUF(mbuf);
    mbuf->len = buflen;
    memcpy(mbuf->buf, buf, buflen);
    mbuf->src_id = 0xffff;
    mbuf->dst_id = 0xffff;
    if(CL_SUCCESS != cl_sendto_q(MTYPE(AIRLINE, CL_MGR_ID), mbuf, mbuf->len + sizeof(msg_buf_t))) {
        ERROR("cl_sendto_q failed. utter failure!\r\n");
    }
}

extern "C" otError __wrap_otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    //extern otError __real_otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame);
    //prn_buffer("Frame", aFrame->mPsdu, aFrame->mLength);
    commline_sendto(aFrame->mPsdu, aFrame->mLength);
    return OT_ERROR_NONE;//__real_otPlatRadioTransmit(aInstance, aFrame);
}

extern "C" void __wrap_otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    extern void __real_otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64);
    ENTERING;
    return __real_otPlatRadioGetIeeeEui64(aInstance, aIeeeEui64);
}

extern "C" void __wrap_otPlatRadioSetExtendedAddress(otInstance *aInstance,
                    const otExtAddress *aExtAddress)
{
    extern void __real_otPlatRadioSetExtendedAddress(otInstance *aInstance,
                    const otExtAddress *aExtAddress);
    ENTERING;
    return __real_otPlatRadioSetExtendedAddress(aInstance, aExtAddress);
}

extern "C" void __wrap_otPlatRadioSetShortAddress(otInstance *aInstance,
                    uint16_t aShortAddress)
{
    extern void __real_otPlatRadioSetShortAddress(otInstance *aInstance,
                    uint16_t aShortAddress);
    ENTERING;
    return __real_otPlatRadioSetShortAddress(aInstance, aShortAddress);
}

extern "C" otError __wrap_otPlatRadioEnable(otInstance *aInstance)
{
    extern otError __real_otPlatRadioEnable(otInstance *aInstance);
    ENTERING;
    return __real_otPlatRadioEnable(aInstance);
}

extern "C" otError __wrap_otPlatRadioDisable(otInstance *aInstance)
{
    extern otError __real_otPlatRadioDisable(otInstance *aInstance);
    ENTERING;
    return __real_otPlatRadioDisable(aInstance);
}

extern "C" bool __wrap_otPlatRadioIsEnabled(otInstance *aInstance)
{
    extern bool __real_otPlatRadioIsEnabled(otInstance *aInstance);
    ENTERING;
    return __real_otPlatRadioIsEnabled(aInstance);
}

extern "C" otError __wrap_otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    extern otError __real_otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel);
    ENTERING;
    return __real_otPlatRadioReceive(aInstance, aChannel);
}

extern "C" void __wrap_otPlatRadioReceiveDone(otInstance *aInstance,
                otRadioFrame *aFrame, otError aError)
{
    extern void __real_otPlatRadioReceiveDone(otInstance *aInstance,
                    otRadioFrame *aFrame, otError aError);
    ENTERING;
    return __real_otPlatRadioReceiveDone(aInstance, aFrame, aError);
}

extern "C" void __wrap_otPlatRadioTxStarted(otInstance *aInstance, otRadioFrame *aFrame)
{
    extern void __real_otPlatRadioTxStarted(otInstance *aInstance, otRadioFrame *aFrame);
    ENTERING;
    return __real_otPlatRadioTxStarted(aInstance, aFrame);
}

extern "C" void __wrap_otPlatRadioTxDone(otInstance *aInstance, 
                otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError)
{
    extern void __real_otPlatRadioTxDone(otInstance *aInstance,     
                otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError);
    ENTERING;
    return __real_otPlatRadioTxDone(aInstance, aFrame, aAckFrame, aError);
}

extern "C" otRadioCaps __wrap_otPlatRadioGetCaps(otInstance *aInstance)
{
    extern otRadioCaps __real_otPlatRadioGetCaps(otInstance *aInstance);
    ENTERING;
    return __real_otPlatRadioGetCaps(aInstance);
}

extern int uds_send(char *buf, int buflen);
extern "C" otError __wrap_otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    extern otError __real_otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength);
    uds_send((char*)aBuf, aBufLength);
    return __real_otPlatUartSend(aBuf, aBufLength);
}
