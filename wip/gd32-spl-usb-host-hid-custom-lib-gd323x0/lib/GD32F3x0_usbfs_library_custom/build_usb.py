from os.path import join, realpath
Import("env")

# map flags to their class name (which we can deduct the path from)
device_flags_to_sources = {
    "PIO_USBFS_DEVICE_AUDIO": "audio",
    "PIO_USBFS_DEVICE_CDC": "cdc",
    "PIO_USBFS_DEVICE_DFU": "dfu",
    "PIO_USBFS_DEVICE_HID_STANDARD": "hid",
    "PIO_USBFS_DEVICE_HID_CUSTOM": "hid",
    "PIO_USBFS_DEVICE_IAP": "iap",
    "PIO_USBFS_DEVICE_MSC": "msc",
    "PIO_USBFS_DEVICE_PRINTER": "printer"
}

device_flags_to_special_excludes = {
    "PIO_USBFS_DEVICE_HID_STANDARD": join("device", "class", "hid", "Source", "custom_hid_core.c"),
    "PIO_USBFS_DEVICE_HID_CUSTOM": join("device", "class", "hid", "Source", "standard_hid_core.c"),
}

host_driver_files = [
    join("driver", "Source", "drv_usb_host.c"),
    join("driver", "Source", "drv_usbh_int.c")
]

device_driver_files = [
    join("driver", "Source", "drv_usb_dev.c"),
    join("driver", "Source", "drv_usbd_int.c")
]

host_flags_to_sources = {
    "PIO_USBFS_HOST_HID": "hid",
    "PIO_USBFS_HOST_MSC": "msc"
}

include_parts = []
source_parts = []
include_device_core = False
include_host_core = False
special_excludes = []

# source through CPPDEFINES for config flags
for item in env.Flatten(env.get("CPPDEFINES", [])):
    item = str(item)
    #print("Got item %s" % str(item))
    if item in device_flags_to_sources.keys():
        include_device_core = True
        class_name = device_flags_to_sources[item]
        print("USBFS option %s found, adding class \"%s\"" %
              (str(item), class_name))
        include_parts.append(join("device", "class", class_name, "Include"))
        source_parts.append(join("device", "class", class_name, "Source"))
        if item in device_flags_to_special_excludes.keys():
            special_excludes.append(device_flags_to_special_excludes[item])
    if item in host_flags_to_sources.keys():
        include_host_core = True
        class_name = host_flags_to_sources[item]
        include_parts.append(join("host", "class", class_name, "Include"))
        source_parts.append(join("host", "class", class_name, "Source"))

if include_device_core:
    include_parts.append(join("device", "core", "Include"))
    source_parts.append(join("device", "core", "Source"))

if include_host_core:
    include_parts.append(join("host", "core", "Include"))
    source_parts.append(join("host", "core", "Source"))

if include_device_core and not include_host_core:
    # prevent host files from building (they will not find usbh_conf.h)
    special_excludes.extend(host_driver_files)

if not include_device_core and include_host_core:
    # prevent devicefiles from building (they will not find usbd_conf.h)
    special_excludes.extend(device_driver_files)

# global build environment is for all libraries and code
global_env = DefaultEnvironment()

for inc in include_parts:
    env.Append(CPPPATH=[realpath(inc)])
    global_env.Append(CPPPATH=[realpath(inc)])

# build one source filter expressin
src_filter_default = [
    "-<*>", # exclude everything by default
    "+<%s*>" % join("driver", "Source"), # include the entire driver code (host + device), special exclusions apply.
]

for src in source_parts:
    src_filter_default.append(
        "+<%s*>" % src
    )

for excl in special_excludes:
    src_filter_default.append(
        "-<%s>" % excl
    )

print("USBFS source filter excludes: %s" % str(src_filter_default))

env.Replace(SRC_FILTER=src_filter_default)

# needed for examples to work. Otherwise weird hangs / USB resets were experienced
global_env.Append(CPPDEFINES=["USE_USB_FS"])
env.Append(CPPDEFINES=["USE_USB_FS"])
