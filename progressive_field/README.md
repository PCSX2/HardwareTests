# Progressive Field

## Description
Checks the behaviour of FIELD(in CSR) when the CMOD(in SMODE1)
and VFP(in SYNCHV) bits are changed.

## PCSX2 Behaviour
PCSX2 would swap FIELD unconditionally, this caused some progressive games to
bounce.

After the original progressive hardware test, PCSX2 would look at the interlace bit of SMODE2.
If the bit was not set (progressive), PCSX2 would set FIELD, otherwise it would swap.

This was later found to be incorrect behaviour.

## Findings
The original finding determined that when the video mode is set to progressive, FIELD is set.


Turns out, the SMODE2 interlace bit has no control over FIELD behaviour.

The actual behaviour is determined by CMOD (bits 13 & 14 in SMODE1)
and VFP (bottom bits in SYNCV)

![img](https://i.imgur.com/dsFYHm5.jpg)

## Related PR(s) or Issue(s)
https://github.com/PCSX2/pcsx2/pull/6256

https://github.com/PCSX2/pcsx2/pull/6342
## Expected Results

When testing CMOD 1, odd VFPs should say `FIELD IS CHANGING`

When testing CMOD 0, no matter the VFP, it should say `FIELD IS NOT CHANGING`
