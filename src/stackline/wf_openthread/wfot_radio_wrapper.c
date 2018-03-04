#define _WFOT_RADIO_WRAPPER_C_

#include <stdio.h>

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
