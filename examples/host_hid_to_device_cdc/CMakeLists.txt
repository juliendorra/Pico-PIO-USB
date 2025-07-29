set(target_name host_hid_to_device_cdc)
add_executable(${target_name})

target_sources(${target_name} PRIVATE
 host_hid_to_device_cdc.c
 usb_descriptors.c
 log_queue.c
 # can use 'tinyusb_pico_pio_usb' library later when pico-sdk is updated
 ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c
 ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
 )

 # print memory usage, enable all warnings
target_link_options(${target_name} PRIVATE -Xlinker --print-memory-usage)
target_compile_options(${target_name} PRIVATE -Wall -Wextra)

# use tinyusb implementation + set TinyUSB debug level per build config
target_compile_definitions(${target_name} PRIVATE
  PIO_USB_USE_TINYUSB
  # TinyUSB debug verbosity by configuration:
  # - Debug:      2  (warnings+errors)
  # - Release:    0  (off)
  # - RelWithDebInfo: 1 (errors)  [optional, nice default]
  # - MinSizeRel: 0  (off)
  $<$<CONFIG:Debug>:CFG_TUSB_DEBUG=2>
  $<$<CONFIG:RelWithDebInfo>:CFG_TUSB_DEBUG=1>
  $<$<CONFIG:Release>:CFG_TUSB_DEBUG=0>
  $<$<CONFIG:MinSizeRel>:CFG_TUSB_DEBUG=0>
)

# needed so tinyusb can find tusb_config.h
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${target_name} PRIVATE pico_stdlib pico_pio_usb tinyusb_device tinyusb_host)
pico_add_extra_outputs(${target_name})

