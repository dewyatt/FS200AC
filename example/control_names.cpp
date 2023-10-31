#include <FS200AC/FS200AC.hpp>

const char *control_name(FS200AC::Control control) {
    switch (control) {
        case FS200AC::NONE: return "NONE";

        case FS200AC::NAV1_COURSE_SELECTOR: return "NAV1_COURSE_SELECTOR";
        case FS200AC::NAV2_OBS: return "NAV2_OBS";
        case FS200AC::ADF_BRG: return "ADF_BRG";
        case FS200AC::BARO: return "BARO";
        case FS200AC::AUTOPILOT_HEADING: return "AUTOPILOT_HEADING";
        case FS200AC::FREQ_TUNE_RADIO: return "FREQ_TUNE_RADIO";

        case FS200AC::YOKE_BUTTON_DOWN: return "YOKE_BUTTON_DOWN";
        case FS200AC::YOKE_BUTTON_UP: return "YOKE_BUTTON_UP";
        case FS200AC::NAV1_USE_STBY: return "NAV1_USE_STBY";
        case FS200AC::NAV2_USE_STBY: return "NAV2_USE_STBY";
        case FS200AC::ADF_USE_STBY: return "ADF_USE_STBY";
        case FS200AC::TIMER: return "TIMER";
        case FS200AC::RMI: return "RMI";

        case FS200AC::NAV1_ON: return "NAV1_ON";
        case FS200AC::NAV1_ID: return "NAV1_ID";
        case FS200AC::NAV1_RAD: return "NAV1_RAD";
        case FS200AC::NAV2_ON: return "NAV2_ON";
        case FS200AC::NAV2_ID: return "NAV2_ID";
        case FS200AC::NAV2_RAD: return "NAV2_RAD";
        case FS200AC::ADF_ON: return "ADF_ON";
        case FS200AC::ADF_ID: return "ADF_ID";
        case FS200AC::DME_ON: return "DME_ON";
        case FS200AC::DME_NAV: return "DME_NAV";
        case FS200AC::AUTOPILOT_ON: return "AUTOPILOT_ON";
        case FS200AC::AUTOPILOT_HDG: return "AUTOPILOT_HDG";
        case FS200AC::AUTOPILOT_ALT: return "AUTOPILOT_ALT";
        case FS200AC::FUEL: return "FUEL";

        case FS200AC::GEAR: return "GEAR";
        case FS200AC::FLAPS: return "FLAPS";
        case FS200AC::FREQ_SELECT_RADIO: return "FREQ_SELECT_RADIO";

        case FS200AC::THROTTLE: return "THROTTLE";
        case FS200AC::PROP_RPM: return "PROP_RPM";
        case FS200AC::MIXTURE: return "MIXTURE";
        case FS200AC::COWL_FLAP: return "COWL_FLAP";
        case FS200AC::CARB_HEAT: return "CARB_HEAT";

        case FS200AC::TRIM_DN: return "TRIM_DN";
        case FS200AC::TRIM_UP: return "TRIM_UP";
        case FS200AC::AUTOPILOT_TRIM_DN: return "AUTOPILOT_TRIM_DN";
        case FS200AC::AUTOPILOT_TRIM_UP: return "AUTOPILOT_TRIM_UP";
    }
    return nullptr;
}
