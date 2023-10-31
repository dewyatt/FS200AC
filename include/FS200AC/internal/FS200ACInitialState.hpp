const FS200AC::ConsoleState FS200AC::DEFAULT_INITIAL_STATE = {
    // current radio (for FREQ knob)
    .current_radio = 1, // NAV1
    // NAV1 standby freq [10800, 11795]
    .nav1_standby_freq = {111, 70}, // 111.70 MHz
    // NAV1 current freq [10800, 11795]
    .nav1_current_freq = {109, 50}, // 109.50 MHz
    // NAV2 standby freq [10800, 11795]
    .nav2_standby_freq = {111, 70}, // 111.70 MHz
    // NAV2 current freq [10800, 11795]
    .nav2_current_freq = {109, 50}, // 109.50 MHz
    // ADF standby freq [190, 1799] KHz
    .adf_standby_freq = {19, 0}, // 190 KHz
    // ADF current freq [190, 1799]
    .adf_current_freq = {20, 0}, // 200 KHz
    // unknown
    .unknown_freq = {118, 90}, // 118.90
    // COM freq [11800, 13597]
    .com_freq = {121, 20}, // 121.20 MHz

    // NAV1 course selector [0, 359] deg
    .nav1_course_selector = 180,
    // NAV2 OBS [0, 359] deg
    .nav2_obs = 180,
    // ADF BRG [0, 359] deg
    .adf_brg = 180,
    // BARO [2780, 3100] inHg*100
    .baro = 2992,
    // AUTOPILOT HDG [0, 359] deg
    .autopilot_hdg = 180,

    .unknown = {1, 2, 0, 0}
};

