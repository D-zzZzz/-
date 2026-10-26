# Task Plan: 94.mp4 连续两个 90 度弯过早转向分析

## Goal
结合 94.mp4 的逐帧图像、调试通道与当前 Chezi 代码，判断第二个连续 90 度弯的过早转向主要来自中线/状态机还是 PID，并给出最小调整顺序；不修改控制代码。

## Current Phase
Complete

## Phases

### Phase 1: 视频定位
- [x] 获取视频元数据并生成全片接触表
- [x] 定位第一弯退出、第二弯识别和中线跳左的帧段
- **Status:** complete

### Phase 2: 逐帧证据
- [x] 记录关键帧的边线、中线、A、road_state、IF、Error
- [x] 判断“先有错误中线”还是“中线合理但执行过猛”
- **Status:** complete

### Phase 3: 当前代码关联
- [x] 核对连续拐点、普通弯、抢断、里程和 yaw 条件
- [x] 核对 PID 参数及 Error 到 speed_out 的响应
- **Status:** complete

### Phase 4: 结论
- [x] 明确应优先改代码还是调 PID
- [x] 给出最小修改/调参顺序与验证通道
- **Status:** complete

## Final Decision
- 优先改代码：增加普通弯退出后的短重装门槛，限制无起始点时的大误差继承，并在 A=52 退出候选期间停止继续画完整右弯线。
- PID 暂不先改 Kp/Kp2；代码修正后若仍有 Error 翻转时的猛烈反抽，再将 Kd 从 0.85 小步降到约 0.65~0.70 验证。
- 录像与当前脚本表不一致，必须烧录当前 15:36 构建后复测再最终定参。

## Key Decision Rule
- 若紫色中线在车尚未进入第二弯时已经跳向目标侧：优先改识别/补线/状态切换。
- 若中线位置合理、Error 变化合理，但 speed_out 过冲：再调 PID/陀螺项。

## Errors Encountered
| Error | Attempt | Resolution |
|-------|---------|------------|
| `git diff --stat` failed because this workspace is not a Git repository | 1 | Preserve files directly and avoid Git-based assumptions |
