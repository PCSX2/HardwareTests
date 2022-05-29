# Progressive Field

## Description
Checks the behaviour of FIELD in the GS CSR register when the PCRT is set to progressive. 

## PCSX2 Behaviour
FIELD would alternate, just as if the PCRT was set to interlaced.

This caused some progressive games to bounce.

## Findings
When the video mode is set to progressive, FIELD is set.

## Related PR(s) or Issue(s)
https://github.com/PCSX2/pcsx2/pull/6256

## Expected Results

This 30 times:
```
FIELD Prev 2000 Field New 2000
```

In the case that the PCRT mode was set to interlaced, the expected result would be this, 15 times:

```
FIELD Prev 0 Field New 2000
FIELD Prev 2000 Field New 0
```
