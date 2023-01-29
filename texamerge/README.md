# TEXAMERGE

## Description
Uploads a 16 bit texture directly to a 16 bit framebuffer

Every 50 vsyncs swap `TEXA` `TA0` and `TA1` values between 0x80 and 0

This tests if the merge circuit behaviour is influenced by the `TA0` and `TA1` fields in the TEXA register

## PCSX2 Behaviour
Software modes display output emulation would use the values of `TEXA`, the test image on would blink because of this

Hardware modes display output did not use TEXA
The test image on screen would be constant (accurate to console)

## Related PR(s) or Issue(s)
https://github.com/PCSX2/pcsx2/pull/7991

## Expected Results
On a PS2, the image will not blink, indicating that the display output circuit does not use TEXA
