#pragma once
#include <windows.h>
#include "PCANBasic.h"
#include <iostream>
#include <vector>
#include <string>
#include <mmsystem.h>
struct PCANDeviceInfo {
    TPCANHandle channel_handle;        // PCAN 通道句柄
    TPCANDevice device_type;           // 设备类型 (如 PCAN_USB, PCAN_PCI 等)
    BYTE controller_number;            // CAN 控制器编号
    DWORD device_features;             // 设备特性标志
    std::string device_name;           // 设备名称
    DWORD device_id;                   // 设备 ID 号
    DWORD channel_condition;           // 通道状态
};

struct CANMessageData {
    DWORD id;
    BYTE data[64];  
    BYTE length;
};



class PCAN {
public:
    PCAN(PCANDeviceInfo status , bool is_FD = false);

    PCAN() = delete;

    TPCANStatus Initialize(TPCANBaudrate baudrate);

    TPCANStatus InitializeFD(TPCANBitrateFD baudrate);

    TPCANStatus Uninitialize();

    TPCANStatus SendMessages(DWORD id, BYTE* data, BYTE length, bool extended = false);

    TPCANStatus ReadMessages(CANMessageData* message);

    TPCANStatus GetChannelStatus();
    
    ~PCAN();

    PCANDeviceInfo  status;
private:
    TPCANStatus SendMessage(DWORD id, BYTE* data, BYTE length, bool extended);
    TPCANStatus SendMessageFD(DWORD id, BYTE* data, BYTE length, bool extended);

    TPCANStatus ReadMessage(CANMessageData* message);
    TPCANStatus ReadMessageFD(CANMessageData* message);

    bool is_FD = false;

};



inline void PrintPCANDeviceInfo(const PCANDeviceInfo& dev) {
    std::cout << "PCAN Device Info:" << std::endl;
    std::cout << "  Channel Handle: 0x" << std::hex << dev.channel_handle << std::dec << std::endl;
    std::cout << "  Device Type: " << dev.device_type << std::endl;
    std::cout << "  Controller Number: " << static_cast<int>(dev.controller_number) << std::endl;
    std::cout << "  Device Features: 0x" << std::hex << dev.device_features << std::dec << std::endl;
    std::cout << "  Device Name: " << dev.device_name << std::endl;
    std::cout << "  Device ID: " << dev.device_id << std::endl;
    std::cout << "  Channel Condition: " << dev.channel_condition << std::endl;
}



    // 获取错误信息文本
inline std::string GetErrorText(TPCANStatus error) {
    char buffer[256];
    if (CAN_GetErrorText(error, 0x09, buffer) != PCAN_ERROR_OK) {
        return "无法获取错误描述";
    }
    return std::string(buffer);
}


// 检测并返回所有连接的 PCAN 设备
inline std::vector<PCANDeviceInfo> GetAttachedDevices() {
    std::vector<PCANDeviceInfo> devices;
    DWORD deviceCount = 0;
    TPCANStatus status;

    // 1. 获取连接的设备数量
    status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS_COUNT, &deviceCount, sizeof(deviceCount));
    if (status != PCAN_ERROR_OK) {
        std::cerr << "获取设备数量失败: " << GetErrorText(status) << std::endl;
        return devices;
    }

    // 如果没有设备连接
    if (deviceCount == 0) {
        return devices;
    }

    // 2. 分配足够的内存来存储设备信息
    TPCANChannelInformation* channelInfos = new TPCANChannelInformation[deviceCount];
    memset(channelInfos, 0, deviceCount * sizeof(TPCANChannelInformation));

    // 3. 获取所有设备的信息
    status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS, 
                         channelInfos, deviceCount * sizeof(TPCANChannelInformation));
    
    if (status != PCAN_ERROR_OK) {
        std::cerr << "获取设备信息失败: " << GetErrorText(status) << std::endl;
        delete[] channelInfos;
        return devices;
    }

    // 4. 转换为我们自定义的结构体
    for (DWORD i = 0; i < deviceCount; i++) {
        PCANDeviceInfo device;
        device.channel_handle = channelInfos[i].channel_handle;
        device.device_type = channelInfos[i].device_type;
        device.controller_number = channelInfos[i].controller_number;
        device.device_features = channelInfos[i].device_features;
        device.device_name = channelInfos[i].device_name;
        device.device_id = channelInfos[i].device_id;
        device.channel_condition = channelInfos[i].channel_condition;
        
        devices.push_back(device);
    }

    delete[] channelInfos;
    return devices;
}

inline void GetFormattedError(TPCANStatus error, LPSTR buffer)
{
    // Gets the text using the GetErrorText API function. If the function success, the translated error is returned. 
    // If it fails, a text describing the current error is returned.
    if (CAN_GetErrorText(error, 0x09, buffer) != PCAN_ERROR_OK)
        sprintf_s(buffer, MAX_PATH, "An error occurred. Error-code's text (%Xh) couldn't be retrieved", error);
}
inline void ShowStatus(TPCANStatus status)
{
    std::cout << "=========================================================================================\n";
    char buffer[MAX_PATH];
    GetFormattedError(status, buffer);
    std::cout << buffer << "\n";
    std::cout << "=========================================================================================\n";
}

inline void PreciseSleep(DWORD milliseconds) {
    // 提高系统定时器精度
    timeBeginPeriod(1);
    
    // 使用Sleep
    Sleep(milliseconds);
    
    // 恢复系统定时器精度
    timeEndPeriod(1);
}
