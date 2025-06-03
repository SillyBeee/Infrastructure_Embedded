#include "../include/PCAN.hpp"
#include <iostream>
#include <cstring>
#pragma comment(lib, "winmm.lib") 

PCAN::PCAN(PCANDeviceInfo status , bool is_FD ) {

    this->status = status;
    this->is_FD = is_FD;
}


TPCANStatus PCAN::Initialize(TPCANBaudrate baudrate) {
    if(this->is_FD) {
        std::cerr << "Error: Attempting to initialize a PCAN object with non-FD settings." << std::endl;
        return PCAN_ERROR_ILLPARAMVAL;
    }
    return CAN_Initialize(this->status.channel_handle, baudrate, 0, 0, 0);
}

TPCANStatus PCAN::InitializeFD(TPCANBitrateFD baudrate){
    if(!this->is_FD) {
        std::cerr << "Error: Attempting to initialize a non-FD PCAN object with FD settings." << std::endl;
        return PCAN_ERROR_ILLPARAMVAL;
    }
    return CAN_InitializeFD(this->status.channel_handle, baudrate);
}
TPCANStatus PCAN::Uninitialize() {
    return CAN_Uninitialize(this->status.channel_handle);
}

TPCANStatus PCAN::ReadMessages(CANMessageData* message) {
    if (this->is_FD) {
        return ReadMessageFD(message);
    } 
    else {
        return ReadMessage(message);
    }
}

TPCANStatus PCAN::ReadMessage(CANMessageData* message) {
    TPCANMsg msg;
    TPCANTimestamp timestamp;
    TPCANStatus status = CAN_Read(this->status.channel_handle, &msg, &timestamp);

    if (status == PCAN_ERROR_OK) {
        message->id = msg.ID;
        message->length = msg.LEN;
        memcpy(message->data, msg.DATA, msg.LEN);
    }
    return status;
}

TPCANStatus PCAN::ReadMessageFD(CANMessageData* message) {
    TPCANMsgFD msg;
    TPCANTimestampFD timestamp;
    TPCANStatus status = CAN_ReadFD(this->status.channel_handle, &msg, &timestamp);

    if (status == PCAN_ERROR_OK) {
        message->id = msg.ID;
        message->length = msg.DLC;
        memcpy(message->data, msg.DATA, msg.DLC);
    }
    return status;
}
TPCANStatus PCAN::SendMessages(DWORD id, BYTE* data, BYTE length, bool extended) {
    if (this->is_FD) {
        return SendMessageFD(id, data, length, extended);
    } else {
        return SendMessage(id, data, length, extended);
    }
}

TPCANStatus PCAN::SendMessage(DWORD id, BYTE* data, BYTE length, bool extended){
    if (length > 8) {
        return PCAN_ERROR_ILLPARAMVAL;
    }

    TPCANMsg message;
    message.ID = id;
    message.MSGTYPE = extended ? PCAN_MESSAGE_EXTENDED : PCAN_MESSAGE_STANDARD;
    message.LEN = length;
    
    for (int i = 0; i < length; i++) {
        message.DATA[i] = data[i];
    }

    return CAN_Write(this->status.channel_handle, &message);
}

TPCANStatus PCAN::SendMessageFD(DWORD id, BYTE* data, BYTE length, bool extended) {
    if (length > 64) {
        return PCAN_ERROR_ILLPARAMVAL;
    }

    TPCANMsgFD message;
    message.ID = id;
    message.MSGTYPE = PCAN_MESSAGE_FD | PCAN_MESSAGE_BRS;
    message.DLC = length;

    for (int i = 0; i < length; i++) {
        message.DATA[i] = data[i];
    }
    return CAN_WriteFD(this->status.channel_handle, &message);
}

TPCANStatus PCAN::GetChannelStatus() {
    return CAN_GetStatus(this->status.channel_handle);
}
PCAN::~PCAN() {
    Uninitialize();
    std::cout << "PCAN object destroyed" << std::endl;
}