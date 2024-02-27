/*
 * comms.c
 *
 *  Created on: 24 Feb 2024
 *      Author: mark
 *   Copyright  2024 Neostim
 */

#include <stdlib.h>

#include "bsp_dbg.h"
#include "bsp_mao.h"
#include "bsp_app.h"

// This module implements:
#include "comms.h"


struct _Comms {
    uint8_t outbuf_storage[200];
    int serial_fd;
};


static void rxCallback(Comms *me, uint32_t ch)
{
    if (ch == '\n') {
        BSP_doChannelAction(me->serial_fd, CA_TX_CB_ENABLE);
    } else {
        BSP_logf("%s(0x%02x)\n", __func__, ch);
    }
}


static void rxErrorCallback(Comms *me, uint32_t rx_error)
{
    BSP_logf("%s(%u)\n", __func__, rx_error);
}


static void txCallback(Comms *me, uint8_t *dst)
{
    static char const hello[] = "Hello!", *cp = hello;

    if (*cp != '\0') {
        *dst = *cp++;
    } else {
        BSP_doChannelAction(me->serial_fd, CA_TX_CB_DISABLE);
        *dst = '\n';
        cp = hello;
    }
}


static void txErrorCallback(Comms *me, uint32_t tx_error)
{
    BSP_logf("%s(%u)\n", __func__, tx_error);
}

/*
 * Below are the functions implementing this module's interface.
 */

Comms *Comms_new()
{
    Comms *me = (Comms *)malloc(sizeof(Comms));
    me->serial_fd = -1;
    return me;
}


bool Comms_open(Comms *me)
{
    BSP_initComms();
    if (me->serial_fd >= 0 || (me->serial_fd = BSP_openSerialPort("serial_1")) < 0) return false;

    Selector rx_sel, rx_err_sel, tx_err_sel;
    Selector_init(&rx_sel, (Action)&rxCallback, me);
    Selector_init(&rx_err_sel, (Action)&rxErrorCallback, me);
    Selector_init(&tx_err_sel, (Action)&txErrorCallback, me);
    BSP_registerRxCallback(me->serial_fd, &rx_sel, &rx_err_sel);
    BSP_registerTxCallback(me->serial_fd, (void (*)(void *, uint8_t *))&txCallback, me, &tx_err_sel);
    BSP_doChannelAction(me->serial_fd, CA_OVERRUN_CB_ENABLE);
    BSP_doChannelAction(me->serial_fd, CA_FRAMING_CB_ENABLE);
    BSP_doChannelAction(me->serial_fd, CA_RX_CB_ENABLE);
    BSP_doChannelAction(me->serial_fd, CA_TX_CB_ENABLE);
    return true;
}


void Comms_close(Comms *me)
{
    // TODO Close the underlying channel.
    me->serial_fd = -1;
}


void Comms_delete(Comms *me)
{
    free(me);
}
