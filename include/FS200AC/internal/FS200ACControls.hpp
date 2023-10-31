enum Control {
    NONE = 0,

    // knobs
    NAV1_COURSE_SELECTOR = 0x02,
    NAV2_OBS = 0x04,
    ADF_BRG = 0x06,
    BARO = 0x08,
    AUTOPILOT_HEADING = 0x0a,
    FREQ_TUNE_RADIO = 0x0c,

    // push buttons
    YOKE_BUTTON_DOWN = 0x46,
    YOKE_BUTTON_UP = 0x48,
    NAV1_USE_STBY = 0x10,
    NAV2_USE_STBY = 0x12,
    ADF_USE_STBY = 0x14,
    TIMER = 0x42,
    RMI = 0x44,

    // toggle buttons
    NAV1_ON = 0x18,
    NAV1_ID = 0x1a,
    NAV1_RAD = 0x1c,
    NAV2_ON = 0x1e,
    NAV2_ID = 0x20,
    NAV2_RAD = 0x22,
    ADF_ON = 0x24,
    ADF_ID = 0x26,
    DME_ON = 0x28,
    DME_NAV = 0x2a,
    AUTOPILOT_ON = 0x2c,
    AUTOPILOT_HDG = 0x2e,
    AUTOPILOT_ALT = 0x30,
    FUEL = 0x36,

    // switches
    GEAR = 0x32,
    FLAPS = 0x34,
    FREQ_SELECT_RADIO = 0x16,

    // sliders
    THROTTLE = 0x38,
    PROP_RPM = 0x3a,
    MIXTURE = 0x3c,
    COWL_FLAP = 0x4a,
    CARB_HEAT = 0x4c,

    // special
    TRIM_DN,
    TRIM_UP,
    AUTOPILOT_TRIM_DN,
    AUTOPILOT_TRIM_UP,
};

enum Flaps {
    Flaps_UP = 0,
    Flaps_APP,
    Flaps_LDG
};

enum Gear {
    Gear_DN = 0,
    Gear_UP
};

enum Radio {
    Radio_NAV1 = 1,
    Radio_NAV2,
    Radio_ADF,
    Radio_COM,
    Radio_XPNDR
};
