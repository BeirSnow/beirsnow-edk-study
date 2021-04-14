此脚本用于 UEFI Shell 下重启测试

1. 将 startup.nsh 置于 fs0:\ 路径下
2. 默认启动项设置为 UEFI Shell
3. 首次运行或重置计数，使用 -start 参数：
   startup.nsh -start
4. 在 UEFI Shell 下可使用 edit 命令修改脚本
5. 累计重启次数信息保存在文件 Reboot.log 中
