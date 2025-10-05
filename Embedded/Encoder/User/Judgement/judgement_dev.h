#ifndef JUDGEMENT_H
#define JUDGEMENT_H
#include <stdint.h>

#define _packed __attribute__((packed))

typedef _packed struct
{
    //Command 0x0001
    uint8_t game_type :4;     //1:对抗赛 2:单项赛 3:人工智能赛 4:联盟赛 5:步兵对抗赛
    uint8_t game_progress :4;  //0:未开始 1:准备阶段 2:自检阶段 3:5秒倒计时 4:比赛中 5:比赛结算中
    uint16_t stage_remaining_time; //当前阶段剩余时间，单位秒
    uint64_t SyncTimeStamp;  //UNIX时间，当机器人正确连接到裁判系统的NTP服务器后生效
}game_status_t;

typedef _packed struct
{   //Command 0x0002
    uint8_t winner;  //0:平局 1:红方获胜 2:蓝方获胜
}game_result_t;

typedef _packed struct
{   //Command 0x0003
    uint16_t red_1_robot_HP; //红方1号机器人血量
    uint16_t red_2_robot_HP; //红方2号机器人血量
    uint16_t red_3_robot_HP; //红方3号机器人血量
    uint16_t red_4_robot_HP; //红方4号机器人血量
    uint16_t reserved1;
    uint16_t red_7_robot_HP; //红方7号机器人血量
    uint16_t red_outpost_HP; //红方前哨站血量
    uint16_t red_base_HP; //红方基地血量
    uint16_t blue_1_robot_HP; //蓝方1号机器人血量
    uint16_t blue_2_robot_HP; //蓝方2号机器人血量
    uint16_t blue_3_robot_HP; //蓝方3号机器人血量
    uint16_t blue_4_robot_HP; //蓝方4号机器人血量
    uint16_t reserved2;
    uint16_t blue_7_robot_HP;//蓝方7号机器人血量
    uint16_t blue_outpost_HP; //蓝方前哨站血量
    uint16_t blue_base_HP; //蓝方基地血量
}game_robot_HP_t;

typedef _packed struct
{   //Command 0x0101
    uint32_t event_data;
}event_data_t;

typedef _packed struct
{   //Command 0x0104
    uint8_t level;  //受罚等级 1:双方黄牌 2.黄牌 3.红牌 4.判负
    uint8_t offending_robot_id; //己方受罚机器人ID，判负和双方黄牌值为0
    uint8_t count; //己方最后一次受到判罚的违规机器人对应判罚等级的违规次数
}referee_warning_t;

typedef _packed struct
{   //Command 0x0105
    uint8_t dart_remaining_time; //飞镖发射剩余时间，单位秒
    uint16_t dart_info;  //
}dart_info_t;

typedef _packed struct
{   //Command 0x0201
    uint8_t robot_id;
    uint8_t robot_level;
    uint16_t current_HP;
    uint16_t maximum_HP;
    uint16_t shooter_barrel_cooling_value;
    uint16_t shooter_barrel_heat_limit;
    uint16_t chassis_power_limit;
    uint8_t power_management_gimbal_output : 1;
    uint8_t power_management_chassis_output : 1;
    uint8_t power_management_shooter_output : 1;
}robot_status_t;

typedef _packed struct
{   //Command 0x0202
    uint16_t reserved1;
    uint16_t reserved2;
    float reserved3;
    uint16_t buffer_energy;
    uint16_t shooter_17mm_1_barrel_heat;
    uint16_t shooter_17mm_2_barrel_heat;
    uint16_t shooter_42mm_barrel_heat;
}power_heat_data_t;

typedef _packed struct
{   //Command 0x0203
    float x;
    float y;
    float angle;
}robot_pos_t;

typedef _packed struct
{   //Command 0x0204
    uint8_t recovery_buff;
    uint8_t cooling_buff;
    uint8_t defence_buff;
    uint8_t vulnerability_buff;
    uint16_t attack_buff;
    uint8_t remaining_energy;
}buff_t;

typedef _packed struct
{   //Command 0x0206
    uint8_t armor_id : 4;
    uint8_t HP_deduction_reason : 4;
}hurt_data_t;

typedef _packed struct
{   //Command 0x0207
    uint8_t bullet_type;
    uint8_t shooter_number;
    uint8_t launching_frequency;
    float initial_speed;
}shoot_data_t;

typedef _packed struct
{   //Command 0x0208
    uint16_t projectile_allowance_17mm;
    uint16_t projectile_allowance_42mm;
    uint16_t remaining_gold_coin;
}projectile_allowance_t;

typedef _packed struct
{   //Command 0x0209
    uint32_t rfid_status;
}rfid_status_t;

