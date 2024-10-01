/*
 * app_event.h -- the application event types
 *  Note: keep any and all platform dependencies out of this file.
 * 
 *  NOTICE (do not remove):
 *      This file is part of project NeoDK (https://github.com/Onwrikbaar/NeoDK).
 *      See https://github.com/Onwrikbaar/NeoDK/blob/main/LICENSE.txt for full license details.
 *
 *  Created on: 22 May 2021
 *      Author: Mark
 *   Copyright  2021..2024 Neostim™
 */

#ifndef INC_APP_EVENT_H_
#define INC_APP_EVENT_H_

#include "ao_event.h"

// Make sure the ordinals are >= ET_FIRST_APP_EVENT.
enum {
    ET_THIRD_PARTY = ET_AO_FIRST_APP_EVENT,     // To keep other libs working.
    ET_APP_HEARTBEAT, ET_ADC_DATA_AVAILABLE,
    ET_CONTROLLER_CONNECTED, ET_CONTROLLER_DISCONNECTED,
    ET_CONTROLLER_SENT_REQUEST, ET_CONTROLLER_SENT_CHOICE,
    ET_SEQUENCER_READY, ET_SEQUENCER_PLAY_PAUSE, ET_NEXT_ROUTINE,
    ET_BURST_REQUESTED, ET_BURST_REJECTED, ET_BURST_STARTED, ET_BURST_COMPLETED, ET_BURST_EXPIRED,
};

#endif
