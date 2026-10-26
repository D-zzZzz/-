# Findings: 94.mp4 连续两个 90 度弯

## Requirements
- 分析连续两个 90 度弯时第二弯是否转得太早。
- 判断优先调整 PID 还是图像/状态机代码。
- 只诊断和给建议，不修改控制代码。

## Current Evidence
- 94.mp4 为 2558×1492，约 0.704 s，视频流 22 帧，适合全量逐帧检查。
- 项目当前没有 `.codegraph/` 索引，本次使用当前源码直接读取；未初始化或修改项目索引。
- 已用 ffmpeg 无损提取全部 22 帧并生成 6×4 全片接触表。
- 视频创建于约 15:24；当前 `pid.c` 修改于 15:32、`deal_image.c` 与固件构建于约 15:36，说明录像后代码已经变更并重新编译。
- 视频中 `debug_turn_cmd=1` 时进入 `ROAD_RIGHT_TURN`，意味着录像固件当时的 `Turn_Command[0]==1`；当前源码却是 `Turn_Command[0]==2`，脚本表已不一致。
- 当前源码仍保留 `start_point_ok==0` 时 `Error=Last_Error`，因此视频暴露的“大误差丢线后盲保持”风险在现代码中仍然存在。
- 已确认当前通道映射：0=`debug_turn_cmd`，1=`A`，2=`debug_road_state`，3/4=`motorL_Set/motorR_Set`，5/6=`Actual_Speed_L/R`，7=`Length`，8=`IF`，9=`Error`，10=`debug_frame_id`。
- 录像没有发送 `speed_out`、`turn_yaw_deg`，所以不能从该视频量化 PID 各项或 yaw 门槛；但可以直接确认进入 PID 前的 Error 已经错误跳大并在失线时保持。

## Code Questions
- 第二个拐点是否通过 `interrupt_turn` 提前抢断进入新状态？
- `Junction_Index` 是否过早前移，导致下一条指令提前生效？
- `Right_curve_line()` / `Left_curve_line()` 是否在对应丢线尚未出现时使用虚拟弯心？
- 大 Error 是否由中线先跳变产生，而非 PID 自己制造？

## Current Code Findings
- `junction_curve_hold_cnt < 1` 只等待一帧；当前版本对“上一普通弯刚退出、下一脚本拐点又出现”的抑制很弱。
- `MIN_JUNCTION_DIST=40` 只围绕 `Last_Valid_Junction_Length`（已接受的脚本拐点）工作；普通弯 `A=40/50→44` 退出时没有更新这个距离，因此第一普通 90 度弯不会给第二脚本弯提供间距保护。
- `Right_curve_line()` 一进入就从 `XM/2` 画到 `Deal_Right`，曲率 `-55`；`Left_curve_line()` 对称使用 `+55`。这是阶跃式强补线，符合视频中 Error 由 -8 直接跳到 +77。
- `ROAD_RIGHT_TURN` 在 `right_exit` 首次成立但 `turn_exit_cnt<2` 或 yaw<68° 时，仍调用完整的 `Right_curve_line()`；视频帧 82 的 `A=52/Error=75` 正对应此分支。
- `start_point_ok==0` 时无计数、无衰减、无上限地执行 `Error=Last_Error`；在 Error=75 的弯中丢线会持续强转。
- 当前 PID 为 `Kp=.75, Kp2=.029, Kd=.85, gyro=-.022`。Error 从 -8 跳到 +77 时，仅 P、非线性 P2 和 D 理论贡献约为 58、172、72（未计 gyro/限幅），因此会立刻撞到或接近转向限幅；但触发这个阶跃的是中线代码。
- 当前 `interrupt_turn` 还有两个风险：左右条件分别被重复写了一遍，并且在确认 `r_lost_num/l_lost_num>0` 之前访问 `r_center[r_lost_num-1]`/`l_center[l_lost_num-1]`。
- `TURN_LIMIT=228`，且 `motorL_Set=speed_base+speed_out`、`motorR_Set=speed_base-speed_out`，因此可由视频反推 `speed_out=(L-R)/2`。
- 反推结果：帧 74 约 -14；帧 78 约 +112；帧 80 约 +110；帧 82 约 +91；失线帧 85 仍约 +85；重见线帧 88 突然约 -219；帧 92 仍约 -172。
- 这说明 PID 的确在 Error 翻转后产生很大的反向修正（接近 -228 限幅），但“提前右转并盲转”的阶段已经由代码目标造成；调低 Kd 可减轻后续反抽，不能修复起因。
- `cpu1_main` 调用顺序为 `go() → Circuit_Control() → Turn_Yaw_Task()`，当前 yaw 使用本帧刚更新的 `Actual_Z`，不存在此前担心的整帧陈旧问题。

