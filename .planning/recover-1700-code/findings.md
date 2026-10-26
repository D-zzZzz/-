# Findings: recover code from around 17:00

## Confirmed
- Target window is interpreted as today, 2026-07-13 17:00-17:59.
- Current files must remain untouched until a candidate snapshot is validated.
- Existing root planning files belong to earlier video analysis and were not overwritten.
- Earlier observations showed a successful `Debug\\Chezi.elf` around 17:29, but this is only a build artifact, not yet proof of a complete source snapshot.
- Initial `code`/`user` inventory found two modified source files, but a full-project inventory also found `libraries\\zf_device\\zf_device_mt9v03x.h` modified at 17:42:54.
- `.cproject` and the current ELF/HEX/MAP were regenerated around 19:08.
- The project `.git` path is an empty directory, so there is no usable Git history here.

## Candidates
- The 17:29 recovery target requires checking three files: `deal_image.c`, `cpu0_main.c`, and the camera header `zf_device_mt9v03x.h`. Because the header was modified at 17:42, its latest pre-17:29 history entry is needed.
- Current camera header uses 188x120, exposure 138, 90 FPS, LR/UD offsets 1, and gain 32; its exact pre-17:29 values still need extraction.
- The first header-guard fingerprint produced no history hit, so the next lookup must use a stable camera-configuration macro instead.
- Stable-macro search found the camera header history. The latest snapshot before 17:29 is 15:52:48 and has `MT9V03X_EXP_TIME_DEF` 142.
- The header changed from exposure 142 to 144 around 17:40, then to the current 138 at 17:42; no other header difference appears in those comparisons.
- AURIX/Eclipse local history remains the best source for exact snapshots.
- AURIX configuration confirms the active/recent workspace is `E:\\TCB\\2`.
- The matching Eclipse metadata directory exists at `E:\\TCB\\2\\.metadata`, so local-history recovery is available for direct inspection.
- Local history contains multiple full `deal_image.c` snapshots during 17:xx: 17:03:58, 17:08:43, 17:28:02, 17:29:09, 17:35:40, 17:40:04, 17:42:54, and 17:57:41.
- The 17:29:09 `deal_image.c` snapshot is especially valuable because earlier build evidence showed an ELF produced at about 17:29:35, making it the closest source/build-matched candidate.
- `cpu0_main.c` has local-history snapshots at 16:53:11 and 16:53:28 before the target window, then 18:30:13 after it. Therefore the 16:53 version is the last known version at 17:29 unless another mapping contradicts it.
- Content inspection confirms both 16:53 entries are `cpu0_main.c`; the later 16:53:28 entry is the correct last pre-17:29 candidate.
- Current `cpu0_main.c` visibly changed `negative_pressure_set_us(1600)` from the 16:53 candidate to `1500`.
- The 17:29 `deal_image.c` candidate differs from current by 49 inserted and 21 deleted lines; the 17:57 candidate differs by 36 inserted and 21 deleted lines. These are real source versions, not empty or metadata files.
- Exact diff shows current `deal_image.c` changed several behavior-critical items after 17:29: the active `Turn_Command` array, `far_Threshold` from 0 to 5, turn-hold reset placement, turn-exit confirmation from 2 frames to 1, `lvbo_diode_row_white()`, and normal-curve start thresholds from 45 to 65.
- Exact diff shows current `cpu0_main.c` changed suction from 1600 to 1500 and switched oscilloscope channels from turn diagnostics to motor/speed diagnostics.
- No separate same-day project backup copy was found under `E:\\TCB`; Eclipse local history is the authoritative recovery source.

## Recovery decision
- Compare and validate the 17:29 source pair first; retain the other 17:xx `deal_image.c` versions as alternatives.
- The complete 17:29 source tree can be reconstructed after adding the latest pre-17:29 `zf_device_mt9v03x.h` snapshot to the already identified `deal_image.c` and `cpu0_main.c` pair.
- The 17:29 recovery set is now complete: `deal_image.c` at 17:29:09, `cpu0_main.c` at 16:53:28, and `zf_device_mt9v03x.h` at 15:52:48 (exposure 142).
- Candidate and current versions were copied to `.recovery\\2026-07-13_1729` using non-compilable `.snapshot` suffixes.
- SHA-256 verification passed for all six copies; every saved copy is byte-identical to its source.
- Active project source has not been overwritten.
- The user explicitly authorized restoring the active project to the validated 17:29 set and requested an exact change report.
- All three active files now match the validated target snapshots byte-for-byte: `deal_image.c` SHA-256 `4E8891...2E1`, `cpu0_main.c` `5A7852...55C`, and `zf_device_mt9v03x.h` `C2BCA3...611`.
- Active source timestamps were refreshed after copying so AURIX incremental build will recompile them instead of treating the 19:08 objects as newer.
- Restored key locations: scripted route line 7, far threshold line 70, turn-hold reset line 3053, turn-exit gates lines 3140/3170, row-white filter lines 3645-3671, normal-curve thresholds lines 3681/3707, suction line 43 in `cpu0_main.c`, debug channels lines 107-114, and camera exposure line 89.
- The command-line builder reached compilation of both changed C files but stopped only on Tasking standalone-license error F104; no compiler syntax error was emitted before the license stop.
