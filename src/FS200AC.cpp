#include <chrono>
#include <thread>
#include <cassert>

#include "FS200AC/FS200AC.hpp"
#include "FS200AC/internal/FS200ACInitialState.hpp"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using namespace std::chrono_literals;

typedef std::chrono::steady_clock Clock;

const uint8_t COMMAND_RESET = 0x16;
const uint8_t CODE_ACKNOWLEDGE = 0x06;

enum EventID {
    ID_NONE = 0,

    // knobs
    // uint16_t
    ID_NAV1_COURSE_SELECTOR = 0x02,
    ID_NAV2_OBS = 0x04,
    ID_ADF_BRG = 0x06,
    ID_BARO = 0x08,
    ID_AUTOPILOT_HEADING = 0x0a,
    ID_FREQ_TUNE = 0x0c,
    // 2x uint8_t
    ID_FREQ_TUNE_XPNDR = 0x0e, // special handling

    // momentary buttons
    // no state
    ID_NAV1_USE_STBY = 0x10,
    ID_NAV2_USE_STBY = 0x12,
    ID_ADF_USE_STBY = 0x14,
    ID_TIMER = 0x42,
    ID_RMI = 0x44,
    ID_TRIM = 0x3e, // 2x
    ID_AUTOPILOT_TRIM = 0x40, // 2x
    ID_YOKE_SWITCH_DOWN = 0x46,
    ID_YOKE_SWITCH_UP = 0x48,

    // toggle buttons
    // 1-bit state
    ID_NAV1_ON = 0x18,
    ID_NAV1_ID = 0x1a,
    ID_NAV1_RAD = 0x1c,
    ID_NAV2_ON = 0x1e,
    ID_NAV2_ID = 0x20,
    ID_NAV2_RAD = 0x22,
    ID_ADF_ON = 0x24,
    ID_ADF_ID = 0x26,
    ID_DME_ON = 0x28,
    ID_DME_NAV = 0x2a,
    ID_AUTOPILOT_ON = 0x2c,
    ID_AUTOPILOT_HDG = 0x2e,
    ID_AUTOPILOT_ALT = 0x30,
    ID_FUEL = 0x36,

    // switches
    // uint8_t
    ID_GEAR = 0x32,
    ID_FLAPS = 0x34,
    ID_FREQ_SELECT_RADIO = 0x16,

    // sliders
    // uint8_t
    ID_THROTTLE = 0x38,
    ID_PROP_RPM = 0x3a,
    ID_MIXTURE = 0x3c,
    ID_COWL_FLAP = 0x4a,
    ID_CARB_HEAT = 0x4c,
};

auto retry = [](int count, auto func) {
    for (int i = 0; i < count; i++) {
        if (func()) {
            return true;
        }
    }
    assert(false);
    return false;
};

FS200AC::FS200AC(FS200AC::SerialProvider &serial) : m_serial(serial) {
}

FS200AC::~FS200AC() {
    reset_console();
}

bool FS200AC::initialize(ControlsState &controls, const ConsoleState &initial_state) {
    if (!reset_console()) {
        assert(false);
        return false;
    }
    if (!get_controls_state(controls)) {
        assert(false);
        return false;
    }
    return setup_console(initial_state);
}

bool FS200AC::wait_on_code(uint8_t code, int timeout) {
    auto start = Clock::now();
    uint8_t value = 0;
    do {
        m_serial.read(&value, 1);
    } while (value != code && duration_cast<milliseconds>(Clock::now() - start).count() < timeout);
    return value == code;
}

bool FS200AC::send_command(uint8_t command, bool wait) {
    m_serial.setWriteTimeout(42);
    uint8_t bytes[] = {0xa5, command, (uint8_t)(~(command ^ 0xa5) & 0x7f)};
    for (int i = 0; i < 3; i++) {
        if (!m_serial.write(&bytes[i], 1)) {
            assert(false);
            return false;
        }
        std::this_thread::sleep_for(42ms);
    }
    if (wait) {
        return wait_on_code(CODE_ACKNOWLEDGE, 500);
    }
    return true;
}

