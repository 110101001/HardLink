#include "HardLink.h"
#include <stdio.h>
#define HARD_LINK_CMD_MASK RF_EventLastCmdDone | \
             RF_EventCmdAborted | RF_EventCmdStopped | RF_EventCmdCancelled | \
             RF_EventCmdPreempted

#define ms_To_RadioTime(ms) (ms*(4000000/1000))
#define bytes_per_raw_bit 64
#define RF_TxPowerTable_INVALID_VALUE 0x3fffff

static RF_Object rfObject;
static RF_Handle rfHandle;
static RF_Params rfParams;

//Indicating that the API is initialized
static uint8_t configured = 0;
//Indicating that the API suspended
static uint8_t suspended = 0;
// local commands, contents will be defined by modulation type
static rfc_CMD_FS_t HardLink_cmdFs;


unsigned char prs_0[64] = {
  0b00000001, 0b01011110, 0b11010100, 0b01100001, 0b00001011, 0b11110011, 0b00110001, 0b01011100,
  0b01100110, 0b10010010, 0b01011011, 0b00101010, 0b11100000, 0b10100011, 0b00000000, 0b11100001,
  0b10111011, 0b10011111, 0b00110001, 0b11001111, 0b11110111, 0b11000000, 0b10110010, 0b01110101,
  0b10101010, 0b10100111, 0b10100101, 0b00010010, 0b00001111, 0b01011011, 0b00000010, 0b00111101,
  0b01001110, 0b01100000, 0b10001110, 0b00010111, 0b00110100, 0b10000101, 0b01100001, 0b01000101,
  0b00000110, 0b10100010, 0b00110110, 0b00101111, 0b10101001, 0b00011111, 0b11010111, 0b11111101,
  0b10011101, 0b01001000, 0b00011001, 0b00011000, 0b10101111, 0b00110110, 0b10010011, 0b00000000,
  0b00010000, 0b10000101, 0b00101000, 0b00011101, 0b01011100, 0b10101111, 0b01100100, 0b11011010
};

unsigned char prs_1[64] = {
  0b11111101, 0b00111110, 0b01110111, 0b11010101, 0b00100101, 0b11101111, 0b00101100, 0b01101001,
  0b00101010, 0b11101001, 0b00111100, 0b11000100, 0b00000111, 0b10010011, 0b11000101, 0b00000111,
  0b00110111, 0b00011111, 0b01111011, 0b11010001, 0b10111010, 0b00000111, 0b10010000, 0b00110111,
  0b11011111, 0b01011010, 0b11101101, 0b11001000, 0b10001100, 0b01101001, 0b10010111, 0b00101001,
  0b10101100, 0b11011001, 0b11010110, 0b00011010, 0b11010110, 0b10101000, 0b00000101, 0b11010011,
  0b01101010, 0b11001011, 0b11010110, 0b01010010, 0b00111111, 0b11100111, 0b10000010, 0b10000110,
  0b01101110, 0b10011010, 0b01100101, 0b10100110, 0b00101110, 0b01010100, 0b11110100, 0b01111010,
  0b11001011, 0b00101110, 0b01100011, 0b10111111, 0b01010100, 0b11000100, 0b11010100, 0b01010100
};

