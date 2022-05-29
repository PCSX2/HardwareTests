# VIFCMD 12

## Description
Checks the behaviour of an undefined VIF command (0x12)

This command is intriguing because it's CMD byte happens to be sandwiched between defined VIF FLUSH[X] CMD bytes

    FLUSHE -> 0x10
    FLUSH  -> 0x11
    ???    -> 0x12
    FLUSHA -> 0x13

The procedure for checking this behaviour is to upload a micro program to VU1 that loops indefinitely. Then see if the VIF will wait for the micro programs end.

## PCSX2 Behaviour
When coming across this command, PCSX2 will set `ER1` in `VIFSTAT` and emit the following message:

`Vif1: Unknown VifCmd! [12]`

This would happen to be the correct behaviour.
## Findings
When the VIF executes an instruction with CMD 0x12, it will _not_ wait for an E bit (end of micro program).

`ER1` in `VIFSTAT` will be set.

## Related PR(s) or Issue(s)
https://github.com/PCSX2/pcsx2/issues/4179

## Expected Results
(VIF1STAT ER1 = 2000 -> ER1 bit is set)
```
VIF1STAT ER1 = 0
VIF1 finished, VIF1STAT ER1 = 2000
```

In the case that you replace the 0x12 CMD with a proper FLUSH command, the result would be expected to be:

```
VIF1STAT ER1 = 0
waited 10 mil times (VIF1STAT ER1 = 0)
waited 20 mil times (VIF1STAT ER1 = 0)
waited 30 mil times (VIF1STAT ER1 = 0)
waited 40 mil times (VIF1STAT ER1 = 0)
...
```

In this case, we are waiting on the VIF to finish waiting on the micro program, which will complete during your next power outage. :^)