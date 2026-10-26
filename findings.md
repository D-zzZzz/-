# Findings: 93.mp4 最后一个弯未回正

## Requirements
- 分析视频最后一个弯为什么没有回正。
- 必须把可见图像/通道与当前源码分支对应起来。
- 不修改控制代码，只给出诊断和最小修正建议。

## Research Findings
- 视频分辨率 2558×1504，约 3.157 s；主画面流为 30 fps，共 95 帧。
- 已提取 0.00~3.00 s、间隔 0.15 s 的 21 张全屏帧。
- t=3.000 s 的助手图像帧号为 220：`A=100`、`debug_road_state=2`、`IF=0`、`Error=0`。
- 当前 `go.c` 中 `A=100` 是本帧未得到有效起始点的标志；因此末帧不是仍在输出普通弯道识别码。
- 末帧仍显示红/蓝边线不代表本帧重新搜到了线：`go()` 失败分支不会调用 `search_l_r()`，而调试发送仍可能显示上一帧残留的左右线数组。

## Technical Decisions
| Decision | Rationale |
|----------|-----------|
| 使用技能自带的逐飞视频提取脚本 | 能稳定输出全屏、相机裁剪、接触表与时间元数据 |

## Issues Encountered
| Issue | Resolution |
|-------|------------|
| Bundled Python and skill crop step lack `cv2` | Keep generated full frames and use system video tools |
| `ffmpeg` is installed but not present in this PowerShell PATH | Locate the WinGet executable and call it by absolute path |
| Earlier visual-findings patch attempts did not match file context | Reapplied against stable UTF-8 section anchors |

## Resources
- C:\Users\丁子皓\OneDrive\Desktop\赛道图\93.mp4
- E:\TCB\2\Chezi\code\go.c
- E:\TCB\2\Chezi\code\deal_image.c
- E:\TCB\2\Chezi\code\pid.c

## Visual Findings
- t=0.000 s 为黑屏；t=3.000 s 已进入最后画面。
- t=3.000 s 相机区白色赛道挤在左侧，蓝色右边线向右下弯后中断，画面中没有明显紫色中线。
- 中线消失与 `go()` 在起始点检测前清空 `mid_line[]`、随后走失败分支一致。
- ffmpeg 全片接触表显示，最后一段集中在助手帧号约 184、191、198、207、219；赛道由右弯/斜直道逐渐退到视野左缘，最后彻底失去可用近场入口。
- 这一段 `debug_road_state` 可见仍为 2，需用原始分辨率逐帧确认 `A` 与 `Error` 的精确跳变。
- t=2.400 s（助手帧 186）：`debug_turn_cmd=6`、`A=50`、`debug_road_state=2`、`IF=2`、`Error=77`；紫色中线大幅指向右上，仍在给很大的右转误差。
- t=2.550 s（助手帧 193）：`debug_turn_cmd=7`、`A=52`、`debug_road_state=1`、`IF=2`、`Error=75`；已出现右弯退出候选码，但误差基本没有回落。
- 因此“完全没识别到出弯”不是主因；更直接的异常是出弯候选出现后，控制误差仍被补线/前瞻点维持在约 +75。
- t=2.700 s（助手帧 200）：`A=9`、`debug_road_state=2`、`IF=0`、`Error=4`；紫色中线已回到接近当前赛道中心，视觉误差实际已经回正。
- t=2.850 s（助手帧 207）：`A=98`、`debug_road_state=2`、`IF=0`、`Error=0`；中线消失，随后到帧 220 变为 `A=100`，表明末段问题发生在回正之后的再次失线。
- `debug_turn_cmd` 从帧 193 起为 7，并在帧 200/207/220 保持；需对照其枚举含义，判断是否仍有陀螺转向命令残留。
