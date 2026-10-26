# Task Plan: recover code from around 17:00

## Goal
Locate the closest recoverable source snapshot from 2026-07-13 17:00-17:59, preserve the current project, validate the candidate, and restore only after the recovery set is understood.

## Safety rules
- Do not overwrite current source during discovery.
- Prefer IDE local history and source backups over reverse engineering build artifacts.
- Back up the current changed files before any restoration.
- Restore only files proven to differ; do not roll back the whole tree blindly.

## Phases

### Phase 1: inventory current state
- [x] List source and project files modified since 17:00.
- [x] Record current build artifact timestamps.
- [x] Inspect whether `.git` contains usable history.

### Phase 2: locate snapshots
- [x] Find AURIX/Eclipse workspace and local history.
- [x] Search project/parent/temp locations for source copies and backups.
- [x] Collect candidates around 17:00-17:59 without modifying source.

### Phase 3: validate candidates
- [x] Match candidate paths, timestamps, and file contents.
- [x] Compare candidates against current source.
- [x] Identify the smallest complete recovery set.

### Phase 4: safe recovery
- [x] Copy current affected files to a timestamped backup.
- [x] Restore the validated snapshot after user authorization.
- [x] Verify project consistency and buildability as far as the local toolchain allows.

## Current phase
Complete. Active source matches the validated 17:29 recovery set; IDE build/flash remains a user-side step because Tasking blocks standalone builds.

## Errors encountered

| Error | Attempt | Resolution |
|---|---:|---|
| `git diff --stat` reported that the project is not a Git repository | 1 | Use the already verified snapshot hashes and `git diff --no-index` for file comparisons. |
| Patched `deal_image.c` had no text diff but did not yet match target SHA-256 | 1 | Byte inspection found 44 CR bytes normalized by patch insertion; exact validated snapshot copies were then used, and all target hashes matched. |
| Command-line Tasking build failed with `ctc F104: License does not support running as standalone` | 1 | This is an IDE-license restriction, not a source diagnostic; build the restored files from AURIX Development Studio. |
