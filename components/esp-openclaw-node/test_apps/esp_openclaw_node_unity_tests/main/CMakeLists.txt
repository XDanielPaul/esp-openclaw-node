idf_component_register(
    SRCS "test_esp_openclaw_node.c"
    INCLUDE_DIRS "."
    REQUIRES
        mbedtls
        nvs_flash
        esp-openclaw-node
        unity
)

idf_component_get_property(esp_openclaw_node_dir esp-openclaw-node COMPONENT_DIR)
target_include_directories(${COMPONENT_LIB} PRIVATE "${esp_openclaw_node_dir}/private_include")
