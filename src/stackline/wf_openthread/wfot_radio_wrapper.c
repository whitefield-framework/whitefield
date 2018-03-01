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

extern "C" void __wrap_platformRadioInit(void)
{
    extern void __real_platformRadioInit(void);
    printf("Hell yah!\n");
    __real_platformRadioInit();
}

#if 0
extern "C" void __wrap_radioTransmit(struct RadioMessage *aMessage, const struct otRadioFrame *aFrame)
{
    extern void __real_radioTransmit(struct RadioMessage *aMessage, const struct otRadioFrame *aFrame);
    printf("woowu\n");
    __real_radioTransmit(aMessage, aFrame);
}
#endif

extern "C" otError __wrap_otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    extern otError __real_otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame);
    printf("wowwwowwu\n");
    return __real_otPlatRadioTransmit(aInstance, aFrame);
}