bool FS200AC::reset_console(bool retry) {
    if (wait_on_code('X', 50)) {
        return true;
    }
    for (int i = 0; i < 3; i++) {
        send_command(COMMAND_RESET, false);
        wait_on_code(CODE_ACKNOWLEDGE, 500);
    }
    if (!wait_on_code('X', 500)) {
        write_byte(CODE_ACKNOWLEDGE);
        return reset_console(false);
    }
    return true;
}

bool FS200AC::try_get_controls_state(ControlsState &controls) {
    if (!wait_on_code(0xa5, 440)) {
        assert(false);
        return false;
    }
    m_serial.setReadTimeout(42, 440);
    uint8_t ck = 0;
    if (!m_serial.read(&ck, 1) ||
        !m_serial.read((uint8_t*)&controls, sizeof(controls))) {
        assert(false);
        return false;
    }
    static_assert(sizeof(ControlsState) == 21);
    for (int i = 0; i < sizeof(controls); i++) {
        ck ^= ((uint8_t*)&controls)[i];
    }
    controls.major_version -= 48;
    controls.minor_version -= 48;
    uint8_t checkbyte;
    if (!m_serial.read(&checkbyte, 1)) {
        assert(false);
        return false;
    }
    return (ck ^ checkbyte) == 0x7f;
}

bool FS200AC::get_controls_state(ControlsState &controls) {
    if (!send_command(0x36, true)) {
        assert(false);
        return false;
    }
    if (!send_command(0x23, true)) {
        assert(false);
        return false;
    }
    for (int i = 0; i < 3; i++) {
        if (!try_get_controls_state(controls)) {
            assert(false);
            return false;
        } else {
            if (m_serial.write(&CODE_ACKNOWLEDGE, 1)) {
                return true;
            }
        }
    }
    assert(false);
    return false;
}

bool FS200AC::setup_console(const ConsoleState &state) {
    uint8_t buffer[32] = {
        state.nav1_standby_freq.second, state.nav1_standby_freq.first,
        state.nav1_current_freq.second, state.nav1_current_freq.first,

        state.nav2_standby_freq.second, state.nav2_standby_freq.first,
        state.nav2_current_freq.second, state.nav2_current_freq.first,

        state.adf_standby_freq.second, state.adf_standby_freq.first,
        state.adf_current_freq.second, state.adf_current_freq.first,
        state.current_radio,

        (uint8_t)(state.nav1_course_selector & 0x7f),
        (uint8_t)(state.nav1_course_selector >> 7),

        (uint8_t)(state.nav2_obs & 0x7f),
        (uint8_t)(state.nav2_obs >> 7),

        (uint8_t)(state.adf_brg & 0x7f),
        (uint8_t)(state.adf_brg >> 7),

        (uint8_t)(state.baro & 0x7f),
        (uint8_t)(state.baro >> 7),

        (uint8_t)(state.autopilot_hdg & 0x7f),
        (uint8_t)(state.autopilot_hdg >> 7),

        state.unknown_freq.second, state.unknown_freq.first,
        
        state.com_freq.second, state.com_freq.first,

        state.unknown[0], state.unknown[1], state.unknown[2], state.unknown[3],
        0xbc
    };
    for (int i = 0; i < 31; i++) {
        buffer[31] ^= buffer[i];
    }
    buffer[31] = ~buffer[31] & 0x7f;

    if (!retry(3, [&]{ return wait_on_code(0xa5, 440) &&
                                 wait_on_code(0x23, 440) &&
                                 wait_on_code(0x5c, 440); })) {
        assert(false);
        return false;
    }
    if (!write_byte(CODE_ACKNOWLEDGE)) {
        assert(false);
        return false;
    }
    std::this_thread::sleep_for(28ms);
    return retry(3, [&] {
        if (!write_byte(0xa5) || !write_byte(0x19)) {
            assert(false);
            return false;
        }
        if (!m_serial.write(buffer, sizeof(buffer))) {
            assert(false);
            return false;
        }
        return wait_on_code(6, 440);
    });
}