typedef _packed struct
{   //Command 0x020A
    uint8_t dart_launch_opening_status;
    uint8_t reserved1;
    uint16_t target_change_time;
    uint16_t latest_launch_cmd_time;
}dart_client_cmd_t;

typedef _packed struct
{   //Command 0x020B
    float hero_x;
    float hero_y;
    float engineer_x;
    float engineer_y;
    float standard_3_x;
    float standard_3_y;
    float standard_4_x;
    float standard_4_y;
    float reserved1;
    float reserved2;
}ground_robot_position_t;

typedef _packed struct
{   //Command 0x020C
    uint8_t mark_progress;
}radar_mark_data_t;

typedef _packed struct
{   //Command 0x020D
    uint32_t sentry_info;
    uint16_t sentry_info_2;
} sentry_info_t;

typedef _packed struct
{   //Command 0x020E
    uint8_t radar_info;
} radar_info_t;



//画UI的
typedef _packed struct
{   //Command 0x0301
    uint16_t data_cmd_id;
    uint16_t sender_id;
    uint16_t receiver_id;
    uint8_t user_data[];
}robot_interaction_data_t;

typedef _packed struct
{   //SubCommand 0x0100
    uint8_t delete_type;
    uint8_t layer;
}interaction_layer_delete_t;

typedef _packed struct
{   //SubCommand 0x0101
    uint8_t figure_name[3];
    uint32_t operate_tpye:3;
    uint32_t figure_tpye:3;
    uint32_t layer:4;
    uint32_t color:4;
    uint32_t details_a:9;
    uint32_t details_b:9;
    uint32_t width:10;
    uint32_t start_x:11;
    uint32_t start_y:11;
    uint32_t details_c:10;
    uint32_t details_d:11;
    uint32_t details_e:11;
}interaction_figure_t;

typedef _packed struct
{   //SubCommand 0x0102
    interaction_figure_t interaction_figure[2];
}interaction_figure_2_t;

typedef _packed struct
{   //SubCommand 0x0103
    interaction_figure_t interaction_figure[5];
}interaction_figure_3_t;

typedef _packed struct
{   //SubCommand 0x0104
    interaction_figure_t interaction_figure[7];
}interaction_figure_4_t;

typedef _packed struct
{   //SubCommand 0x0110
    graphic_data_struct_t grapic_data_struct;
    uint8_t data[30];
} ext_client_custom_character_t;

typedef _packed struct
{   //SubCommand 0x0120
    uint32_t sentry_cmd;
} sentry_cmd_t;

typedef _packed struct
{   //SubCommand 0x0121
    uint8_t radar_cmd;
} radar_cmd_t;




typedef _packed struct
{   //Command 0x0303
    float target_position_x;
    float target_position_y;
    uint8_t cmd_keyboard;
    uint8_t target_robot_id;
    uint16_t cmd_source;
}map_command_t;


//选手端接收数据
typedef _packed struct
{   //Command 0x0305
    uint16_t hero_position_x;
    uint16_t hero_position_y;
    uint16_t engineer_position_x;
    uint16_t engineer_position_y;
    uint16_t infantry_3_position_x;
    uint16_t infantry_3_position_y;
    uint16_t infantry_4_position_x;
    uint16_t infantry_4_position_y;
    uint16_t infantry_5_position_x;
    uint16_t infantry_5_position_y;
    uint16_t sentry_position_x;
    uint16_t sentry_position_y;
} map_robot_data_t;

typedef _packed struct
{   //Command 0x0307
    uint8_t intention;
    uint16_t start_position_x;
    uint16_t start_position_y;
    int8_t delta_x[49];
    int8_t delta_y[49];
    uint16_t sender_id;
}map_data_t;

typedef _packed struct
{   //Command 0x0308
    uint16_t sender_id;
    uint16_t receiver_id;
    uint8_t user_data[30];
} custom_info_t;



//图传链路相关数据
typedef _packed struct
{   //Command 0x0302
    uint8_t data[x];
}custom_robot_data_t;

typedef _packed struct
{   //Command 0x0309
    uint8_t data[x];
}robot_custom_data_t;

typedef _packed struct
{  //Command 0x0304
int16_t mouse_x;
int16_t mouse_y;
int16_t mouse_z;
int8_t left_button_down;
int8_t right_button_down;
uint16_t keyboard_value;
uint16_t reserved;
}remote_control_t;

//非链路数据
typedef _packed struct
{   //Command 0x0306
    uint16_t key_value;
    uint16_t x_position:12;
    uint16_t mouse_left:4;
    uint16_t y_position:12;
    uint16_t mouse_right:4;
    uint16_t reserved;
}custom_client_data_t;

#endif //JUDGEMENT_H
