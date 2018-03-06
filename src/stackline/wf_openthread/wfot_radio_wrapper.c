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

extern uint32_t NODE_ID;

#define ENTERING    printf("%s:%d...\r\n", __FUNCTION__, __LINE__)
extern "C" void __wrap_platformRadioInit(void)
{
    extern void __real_platformRadioInit(void);
    ENTERING;
    return __real_platformRadioInit();
}

extern "C" void __wrap_platformRadioDeinit(void)
{
    extern void __real_platformRadioDeinit(void);
    ENTERING;
    return __real_platformRadioDeinit();
}

#if 0
#define COPY_BUF(DST, DSTLEN, SRC, SRCLEN)  \
    if(DSTLEN > SRCLEN) {\
        printf("DSTLEN:%d exceeded SRCLEN:%d\n", DSTLEN, SRCLEN);\
        return;\
    }\
    memcpy(DST, SRC, DSTLEN);\
    (SRCLEN)-=(DSTLEN);

#define FRAME_TYPE_BEACON   0x0
#define FRAME_TYPE_DATA     0x1
#define FRAME_TYPE_ACK      0x2
#define FRAME_TYPE_CMD      0x3

#define HDR_FRAME_TYPE      0x0007  //bits 0-2 Frame type
#define HDR_SECURITY_EN     0x0008  //bit 3 security enabled?
#define HDR_FRAME_PEND      0x0010  //bit 4 frame pending?
#define HDR_ACK_REQ         0x0020  //bit 5 ack requested?
#define HDR_INTRA_PAN       0x0040  //bit 6 intra pan?
#define HDR_RESV1           0x0380  //bits 7-9 reserved
#define HDR_DST_ADDR_MODE   0x0c00  //bits 10-11 dst address mode
#define HDR_RESV2           0x3000  //bits 12-13 reserved
#define HDR_SRC_ADDR_MODE   0xc000  //bits 14-15 src address mode

void decode_802154_hdr(uint8_t *buf, int buflen)
{
    uint8_t frame_type;
    uint16_t frame_ctrl;

    COPY_BUF(&frame_ctrl, 2, buf, buflen);
    frame_type = frame_ctrl & HDR_FRAME_TYPE;
    printf("frame_ctrl:%x\r\n", frame_ctrl);
    printf("frame_type:%02x\r\n", frame_type);
    switch(frame_type) {
        case FRAME_TYPE_BEACON:
            printf("BEACON:");
            break;
        case FRAME_TYPE_DATA:
            printf("DATA:");
            break;
        case FRAME_TYPE_ACK:
            printf("ACK:");
            break;
        case FRAME_TYPE_CMD:
            printf("CMD:");
            break;
        default:
            printf("ERROR: Unknown frame_type:%d\n", frame_ctrl & HDR_FRAME_TYPE);
            return;
    }
    printf("sec=%d, frm_pending=%d, ack_req=%d, intra_pan=%d, dst_mode=%d, src_mode=%d\r\n", 
        (frame_ctrl&HDR_SECURITY_EN)>>3,
        (frame_ctrl&HDR_FRAME_PEND)>>4,
        (frame_ctrl&HDR_ACK_REQ)>>5,
        (frame_ctrl&HDR_INTRA_PAN)>>6,
        (frame_ctrl&HDR_DST_ADDR_MODE)>>10,
        (frame_ctrl&HDR_SRC_ADDR_MODE)>>14
        );
}
#endif

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
    //decode_802154_hdr(buf, buflen);
    //dump_pcap(buf, buflen);
}

extern "C" otError __wrap_otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    extern otError __real_otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame);
    prn_buffer("Frame", aFrame->mPsdu, aFrame->mLength);
    fprintf(stdin, "\nhelp\n");
    return __real_otPlatRadioTransmit(aInstance, aFrame);
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