const RF_TxPowerTable_Entry PROP_RF_txPowerTable[] =
{
    {-10, RF_TxPowerTable_DEFAULT_PA_ENTRY(0, 3, 0, 4) },
    {0, RF_TxPowerTable_DEFAULT_PA_ENTRY(1, 1, 0, 0) },
    {1, RF_TxPowerTable_DEFAULT_PA_ENTRY(3, 3, 0, 8) },
    {2, RF_TxPowerTable_DEFAULT_PA_ENTRY(2, 1, 0, 8) },
    {3, RF_TxPowerTable_DEFAULT_PA_ENTRY(4, 3, 0, 10) },
    {4, RF_TxPowerTable_DEFAULT_PA_ENTRY(5, 3, 0, 12) },
    {5, RF_TxPowerTable_DEFAULT_PA_ENTRY(6, 3, 0, 12) },
    {6, RF_TxPowerTable_DEFAULT_PA_ENTRY(7, 3, 0, 14) },
    {7, RF_TxPowerTable_DEFAULT_PA_ENTRY(9, 3, 0, 16) },
    {8, RF_TxPowerTable_DEFAULT_PA_ENTRY(11, 3, 0, 18) },
    {9, RF_TxPowerTable_DEFAULT_PA_ENTRY(13, 3, 0, 22) },
    {10, RF_TxPowerTable_DEFAULT_PA_ENTRY(19, 3, 0, 28) },
    {11, RF_TxPowerTable_DEFAULT_PA_ENTRY(26, 3, 0, 40) },
    {12, RF_TxPowerTable_DEFAULT_PA_ENTRY(24, 0, 0, 92) },
    {13, RF_TxPowerTable_DEFAULT_PA_ENTRY(63, 0, 0, 83) }, // The original PA value (12.5 dBm) have been rounded to an integer value.
    {14, RF_TxPowerTable_DEFAULT_PA_ENTRY(63, 0, 1, 83) }, // This setting requires CCFG_FORCE_VDDR_HH = 1.
    RF_TxPowerTable_TERMINATION_ENTRY
};

const uint8_t PROP_RF_txPowerTableSize = sizeof(PROP_RF_txPowerTable)/sizeof(RF_TxPowerTable_Entry);


int HardLink_init(){

    RF_Params_init(&rfParams);
    //rfParams.nInactivityTimeout = ms_To_RadioTime(1);

    rfHandle = RF_open(&rfObject, &RF_prop,
                &RF_cmdPropRadioDivSetup, &rfParams);

    /*RF_runCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, 0,
              HARD_LINK_CMD_MASK);*/

    return 0;
}

int HardLink_send(uint8_t *packet,size_t size){
    //malloc has a risk of memory leaking


    /*rfc_CMD_PROP_TX_t RF_cmdPropTx={
        //CMD No, fixed for transmit
        .commandNo = 0x3801,
        .status = 0x0000,
        .pNextOp = 0,
        //immediate transmit
        .startTime = 0x00000000,
        .startTrigger.triggerType = TRIG_NOW,
        .startTrigger.bEnaCmd = 0x0,
        .startTrigger.triggerNo = 0x0,
        .startTrigger.pastTrig = 0x0,
        //No further commands, not apply
        .condition.rule = 0x1,
        .condition.nSkip = 0x0,
        //keep synthesizer on after transmit
        .pktConf.bFsOff = 0x0,
        //Do CRC
        .pktConf.bUseCrc = 0x1,
        //Fix the length
        .pktConf.bVarLen = 0x1,
        //TODO figure out what this thing does
        .syncWord = 0x930b51de

        rfc_CMD_PROP_TX_t RF_cmdPropTx =
{
    .commandNo = 0x3801,
    .status = 0x0000,
    .pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .pktConf.bFsOff = 0x0,
    .pktConf.bUseCrc = 0x1,
    .pktConf.bVarLen = 0x1,
    .pktLen = 0x1E, // SET APPLICATION PAYLOAD LENGTH
    .syncWord = 0x930B51DE,
    .pPkt = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
};
    };*/

    if(!packet){
        return -1;
    }

    //TODO encode packet
    RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;
    RF_cmdPropTx.pktLen=bytes_per_raw_bit;


    size_t i;
    for(i=0; i<size;i++){
        uint8_t byte = packet[i];
        uint8_t digit;
        for(digit=0;digit<8;digit++){
            if(byte & 1<<digit){
                RF_cmdPropTx.pPkt = prs_1;
            }
            else{
                RF_cmdPropTx.pPkt = prs_0;
            }
            RF_EventMask terminationReason = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx,RF_PriorityNormal, NULL, 0);
        }
    }

    return 0;
}