bool FS200AC::write_byte(uint8_t b) {
    return m_serial.write(&b, 1);
}

void FS200AC::fill_event(Event &event, uint8_t id, uint8_t b2, uint8_t b3) {
    event.control = (Control)id;
    if (!id) {
        event.type = None;
        return;
    }
    switch (id) {
        // momentary switches, map these to buttons
        case ID_TRIM:
            event.type = Button;
            event.control = (Control)(Control::TRIM_DN + b2 - 1);
            break;
        case ID_AUTOPILOT_TRIM:
            event.type = Button;
            event.control = (Control)(Control::AUTOPILOT_TRIM_DN + b2 - 1);
            break;

        // knobs
        case ID_NAV1_COURSE_SELECTOR:
        case ID_NAV2_OBS:
        case ID_ADF_BRG:
        case ID_BARO:
        case ID_AUTOPILOT_HEADING:
        case ID_FREQ_TUNE:
            event.type = Knob;
            event.knob = ((uint16_t)b3 << 7) | b2;
            break;
        case ID_FREQ_TUNE_XPNDR:
            event.type = Knob;
            event.control = FREQ_TUNE_RADIO;
            event.knob = ((b2 & 3) << 8) | b3;
            break;

        // push buttons
        case ID_NAV1_USE_STBY:
        case ID_NAV2_USE_STBY:
        case ID_ADF_USE_STBY:
        case ID_TIMER:
        case ID_RMI:
        case ID_YOKE_SWITCH_DOWN:
        case ID_YOKE_SWITCH_UP:
            event.type = Button;
            break;

        // toggles
        case ID_NAV1_ON:
        case ID_NAV1_ID:
        case ID_NAV1_RAD:
        case ID_NAV2_ON:
        case ID_NAV2_ID:
        case ID_NAV2_RAD:
        case ID_ADF_ON:
        case ID_ADF_ID:
        case ID_DME_ON:
        case ID_DME_NAV:
        case ID_AUTOPILOT_ON:
        case ID_AUTOPILOT_HDG:
        case ID_AUTOPILOT_ALT:
        case ID_FUEL:
            event.type = Toggle;
            event.toggle = (bool)b2;
            break;


        // logically treated as a switch
        case ID_FREQ_SELECT_RADIO:
        // mechanical switches
        case ID_GEAR:
        case ID_FLAPS:
            event.type = Switch;
            event.switch_ = b2;
            break;

        // sliders
        case ID_THROTTLE:
        case ID_PROP_RPM:
        case ID_MIXTURE:
        case ID_COWL_FLAP:
        case ID_CARB_HEAT:
            event.type = Slider;
            event.slider = b2;
            break;
        default:
            assert(false);
            break;
    }
}

bool FS200AC::poll(uint8_t &roll, uint8_t &pitch, uint8_t &yaw, Event &event) {
    if (!wait_on_code(0xa5, 100)) {
        return false;
    }
    uint8_t buff[8];
    uint8_t ck = 0;
    for (int i = 0; i < 8; i++) {
        if (!m_serial.read(&buff[i], 1)) {
            return false;
        }
        ck ^= buff[i];
    }
    if (ck != 0x7f) {
        return false;
    }
    if (!write_byte(CODE_ACKNOWLEDGE)) {
        return false;
    }
    roll = (buff[0] & 2) ? -buff[2] : buff[2];
    pitch = (buff[0] & 1) ? -buff[1] : buff[1];
    yaw = (buff[0] & 4) ? -buff[3] : buff[3];
    fill_event(event, buff[4], buff[5], buff[6]);
    return true;
}

