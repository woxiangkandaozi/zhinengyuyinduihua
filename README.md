# My_frits_-
这是我的第一个项目，芝士一个关于智能语音对话芯片的程序，可以搞重复播放并且定时播放以及情景对话，不过也是一坨屎，具体我会在再_asr.cpp上标注释
这个是基于天问block开发的程序我们只需要下载一个天问block然后把这个拖拽到asrpro这个文件下就可以了
至于更改的话：
ASR：修改命令词
ASRTO：修改回复语句
ID：最重要的，你每一个回复语句都要匹配相应的ID
你们如果想要修改要播放的
只需要prompt_play_by_cmd_id后（）内第一个数字
如：prompt_play_by_cmd_id(40, -1, play_end_callback, false);
我想让他播放20这条语音只需要：
    prompt_play_by_cmd_id(20, -1, play_end_callback, false);
