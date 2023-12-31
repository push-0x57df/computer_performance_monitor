# 计算机性能显示器Windows端数据采集程序

本程序设计用于采集windows操作系统上的性能参数，并通过串口自动搜索本项目的小屏幕硬件，通过串口将数据下发到外接的小屏幕进行显示。

目前采集以下参数：

- CPU：总体利用率；温度
- 物理内存：总体利用率；已使用内存量；已安装内存总量
- 显卡：核心利用率；核心温度；显存使用率；显存已用量；显存总量；

目前确认支持的计算机硬件：

- 安装有单个CPU的电脑，支持intel的酷睿系列
- 安装有单张英伟达显卡的电脑，其上必须安装英伟达显卡驱动程序。且支持英伟达第二代数据开放接口
- 安装有标准内存的电脑，支持多条内存的统计

## 编译环境搭建

本程序的设计目标就是尽可能的简约，所以没有使用过多的第三方库和技术。因此本程序的源文件使用visual studio这款微软官方IDE即可完成编译运行和调试

## 相关开源资料

与其匹配的外接小屏幕开源资料：

- 嵌入式程序：https://github.com/push-0x57df/computer_performance_monitor_rp_2040
- 硬件设计资料：https://oshwhub.com/push_a/xing-neng-jian-shi-qi

已实现的串口通信协议见 https://github.com/push-0x57df/computer_performance_monitor_rp_2040 的 readme 部分
