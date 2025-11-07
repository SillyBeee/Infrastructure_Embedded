## 串口协议(一些配置要求)

######  通信方式为串口，配置为:常规链路的波特率为115200,图传链路的波特率为921600,数据位8位,停止位1位,无硬件流控,无流控.
![img.png](assets/protocol.png)


-----------
### 文件定义:

``dev_referee.c   
dev_referee.h``  实现了基础裁判系统接收数据与一些获取数据的接口，如果不用绘制UI,直接#include "dev_referee.h"即可

--------

``dev_referee_protocol.h`` 裁判系统接收数据结构体定义头文件

-------
``dev_referee_ui.c
dev_referee_ui.h``   在referee_dev.h的基础上实现了UI绘制等逻辑作为升级模块，如果需要绘制UI,将#include "dev_referee.h"改为dev_referee_ui.h即可


-------
省流(最简版):
![img_1.png](assets/example.png)

### 数据结构讲解
![img_2.png](assets/struct.png)
RefereeInstance_s中 origin_data是一个包含所有可接收数据结构体的结构体，每一个解析通过的数据帧都会实时存储更新到这里，你可以在这里获取裁判系统数据的原始数据


Referee_Data_TF 表示了当前数据是否可用，默认为false,当第一次收到完整的数据帧时变为true。


Referee_Self_ID与Referee_SelfClient_ID保存了当前机器人ID与客户端ID(不会自动更新),当调用

```cpp 
#获取当前机器人ID 
uint8_t Referee_Get_Robot_ID(RefereeInstance_s *ref_instance);
```

```cpp 
#获取当前客户端ID
uint16_t Referee_Get_Client_ID(RefereeInstance_s *ref_instance);
```
这两个辅助函数会进行更新与返回

cnt记录了当前完整接收数据帧的个数

其他辅助函数详见referee_dev.h注释
关于UI绘制辅助函数详见referee_ui.h注释









