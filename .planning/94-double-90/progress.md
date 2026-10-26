# Progress Log: 94.mp4 连续两个 90 度弯

## Session: 2026-07-13

### Phase 1: 视频定位
- **Status:** in_progress
- Actions taken:
  - 读取智能车代码、逐飞视频和文件化计划技能。
  - 检查上一任务记录；为 94.mp4 建立独立作用域计划，保留 93.mp4 记录不动。
  - 确认 94.mp4 元数据：0.704 s、22 帧、2558×1492。
  - 检查 CodeGraph 状态：项目尚未初始化，继续用只读源码检查。
  - 用 ffmpeg 提取全部 22 帧并生成全片接触表。
  - 核对助手帧 74 与 78：状态从直行侧跳到右转状态，Error 从 -8 跳到 +77。
  - 核对助手帧 80 与 82：Error 保持 78/75；帧 82 已进入 `A=52` 退出候选但仍输出右弯线。
  - 核对助手帧 85 与 88：丢起始点时仍继承 +75；重新见线时赛道已到左侧，Error 翻为 -41。
  - 比较帧 57 与 92：第一弯为普通右弯，末帧已进入下一脚本直行通过态且 Error=-54。
  - 核对源码/固件时间：录像后 `pid.c`、`deal_image.c` 与固件均有更新；当前脚本表与录像固件不一致。
  - 定位实际主核文件并确认当前 11 路调试通道映射；视频未发送 speed_out/yaw。
  - 核对当前状态机、补线和 PID：确认一帧等待、普通弯无脚本间距保护、退出候选仍画完整弯线及无界 Last_Error 继承。
  - 由左右轮目标值反推 speed_out：提前右转约 +112，失线仍 +85，恢复后反抽至约 -219。
- Files created:
  - .planning/94-double-90/task_plan.md
  - .planning/94-double-90/findings.md
  - .planning/94-double-90/progress.md

### Completion
- **Status:** complete
- Final result:
  - 判定为代码时序/失线策略主导，PID 反抽为次要现象。
  - 给出代码修正优先级和 PID 二次微调范围。
  - 未修改任何控制源码。

## Error Log
| Error | Attempt | Resolution |
|-------|---------|------------|
| Workspace is not a Git repository, so `git diff --stat` failed | 1 | Continue with direct read-only source inspection |
| Channel-map search referenced nonexistent `code/cpu0_main.c` and an invalid wildcard | 2 | Discover actual main-file paths before the next search |
| Motor-formula search again passed an invalid wildcard path | 3 | Stop passing wildcard paths; use `rg -g` filters only |
