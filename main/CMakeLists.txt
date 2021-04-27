# idf_component_register(SRCS "adc1_example_main.c"
#                     INCLUDE_DIRS ".")
set(srcs    "adc.c"
            "breath.c"
            "wifi.c"
            "udp.c")
set(include_dirs ".")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}")