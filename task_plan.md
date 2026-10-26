# Task Plan: 93.mp4 最后一个弯未回正分析

## Goal
结合 93.mp4 的最后一弯画面、调试通道与当前 Chezi 源码，定位未回正的直接分支和根因，并给出最小修正建议。

## Current Phase
Phase 1

## Phases

### Phase 1: 视频与环境发现
- [ ] 获取视频元数据和关键时间段
- [ ] 提取全片接触表并定位最后一弯
- **Status:** in_progress

### Phase 2: 逐帧证据
- [ ] 密集提取最后一弯前后帧
- [ ] 记录边线、中线、帧号和通道变化
- **Status:** pending

### Phase 3: 代码关联
- [ ] 对照当前 go/deal_image/pid 状态机
- [ ] 确定未回正的实际条件和次要风险
- **Status:** pending

### Phase 4: 交付
- [ ] 给出证据链、具体代码位置和最小修改建议
- **Status:** pending

## Key Questions
1. 最后一弯后 `A`、`debug_road_state`、`IF`、`Error` 如何变化？
2. 是转弯状态未退出、中线仍偏、误差失效继承，还是 PID/陀螺项阻止回正？

## Decisions Made
| Decision | Rationale |
|----------|-----------|
| 先全片定位，再对最后弯密集取帧 | 避免只看单帧误判状态转换 |

## Errors Encountered
| Error | Attempt | Resolution |
|-------|---------|------------|
| Bundled Python lacks `cv2` while probing metadata | 1 | Use the extraction script's alternate backend or system ffmpeg/ffprobe |
| Skill extractor failed during crop generation because `cv2` is missing | 2 | Reuse extracted full frames and switch crop/contact-sheet work to ffmpeg |