uint32_t HardLink_getFrequency(void)
{
    uint32_t freq_khz;
    if((!configured) || suspended)
    {
        return HardLink_Status_Config_Error;
    }

    freq_khz = HardLink_cmdFs.frequency * 1000000;
    // recover the fractional part, see setFrequency for definition
    freq_khz += (((uint64_t)HardLink_cmdFs.fractFreq * 1000000) / 65536);

    return freq_khz;
}

HardLink_Status HardLink_setFrequency(uint32_t ui32Frequency)
{
    HardLink_Status  status= HardLink_Status_Cmd_Error;
    uint16_t centerFreq, fractFreq;

    if((!configured) || suspended)
    {
        return HardLink_Status_Config_Error;
    }

    // set the frequency
    // get center frequency
    centerFreq = (uint16_t)(ui32Frequency / 1000000);
    // first calculate the fractional part of the frequency, for example, 
    // ui32Frequency = 1200000, then 200000 should be the fractional part 
    // then use 65536 (2^16) to enlarge the fractional part to avoid being 0
    fractFreq = (uint16_t)(((uint64_t)ui32Frequency - 
                            ((uint64_t)centerFreq * 1000000)) *
                            65536 / 1000000);

    HardLink_cmdFs.frequency = centerFreq;
    HardLink_cmdFs.fractFreq = fractFreq;

    // run command to set the frequency, returns RF_EventMask(termination reason)
    RF_EventMask result = RF_runCmd(rfHandle, (RF_Op*)&HardLink_cmdFs,
                RF_PriorityNormal, 0, HARD_LINK_CMD_MASK);

    // check status 
    if((result & RF_EventLastCmdDone) && (HardLink_cmdFs.status == DONE_OK))
    {
        status = HardLink_Status_Success;
    }
    return status;
}

HardLink_Status HardLink_getRfPower(int8_t *pi8TxPowerdBm)
{
    // set a initial value
    int8_t txPowerdBm = 0xff;
    if((!configured) || suspended)
    {
        return HardLink_Status_Config_Error;
    }

    uint8_t rfPowerTableSize = 0;
    RF_TxPowerTable_Entry *rfPowerTable = NULL;
    RF_TxPowerTable_Value currValue = RF_getTxPower(rfHandle);

    // value not valid
    if(currValue.rawValue == RF_TxPowerTable_INVALID_VALUE)
    {
        return HardLink_Status_Config_Error;
    }
    else
    {
        // if valid, found power of dBm unit in the look up table 
        if(currValue.paType == RF_TxPowerTable_DefaultPA)
        {
            rfPowerTable = (RF_TxPowerTable_Entry *)PROP_RF_txPowerTable;
            rfPowerTableSize = PROP_RF_txPowerTableSize;
        }
        txPowerdBm = RF_TxPowerTable_findPowerLevel(rfPowerTable, currValue);
    }

    *pi8TxPowerdBm = txPowerdBm;
    return HardLink_Status_Success;
}

HardLink_Status HardLink_setRfPower(int8_t i8TxPowerdBm)
{
    HardLink_Status status = HardLink_Status_Cmd_Error;

    if((!configured) || suspended)
    {
        return HardLink_Status_Config_Error;
    }

    RF_TxPowerTable_Value newValue;

    newValue = RF_TxPowerTable_findValue((RF_TxPowerTable_Entry*)PROP_RF_txPowerTable, i8TxPowerdBm);

    // didn't found a valid value
    if(newValue.rawValue == RF_TxPowerTable_INVALID_VALUE)
    {
        return HardLink_Status_Config_Error;
    }

    RF_Stat rfStatus = RF_setTxPower(rfHandle, newValue);
    if(rfStatus == RF_StatSuccess)
    {
        status = HardLink_Status_Success;
    }
    else 
    {
        status = HardLink_Status_Config_Error;
    }
    return status;
}
