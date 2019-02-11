typedef union {
  uint8_t data;
  struct {
    uint8_t wifi_down : 1;
    uint8_t mqtt_down : 1;
    uint8_t spare02 : 1;
    uint8_t spare03 : 1;
    uint8_t spare04 : 1;
    uint8_t spare05 : 1;
    uint8_t spare06 : 1;
    uint8_t spare07 : 1;
  };
} StateBitfield;

extern uint32_t uptime;
extern uint8_t  ntp_force_sync;
extern StateBitfield global_state;
