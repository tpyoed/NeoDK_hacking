/*
 * controller.c
 *
 *  NOTICE (do not remove):
 *      This file is part of project NeoDK (https://github.com/Onwrikbaar/NeoDK).
 *      See https://github.com/Onwrikbaar/NeoDK/blob/main/LICENSE.txt for full license details.
 *
 *  Created on: 21 Aug 2024
 *      Author: mark
 *   Copyright  2024 Neostim™
 */

#include <stdlib.h>

#include "bsp_dbg.h"
#include "bsp_app.h"
#include "app_event.h"
#include "sequencer.h"
#include "debug_cli.h"

// This module implements:
#include "controller.h"

typedef void *(*StateFunc)(Controller *, AOEvent const *);

struct _Controller {
    EventQueue event_queue;                     // This MUST be the first member.
    uint8_t event_storage[400];
    StateFunc state;
    DataLink *datalink;
    Sequencer *sequencer;
};


static void handleHostMessage(Controller *me, uint8_t const *msg, uint16_t nb)
{
    BSP_logf("%s, len=%hu\n", __func__, nb);
}


static void *stateNop(Controller *me, AOEvent const *evt)
{
    BSP_logf("Controller_%s unexpected event: %u\n", __func__, AOEvent_type(evt));
    return NULL;
}


static void *stateIdle(Controller *me, AOEvent const *evt)
{
    switch (AOEvent_type(evt))
    {
        case ET_AO_ENTRY:
            BSP_logf("Controller_%s ENTRY\n", __func__);
            break;
        case ET_AO_EXIT:
            BSP_logf("Controller_%s EXIT\n", __func__);
            break;
        default:
            BSP_logf("Controller_%s unexpected event: %u\n", __func__, AOEvent_type(evt));
    }
    return NULL;
}

// Send one event to the state machine.
static void dispatchEvent(Controller *me, AOEvent const *evt)
{
    // BSP_logf("%s(%u)\n", __func__, AOEvent_type(evt));
    StateFunc new_state = me->state(me, evt);
    if (new_state != NULL) {                    // Transition.
        StateFunc sf = me->state(me, AOEvent_newExitEvent());
        // No transition allowed on ENTRY and EXIT events.
        M_ASSERT(sf == NULL);
        me->state = new_state;
        sf = me->state(me, AOEvent_newEntryEvent());
        M_ASSERT(sf == NULL);
    }
}

/*
 * Below are the functions implementing this module's interface.
 */

Controller *Controller_new()
{
    Controller *me = (Controller *)malloc(sizeof(Controller));
    EventQueue_init(&me->event_queue, me->event_storage, sizeof me->event_storage);
    me->state = &stateNop;
    return me;
}


void Controller_init(Controller *me, DataLink *datalink)
{
    me->datalink = datalink;
    BSP_logf("%s\n", __func__);
    me->state = &stateIdle;
    me->state(me, AOEvent_newEntryEvent());
    DataLink_open(me->datalink, me, (PacketCallback)&handleHostMessage);
    DataLink_waitForSync(me->datalink);
}


void Controller_start(Controller *me)
{
    BSP_logf("Starting NeoDK!\n");
    BSP_logf("Push the button to play or pause :-)\n");
    BSP_setPrimaryVoltage_mV(2500);
    BSP_primaryVoltageEnable(true);
}


bool Controller_handleEvent(Controller *me)
{
    return EventQueue_handleNextEvent(&me->event_queue, (EvtFunc)&dispatchEvent, me);
}


void Controller_stop(Controller *me)
{
    me->state(me, AOEvent_newExitEvent());
    me->state = stateNop;
    CLI_logf("End of session\n");
    DataLink_close(me->datalink);
}


void Controller_delete(Controller *me)
{
    free(me);
}
