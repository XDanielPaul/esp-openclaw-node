set(esp_openclaw_node_component_args
    SRCS
        "src/esp_openclaw_node_identity.c"
        "src/esp_openclaw_node_persisted_session.c"
        "src/esp_openclaw_node.c"
        "src/esp_openclaw_node_connect_source.c"
        "src/esp_openclaw_node_registry.c"
        "src/esp_openclaw_node_protocol.c"
        "src/esp_openclaw_node_transport.c"
        "src/esp_openclaw_node_runtime.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "private_include"
    REQUIRES
        espressif__cjson
        esp_app_format
        esp_websocket_client
        libsodium
        mbedtls
        nvs_flash
)

idf_component_register(${esp_openclaw_node_component_args})
