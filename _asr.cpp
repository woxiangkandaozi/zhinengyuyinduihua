#include "asr.h"
extern "C"{ void * __dso_handle = 0 ;}
#include "setup.h"
#include "myLib/asr_event.h"
#include "asr_math.h"
#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>

// 函数声明
uint32_t snid;
void ASR_CODE();
QueueHandle_t asr_code_handler_msg = NULL;
void asr_code_handler_app(void* param);
void stimer_1(TimerHandle_t xTimer);
TimerHandle_t softtimer_1 = NULL;

// 初始化变量
int my_min = 0;
int fourthree = 0;
int fourfour = 0;
int fourfive = 0;
int cishu = 0;
int scene_mode = 0; // 0:无情景, 7:晚上, 8:早上, 23:中午
int jishu_count = 0; // 定时器计数器

// 重复播放不同语音
void chongfu(int my_xuhao)
{
    cishu++;
    if (softtimer_1) {
        xTimerReset(softtimer_1, 0);
        xTimerStart(softtimer_1, 0);
    }

    switch (cishu) 
    {
        case 1:
            prompt_play_by_cmd_id(my_xuhao, -1, play_end_callback, false);
            break;
        case 2:
            prompt_play_by_cmd_id(40, -1, play_end_callback, false);
            break;
        case 3:
            prompt_play_by_cmd_id(41, -1, play_end_callback, false);
            break;
        case 4:
            prompt_play_by_cmd_id(42, -1, play_end_callback, false);
            cishu = 0;
            break;
    }
}

// 音量调节
void my_set(int my_xuhao)
{
    if (softtimer_1) {
        xTimerReset(softtimer_1, 0);
        xTimerStart(softtimer_1, 0);
    }

    switch (my_xuhao) 
    {
        case 101: vol_set(1); break;
        case 102: vol_set(2); break;
        case 103: vol_set(3); break;
        case 104: vol_set(4); break;
        case 105: vol_set(5); break;
        case 106: vol_set(6); break;
        case 107: vol_set(7); break;
    }

    prompt_play_by_cmd_id(60, -1, play_end_callback, false);
}

// 播放语音
void bofang(int my_xuhao)
{
    if (my_xuhao == 7 || my_xuhao == 8 || my_xuhao == 23)
    {
        scene_mode = my_xuhao;
        chongfu(my_xuhao);
    }
    else if (my_xuhao == 10 || my_xuhao == 13 || my_xuhao == 33)
    {
        // 情景对话
        xTimerReset(softtimer_1, 0);
        if (my_xuhao == 13) {
            if (scene_mode == 7) prompt_play_by_cmd_id(46, -1, play_end_callback, false);
            else if (scene_mode == 8) prompt_play_by_cmd_id(47, -1, play_end_callback, false);
            else if (scene_mode == 23) prompt_play_by_cmd_id(48, -1, play_end_callback, false);
            else chongfu(my_xuhao);
        } else if (my_xuhao == 10) {
            if (scene_mode == 7) prompt_play_by_cmd_id(49, -1, play_end_callback, false);
            else if (scene_mode == 8) prompt_play_by_cmd_id(50, -1, play_end_callback, false);
            else if (scene_mode == 23) prompt_play_by_cmd_id(51, -1, play_end_callback, false);
            else chongfu(my_xuhao);
        } else if (my_xuhao == 33) {
            if (scene_mode == 7) prompt_play_by_cmd_id(51, -1, play_end_callback, false);
            else if (scene_mode == 8) prompt_play_by_cmd_id(52, -1, play_end_callback, false);
            else if (scene_mode == 23) prompt_play_by_cmd_id(53, -1, play_end_callback, false);
            else chongfu(my_xuhao);
        }
    }
    else if (my_xuhao >= 101 && my_xuhao <= 107)
    {
        my_set(my_xuhao);
    }
    else
    {
        chongfu(my_xuhao);
    }
}


