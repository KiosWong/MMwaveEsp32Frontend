# idf_component_register(SRCS "bmd101.c" "esp32_mmwave_front.c" "adc1_example_main.c"
#                     INCLUDE_DIRS ".")
set(srcs    "adc.c"
            "esp32_mmwave_front.c"
            "wifi.c"
            "udp.c"
            "bmd101.c")
set(include_dirs ".")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}")