## Final Diagnosis
- 主因 1：第一普通右弯退出后没有给脚本拐点设置独立重装窗口，第二右拐几乎立即进入强补线。
- 主因 2：`A=52` 已识别出弯时仍继续输出完整右弯线，等待计数和 68° yaw。
- 主因 3：弯中丢起始点后无限继承大 `Last_Error`，使车辆在无视觉中继续强转。
- 次因：Error 恢复时从 +75 翻到 -41，`Kd=.85` 与 gyro 共同产生接近负限幅的反抽；这是后续摆动，不是提前入弯的起点。
- 优先级：代码策略 > PID。修完前三项后再评估 Kd；不要先整体降低 Kp2，否则第一个正常 90 度弯也会变弱。

## Issues
| Issue | Resolution |
|-------|------------|
| First channel-map search used nonexistent `code/cpu0_main.c` and a PowerShell-incompatible wildcard | Locate actual files with `rg --files`, then search the discovered paths |
| A later motor-formula search again included a PowerShell-incompatible `code/*.h` argument | Use directory globs (`-g`) only; do not pass wildcard paths |

## Resources
- C:\Users\丁子皓\OneDrive\Desktop\赛道图\94.mp4
- E:\TCB\2\Chezi\code\deal_image.c
- E:\TCB\2\Chezi\code\pid.c
- E:\TCB\2\Chezi\code\go.c

## Visual Findings
- 助手帧 74：紫色中线近似竖直，仅略偏左；通道可见 `debug_turn_cmd=255`、`A=8`、`debug_road_state=2`、`IF=1`、`Error=-8`。
- 助手帧 78：紫色中线突然变为大幅右弯；通道变为 `debug_turn_cmd=1`、`A=51`、`debug_road_state=1`、`IF=2`、`Error=77`。
- 约四个助手帧内，图像控制意图从接近直行跳为强右转，Error 从 -8 跳到 +77；这是中线/状态机先变化，PID 只是执行该大误差。
- 当前证据已经排除“仅靠调 PID 就能根治”：降低 PID 只会减弱错误中线的执行力度，也会同时削弱正常 90 度入弯能力。
- 助手帧 80：右弯紫线继续保持，`debug_road_state=1`、`Error=78`；控制仍处于接近满幅的右转误差。
- 助手帧 82：已经出现 `A=52`（右弯退出候选），但状态仍为 1、`IF=2`、`Error=75`，紫线仍由 `Right_curve_line()` 维持为大右弯。
- 因此还有第二层问题：图像已经满足右弯退出候选后，当前状态机仍至少继续一帧画右弯线，等待 `turn_exit_cnt` 与 yaw 门槛，延长了过转时间。
- 助手帧 85：`A=100`、`debug_road_state=1`、`IF=0`，紫色中线消失，但 `Error` 仍保持为 +75；这与 `start_point_ok==0` 时 `Error=Last_Error` 完全一致。
- 助手帧 88：重新得到中线时，赛道已落到画面左侧，状态回到 2，`Error` 反向跳为 -41。
- 关键因果链：提前进入右转线（-8→+77）→ 出口候选仍继续画右弯（+75）→ 大误差期间丢起始点仍继承 +75 → 继续盲目右转 → 恢复视觉时赛道已经在左侧（-41）。
- 因而当前问题主要是代码策略，不是 PID 参数本身；PID 只放大/执行了由状态机和失线继承提供的错误目标。
- 视频开头助手帧 57 为 `A=50`、`debug_turn_cmd=255`、`debug_road_state=2`、`IF=2`、`Error=77`：第一个 90 度弯由普通右弯状态处理，并非脚本拐点。
- 末尾助手帧 92 为 `debug_turn_cmd=2`、`A=61`、`debug_road_state=3`、`IF=1`、`Error=-54`；此时赛道/中线明显位于画面左侧，而且状态机已经接受到下一条脚本命令并进入直行通过态。
- 连续弯的特有风险是：第一普通弯刚退出后，第二脚本右拐立即把 Error 拉到 +77；中间没有足够的回正/稳定窗口。