// 待机语音定时器回调
void stimer_1(TimerHandle_t xTimer)
{
    int count = rand() % 3; // 随机数
    jishu_count = fourthree + fourfour + fourfive;

    if (jishu_count < 5)
    {
        switch (count)
        {
            case 0: if (prompt_play_by_cmd_id(43, -1, play_end_callback, false)) fourthree++; break;
            case 1: if (prompt_play_by_cmd_id(44, -1, play_end_callback, false)) fourfour++; break;
            case 2: if (prompt_play_by_cmd_id(45, -1, play_end_callback, false)) fourfive++; break;
        }
    }
}

// ASR 识别处理
void ASR_CODE()
{
    // TODO: 在这里填充 ASR 识别后发送消息到队列
    // 例如测试：
    if(asr_code_handler_msg)
    {
        uint16_t test_snid = 7;
        xQueueSend(asr_code_handler_msg, &test_snid, 0);
    }
}

// ASR 队列处理任务
void asr_code_handler_app(void* param)
{
    uint16_t asr_code_handler_snid;
    while (1)
    {
        if (xQueueReceive(asr_code_handler_msg, &asr_code_handler_snid, pdMS_TO_TICKS(100)))
        {
            bofang(asr_code_handler_snid);
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

// 硬件初始化
void hardware_init()
{
    softtimer_1 = xTimerCreate("stimer_1", pdMS_TO_TICKS(60000), pdTRUE, 0, stimer_1);
    asr_code_handler_msg = xQueueCreate(5, sizeof(uint16_t));

    vol_set(4); // 初始化音量
}

// setup 初始化
//如果你遇到播放音频没有声音，你可以把你要播放的音频在这里生成一遍
//如：//{playid:53,voice:准备去抢特价的通心粉}
void setup()
{
    hardware_init();

    if (softtimer_1)
        xTimerStart(softtimer_1, 0);
  //{ID:1,keyword:"命令词",ASR:"芙宁娜",ASRTO:"我在"}
  //情景
  //{ID:7,keyword:"命令词",ASR:"晚上好",ASRTO:""}
  //{ID:8,keyword:"命令词",ASR:"早上好",ASRTO:""} 
  //{ID:23,keyword:"命令词",ASR:"中午好",ASRTO:""}
  //触发条件改变
  //{ID:13,keyword:"命令词",ASR:"你在想什么呀",ASRTO:""}
  //{ID:10,keyword:"命令词",ASR:"芙芙你还好吗",ASRTO:""}
  //{ID:33,keyword:"命令词",ASR:"你在干什么",ASRTO:""}

  //{ID:0,keyword:"命令词",ASR:"我最近可能离开你一段时间",ASRTO:""}
  //{ID:2,keyword:"命令词",ASR:"给你好吃的",ASRTO:""}
  //{ID:3,keyword:"命令词",ASR:"好无聊呀",ASRTO:""}
  //{ID:4,keyword:"命令词",ASR:"你不喜欢吗",ASRTO:""}
  //{ID:5,keyword:"命令词",ASR:"你好呀",ASRTO:""}
  //{ID:6,keyword:"命令词",ASR:"外面下雪了",ASRTO:""}
  //{ID:9,keyword:"命令词",ASR:"不要离开我好吗",ASRTO:""}
  //{ID:11,keyword:"命令词",ASR:"和我聊聊天吧",ASRTO:""}
  //{ID:12,keyword:"命令词",ASR:"你好受欢迎呀",ASRTO:""}
  //{ID:14,keyword:"命令词",ASR:"我们出去玩吧",ASRTO:""}
  //{ID:15,keyword:"命令词",ASR:"今天我生日哦",ASRTO:""}
  //{ID:16,keyword:"命令词",ASR:"芙芙怎么了",ASRTO:""}
  //{ID:17,keyword:"命令词",ASR:"好大的雨呀",ASRTO:""}
  //{ID:18,keyword:"命令词",ASR:"快走快走",ASRTO:""}
  //{ID:19,keyword:"命令词",ASR:"你好厉害呀",ASRTO:""}
  //{ID:20,keyword:"命令词",ASR:"你是谁呀",ASRTO:""}
  //{ID:21,keyword:"命令词",ASR:"晚安",ASRTO:""}
  //{ID:22,keyword:"命令词",ASR:"我们出发吧",ASRTO:""}
  //{ID:24,keyword:"命令词",ASR:"茶会是什么呀",ASRTO:""}
  //{ID:25,keyword:"命令词",ASR:"好大的太阳呀",ASRTO:""}
  //{ID:26,keyword:"命令词",ASR:"欢乐日是什么呀",ASRTO:""}
  //{ID:27,keyword:"命令词",ASR:"你喜欢吃什么",ASRTO:""}
  //{ID:28,keyword:"命令词",ASR:"我很关心你的",ASRTO:""}
  //{ID:29,keyword:"命令词",ASR:"我们关系怎么样呀",ASRTO:""}
  //{ID:30,keyword:"命令词",ASR:"自我介绍下",ASRTO:""}
  //{ID:31,keyword:"命令词",ASR:"今天天气真好",ASRTO:""}
  //{ID:32,keyword:"命令词",ASR:"芙芙有你的信",ASRTO:""}
  //{ID:34,keyword:"命令词",ASR:"今天的剧目是什么",ASRTO:""}
  //{ID:35,keyword:"命令词",ASR:"有什么想对芙卡洛斯说的吗",ASRTO:""}
  //{ID:36,keyword:"命令词",ASR:"我今天觉得有点不舒服",ASRTO:""}
  //{ID:37,keyword:"命令词",ASR:"冰箱里的蛋糕好好吃",ASRTO:""}
  //{ID:38,keyword:"命令词",ASR:"我喜欢你",ASRTO:""}
  //{ID:39,keyword:"命令词",ASR:"你可以和我交往吗",ASRTO:""}
  //{playid:40,voice:呃...你怎么一句话说两遍？}
  //{playid:41,voice:你难道要一直这样做个复读机关吗？}
  //{playid:42,voice:你再这样我就不理你了！}
  //{playid:43,voice:唉...好无聊啊。 没有什么更有趣的事吗？}
  //{playid:44,voice:今天晚上吃些什么好呢？}
  //{playid:45,voice:生活就像演出，得丰富一点才有趣嘛...}
  //{playid:46,voice:当然啦，来一起看星星！}
  //{playid:47,voice:还好啦，有点困…可以给我一杯咖啡吗，多点奶少点糖…}
  //{playid:48,voice:我…我没事啦！快想想我中午吃什么}
  //{playid:49,voice:我正在想要不要邀请你散个步呢}
  //{playid:50,voice:我在想的有很多，像夜空中数不清的星星}
  //{playid:51,voice:在想配什么，肉酱、番茄酱还是…}
  //{playid:52,voice:在散步啦，怎么，要陪我吗}
  //{playid:53,voice:准备去抢特价的通心粉}
  //{playid:54,voice:我在煮通心粉配肉酱啦，下周该配番茄酱了}
  //{ID:101,keyword:"命令词",ASR:"音量一",ASRTO:""}
  //{ID:102,keyword:"命令词",ASR:"音量二",ASRTO:""}
  //{ID:103,keyword:"命令词",ASR:"音量三",ASRTO:""}
  //{ID:104,keyword:"命令词",ASR:"音量四",ASRTO:""}
  //{ID:105,keyword:"命令词",ASR:"音量五",ASRTO:""}
  //{ID:106,keyword:"命令词",ASR:"音量六",ASRTO:""}
  //{ID:107,keyword:"命令词",ASR:"音量七",ASRTO:""}

    xTaskCreate(asr_code_handler_app, "asr_code_handler_app", 256, NULL, 4, NULL);

    set_wakeup_forever(); // 永远唤醒
}

