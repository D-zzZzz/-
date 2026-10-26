# Chezi recovery snapshot: 2026-07-13 17:29

This folder preserves the source state closest to the known 17:29 IDE build and a byte-for-byte backup of the current versions. Files use the `.snapshot` suffix so the compiler will not treat them as project source.

## Candidate recovery set

- `candidate/deal_image.c.172909.snapshot` -> `code/deal_image.c`
  - Source time: 17:29:09
  - SHA-256: `4E889180A771F79801F76C07778FA5F1D0A1C716E62A673C3AAA7CCD197EF2E1`
- `candidate/cpu0_main.c.165328.snapshot` -> `user/cpu0_main.c`
  - Source time: 16:53:28; latest snapshot before 17:29
  - SHA-256: `5A7852FECFA57B9497412C48BD7BC4D400BB0997435D96772D9D8339025DA55C`
- `candidate/zf_device_mt9v03x.h.155248.snapshot` -> `libraries/zf_device/zf_device_mt9v03x.h`
  - Source time: 15:52:48; latest snapshot before 17:29
  - Camera exposure value: 142
  - SHA-256: `C2BCA3F5D91B192060E8588735B765EB0876B07BFFF972288F3D09C5F5B45611`

## Current backup

- `current-backup/deal_image.c.current.snapshot`
- `current-backup/cpu0_main.c.current.snapshot`
- `current-backup/zf_device_mt9v03x.h.current.snapshot`

All six copies were SHA-256 verified against their original files. The active project source was not changed while creating this recovery package.

## Restoration status

Applied to the active project on 2026-07-13 at 19:27. All three active files matched the candidate SHA-256 values afterward. Source timestamps were refreshed to force an IDE rebuild. The existing `Debug/Chezi.elf`, `.hex`, and `.map` are still the 19:08 artifacts and must be regenerated in AURIX Development Studio before flashing.
