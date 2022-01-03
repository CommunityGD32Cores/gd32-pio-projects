# SPL CMSIS-DSP optimized example

## Description 

This example project shows how to take advantage of the CMSIS-DSP library that is available within the SPL framework. 

It performs the computation of `cos(a)`, `sin(a)`, `cos²(a)`, `sin²(a)` and `cos²(a) + sin²(a)` with a range of test values for `a` and compares them to difference to the ideal result (note `cos²(a) + sin²(a) = 1` for any `a` per pythagorean trigonometric identity).

In comparison to the regular SPL CMSIS-DSP example, this example uses:
* the CMSIS-DSP in source-format, not precompiled format. Enables size savings and better optimization
* a smaller printf() implementation (modified version of [`minimal-printf`](https://github.com/ARMmbed/mbed-os/tree/master/platform/source/minimal-printf))
* always hardware floating point support of available (Cortex-M4F, Cortex-M33)
* smaller floating-point code by activating `-ffast-math` optimization. (this is generally an unsafe optimization, but works in this case)

Code size for GD323350G-START (Cortex-M4F) with regular CMSIS-DSP example:
```
RAM:   [=         ]   6.8% (used 556 bytes from 8192 bytes)
Flash: [===       ]  32.2% (used 21104 bytes from 65536 bytes)
```
Code size in this optimized example:

```
RAM:   [          ]   2.3% (used 192 bytes from 8192 bytes)
Flash: [==        ]  16.3% (used 10656 bytes from 65536 bytes)
```

That is only 50% of the previous Flash usage and only 34% of the previous RAM usage!! For the exact same output.

## Technicalities

The libary declares the usage of the CMSIS-DSP library by `#include "arm_math.h"`. This triggers PlatformIO to try and find the library that supplies this header, and will find it in the precompiled [CMSIS-DSP library](https://github.com/CommunityGD32Cores/gd32-pio-spl-package/tree/main/gd32/cmsis/libraries/cmsis_dsp_from_source)).

Further, the `platformio.ini`, for Cortex-M33 targets, makes the compilation use `-mfloat-abi=hard` by activating `board_build.cm33_hardfloat = yes`. `board_build.cm4_hardfloat = yes` is similiary set for Cortex-M4F targets.

Further, to be able to use `printf()` with the `%f` floating point specifier, the project declares `board_build.use_minimal_printf = yes`, which instructs the SPL builder script to activate compilation flags (`"-Wl,--wrap,printf"` and friends) that allow the redefinition of `printf()` functions. The source code in `src/minimal-printf` then provides these functions.

The output is transported in a configurable manner to the developer, defined via `printf_over_x.c` and the activated macros. In the standard case, the UART is used, with two possible pin maps. This technique is exactly the same as in [gd32-spl-usart](../gd32-spl-usart). 

## Expected Output

Tested on a GD32350G-START board, output via the standard UART on the alternative pin mapping TX=PB6. A USB-UART adapter had to be connected to this pin, since the board has no built-in USB-UART converter.

The output after executing the "Upload and Monitor" task should be:

```
ARM cos and sin example start!
Diff from reference output was: 0.000037 for input -1.244917, cos = 0.320136, sin = -0.947352
Diff from reference output was: 0.000036 for input -4.793534, cos = 0.081054, sin = 0.996692
Diff from reference output was: 0.000036 for input 0.360705, cos = 0.935631, sin = 0.352928
Diff from reference output was: 0.000037 for input 0.827930, cos = 0.676389, sin = 0.736519
Diff from reference output was: 0.000017 for input -3.299532, cos = -0.987545, sin = 0.157282
Diff from reference output was: 0.000031 for input 3.427442, cos = -0.959408, sin = -0.281968
Diff from reference output was: 0.000016 for input 3.422402, cos = -0.960824, sin = -0.277131
Diff from reference output was: 0.000022 for input -0.108308, cos = 0.994130, sin = -0.108095
Diff from reference output was: 0.000028 for input 0.941944, cos = 0.588209, sin = 0.808692
Diff from reference output was: 0.000006 for input 0.502610, cos = 0.876326, sin = 0.481712
Diff from reference output was: 0.000025 for input -0.537345, cos = 0.859060, sin = -0.511851
Diff from reference output was: 0.000025 for input 2.088817, cos = -0.495156, sin = 0.868790
Diff from reference output was: 0.000004 for input -1.693169, cos = -0.122067, sin = -0.992520
Diff from reference output was: 0.000000 for input 6.283185, cos = 1.000000, sin = 0.000000
Diff from reference output was: 0.000002 for input -0.392546, cos = 0.923937, sin = -0.382542
Diff from reference output was: 0.000030 for input 0.327893, cos = 0.946709, sin = 0.322044
Diff from reference output was: 0.000022 for input 3.070148, cos = -0.997438, sin = 0.071384
Diff from reference output was: 0.000013 for input 0.170611, cos = 0.985475, sin = 0.169784
Diff from reference output was: 0.000037 for input -0.275275, cos = 0.962333, sin = -0.271807
Diff from reference output was: 0.000024 for input -2.395493, cos = -0.734333, sin = -0.678772
Diff from reference output was: 0.000006 for input 0.847311, cos = 0.661999, sin = 0.749501
Diff from reference output was: 0.000035 for input -3.845517, cos = -0.762295, sin = 0.647203
Diff from reference output was: 0.000038 for input 2.055818, cos = -0.466219, sin = 0.884648
Diff from reference output was: 0.000028 for input 4.672594, cos = -0.039784, sin = -0.999194
Diff from reference output was: 0.000027 for input -1.990923, cos = -0.407870, sin = -0.913025
Diff from reference output was: 0.000025 for input 2.469305, cos = -0.782389, sin = 0.622769
Diff from reference output was: 0.000012 for input 3.609003, cos = -0.892733, sin = -0.450573
Diff from reference output was: 0.000027 for input -4.586737, cos = -0.125320, sin = 0.992103
Diff from reference output was: 0.000009 for input -4.147080, cos = -0.535675, sin = 0.844419
Diff from reference output was: 0.000008 for input 1.643757, cos = -0.072895, sin = 0.997336
Diff from reference output was: 0.000026 for input -1.150866, cos = 0.407691, sin = -0.913106
Diff from reference output was: 0.000022 for input 1.985805, cos = -0.403193, sin = 0.915103
ARM cos() and sin() tests -> SUCCESS
```