#ifndef __HARDLINK_H_
#define __HARDLINK_H_
#include <stdbool.h>
#include <stdint.h>
#include <ti/drivers/rf/RF.h>
#include "smartrf_settings/smartrf_settings.h"
#include <stdlib.h>

#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_sl_longrange.h)
#include DeviceFamily_constructPath(rf_patches/rf_patch_rfe_sl_longrange.h)
#include DeviceFamily_constructPath(rf_patches/rf_patch_mce_sl_longrange.h)
//! \brief HardLink Status and error codes
typedef enum
{
    HardLink_Status_Success         = 0, //!< Success
    HardLink_Status_Config_Error    = 1, //!< Configuration error
    HardLink_Status_Param_Error     = 2, //!< Param error
    HardLink_Status_Mem_Error       = 3, //!< Memory Error
    HardLink_Status_Cmd_Error       = 4, //!< Memory Error
    HardLink_Status_Tx_Error        = 5, //!< Tx Error
    HardLink_Status_Rx_Error        = 6, //!< Rx Error
    HardLink_Status_Rx_Timeout      = 7, //!< Rx Error
    HardLink_Status_Rx_Buffer_Error = 8, //!< Rx Buffer Error
    HardLink_Status_Busy_Error      = 9, //!< Busy Error
    HardLink_Status_Aborted         = 10 //!< Command stopped or aborted
} HardLink_Status;

typedef struct HardLink_packet{
    uint8_t *payload;
    size_t size;
}* HardLink_packet_t;

typedef void (* HardLink_tx_cb)(HardLink_Status);

extern int HardLink_init();
extern int HardLink_send(HardLink_packet_t packet);
extern int HardLink_sendAsync(HardLink_packet_t packet, );
extern uint32_t HardLink_getFrequency(void);
extern HardLink_Status HardLink_setFrequency(uint32_t ui32Frequency);
extern HardLink_Status HardLink_getRfPower(int8_t *pi8TxPowerdBm);
extern HardLink_Status HardLink_setRfPower(int8_t i8TxPowerdBm);

#endif
