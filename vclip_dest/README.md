# VCLIP DEST

## Description
Tests the dest bit fields of the VU0 macro mode instruction VCLIP
The manual states that the first 3 are set, unconditionally reading x/y/z of fs

## Expected Results
It is expected that both clipping functions produce the same clip flag results

```
vclipw.xyz (Normal)
CLIP -> - + - + - +
        z z y y x x
        0 1 1 0 0 1

vclipw.xy
CLIP -> - + - + - +
        z z y y x x
        0 1 1 0 0 1
```
