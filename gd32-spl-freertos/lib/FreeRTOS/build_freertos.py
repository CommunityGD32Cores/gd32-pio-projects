from os.path import join, realpath
Import('env')
global_env = DefaultEnvironment()

board = env.BoardConfig()
cpu = board.get("build.cpu", "")

port_to_use = None
cpu_to_port_map = {
    "cortex-m3": "ARM_CM3",
    "cortex-m4": "ARM_CM4F", # all GD32 Cortex-M4 based MCUs have a FPU
    "cortex-m23": join("ARM_CM23_NTZ", "non_secure"), # no trust zone 
    "cortex-m33": join("ARM_CM33_NTZ", "non_secure"), # no trust zone 
    # the MPU (memory protection unit) and trust zone port versions are unused 
}
if cpu in cpu_to_port_map.keys():
    port_to_use = cpu_to_port_map[cpu]
else:
    print("ERROR: FreeRTOS library can't determine which port to use!")

# check if we should include the CMSIS-OS2 compatibility layer
cpp_defines = env.Flatten(env.get("CPPDEFINES", []))
include_cmsisos2 = "PIO_FREERTOS_WITH_CMSISOS2" in cpp_defines
print("Included CMSIS-S2 layer: " + str(include_cmsisos2))

# build one source filter expression
src_filter_default = [
    "+<*>",
    "-<%s>" %join("portable", "*"), # exclude the entire "portable" folder default
    "+<%s>" % join("portable", port_to_use),
    "-<mpu_wrappers.c>" # we never compile the port with MPU support so we also shouldn't compile that file.
    "%s<cmsis_os2>" % ("+" if include_cmsisos2 else "-")
]

include_parts = [
    join("src", "portable", port_to_use)
]

if include_cmsisos2:
    include_parts += [join("src","cmsis_os2")]

env.Replace(SRC_FILTER=src_filter_default)

for inc in include_parts:
    env.Append(CPPPATH=[realpath(inc)])
    global_env.Append(CPPPATH=[realpath(inc)])

# for Cortex-M4F, we need the floating point compiler flags
if cpu == "cortex-m4":
    env.Append(
        CCFLAGS=["-mfpu=fpv4-sp-d16", "-mfloat-abi=softfp"],
        LINKFLAGS=["-mfpu=fpv4-sp-d16", "-mfloat-abi=softfp"],
    )
# fixup assembly errors by assuming thumb2..
if cpu in ["cortex-m23", "cortex-m33"]:
    env.Append(
        CCFLAGS=["-masm-syntax-unified"]
    )
if cpu == "cortex-m33":
    env.Append(
        CCFLAGS=["-mfpu=fp-armv8", "-mfloat-abi=softfp"],
        LINKFLAGS=["-mfpu=fp-armv8", "-mfloat-abi=softfp"],
    )
