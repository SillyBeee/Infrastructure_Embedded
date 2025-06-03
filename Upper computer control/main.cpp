#include <windows.h>
#include "PCANBasic.h"
#include <iostream>
#include "PCAN.hpp"
#include "motor_dm.h"
#include <memory>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>  // 添加计时相关头文件

std::mutex status_mutex;
std::mutex counter_mutex;  // 用于保护计数器

// 添加全局计数器
int receive_count = 0;
int control_count = 0;
std::chrono::time_point<std::chrono::steady_clock> last_recv_time;
std::chrono::time_point<std::chrono::steady_clock> last_ctrl_time;
bool first_recv = true;
bool first_ctrl = true;

void motor_recv_task(PCAN pcan, std::vector<std::shared_ptr<Motor>>& motors) {
    HANDLE iBuffer = CreateEventW(NULL, FALSE, FALSE, L"");
    TPCANStatus stsResult = CAN_SetValue(pcan.status.channel_handle, PCAN_RECEIVE_EVENT, &iBuffer, sizeof(iBuffer));

    if (stsResult != PCAN_ERROR_OK) {
        ShowStatus(stsResult);
        return;
    }

    int local_count = 0;
    auto output_time = std::chrono::steady_clock::now();
    
    while (1) {
        if (WaitForSingleObject(iBuffer, 50) == WAIT_OBJECT_0) {
            CANMessageData message;
            auto ret = pcan.ReadMessages(&message);
            if (ret == PCAN_ERROR_OK) {
                // 获取当前时间
                auto now = std::chrono::steady_clock::now();
                
                // 更新计数器
                counter_mutex.lock();
                if (first_recv) {
                    last_recv_time = now;
                    first_recv = false;
                }
                receive_count++;
                local_count++;
                counter_mutex.unlock();
                
                // 每10条消息输出一次接收频率
                if (local_count >= 10) {
                    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - last_recv_time).count();
                    if (elapsed_ms > 0) {
                        double freq_hz = (local_count * 1000.0) / elapsed_ms;
                        std::cout << "recv freq: " << freq_hz << " Hz (get " 
                                  << local_count << " messages,waste " 
                                  << elapsed_ms << " ms)" << std::endl;
                    }
                    local_count = 0;
                    counter_mutex.lock();
                    last_recv_time = now;
                    counter_mutex.unlock();
                }
                
                // 处理消息
                for (auto motor : motors) {
                    status_mutex.lock();
                    motor->Handle_CAN_Message(message);
                    status_mutex.unlock();
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    auto devices = GetAttachedDevices();
    std::cout << "number of devices: " << devices.size() << std::endl;
    PCANDeviceInfo target_pcan;
    for (auto device : devices) {
        PrintPCANDeviceInfo(device);
        target_pcan = device;
    }
    PCAN pcan(target_pcan);
    pcan.Initialize(PCAN_BAUD_1M);
    
    // 在main中创建智能指针
    std::shared_ptr<DM4310> motorPtr = std::make_shared<DM4310>(&pcan, MIT_TORQUE_MODE, 0x01, 0x11, SPEED_LOOP);
    motorPtr->pid_speed.Set_Parameters(0.5,0.0,0.000, 0.002f);
    motorPtr->pid_speed.Set_Max_Output(10);
    motorPtr->Enable();
    
    // 创建接收线程，使用引用
    std::thread recv_thread(motor_recv_task, std::ref(pcan), std::vector<std::shared_ptr<Motor>>{motorPtr});
    
    // 控制循环性能监控变量
    auto output_time = std::chrono::steady_clock::now();
    int local_count = 0;
    
    // 主线程中使用同一指针
    while (1) {
        auto start_time = std::chrono::steady_clock::now();
        
        status_mutex.lock();
        motorPtr->Set_Speed(3);  // 使用指针而非直接对象
        status_mutex.unlock();
        
        // 更新计数器
        counter_mutex.lock();
        if (first_ctrl) {
            last_ctrl_time = start_time;
            first_ctrl = false;
        }
        control_count++;
        local_count++;
        counter_mutex.unlock();
        
        // 每1000次控制循环输出一次频率
        if (local_count >= 1000) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - last_ctrl_time).count();
            if (elapsed_ms > 0) {
                double freq_hz = (local_count * 1000.0) / elapsed_ms;
                std::cout << "CTL freq: " << freq_hz << " Hz (finish " 
                          << local_count << " ctl,waste " 
                          << elapsed_ms << " ms)" << std::endl;
            }
            local_count = 0;
            counter_mutex.lock();
            last_ctrl_time = now;
            counter_mutex.unlock();
        }
        
        PreciseSleep(1);
    }
    
    recv_thread.join();
    return 0;
}