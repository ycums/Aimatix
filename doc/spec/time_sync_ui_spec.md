# Time Sync UI Spec (SoftAP + QR)

## Titles (use `>` as separator)
- Step1: `TIME SYNC > JOIN AP`
- Step2: `TIME SYNC > OPEN URL`
- Success: `TIME SYNC > DONE` (no fixed screen; for logs)
- Error: `TIME SYNC > ERROR`

## Button Hints (English)
- A: `REISSUE`
- B: (unused)
- C: `EXIT`

## Transitions
- Success: Stop SoftAP → immediately go to `MAIN_DISPLAY`
- Error: Stop SoftAP → show error message for 2s → go to `SETTINGS_MENU`
- User Exit: C (EXIT) stops SoftAP → `SETTINGS_MENU`

## Error Messages (HTTP reasons)
- Window expired: `AP WINDOW EXPIRED`
- Token mismatch: `TOKEN MISMATCH`
- Apply failed: `APPLY FAILED`
- Validation: `TIME OUT OF RANGE` / `TZ OFFSET OUT OF RANGE` / `BAD REQUEST`

## Timing
- Tick rate: ~20Hz (`TimeSyncDisplayState.onDraw()` calls `ITimeSyncController.loopTick()`)
- Debounce policy: Button=50ms / Screen=200ms / Transition=300ms

## Notes
- Success Local/TZ: Serial log only
- Boot auto: if EXIT was used during auto time sync, suppress further auto attempts until next boot

