#ifndef FS200AC_HPP
#define FS200AC_HPP

#include <utility>
#include <cstdint>

class FS200AC {
    public:
    class SerialProvider {
        public:
            virtual void setReadTimeout(unsigned int multiplier, unsigned int timeout_ms) = 0;
            virtual void setWriteTimeout(unsigned int multiplier) = 0;
            virtual bool read(uint8_t *buffer, std::size_t count) = 0;
            virtual bool write(const uint8_t *buffer, std::size_t count) = 0;
    };
    struct ConsoleState;
    struct ControlsState;
    struct Event;

    #include "internal/FS200ACControls.hpp"

    FS200AC(SerialProvider &serial);
    ~FS200AC();
    bool initialize(ControlsState &controls, const ConsoleState &initial_state = DEFAULT_INITIAL_STATE);
    bool poll(int8_t &roll, int8_t &pitch, int8_t &yaw, Event &event);

    enum EventType {
        None,
        Button,
        Slider,
        Toggle,
        Switch,
        Knob,
    };

    struct Event {
        EventType type;
        Control control;
        union {
            uint8_t slider;
            bool toggle;
            uint8_t switch_;
            uint16_t knob;
        };
    };

    struct ConsoleState {
        typedef std::pair<uint8_t, uint8_t> RadioFrequency;
        uint8_t current_radio;
        RadioFrequency nav1_standby_freq, nav1_current_freq;
        RadioFrequency nav2_standby_freq, nav2_current_freq;
        RadioFrequency adf_standby_freq, adf_current_freq;
        RadioFrequency unknown_freq;
        RadioFrequency com_freq;
        uint16_t nav1_course_selector, nav2_obs, adf_brg, baro, autopilot_hdg;
        uint8_t unknown[4];
    };

    // stateful controls (not incl. carb heat and cowl flap) 
    #ifdef _MSC_VER
    #pragma pack(push, 1)
    #endif
    struct ControlsState {
        uint8_t nav1_on, nav1_id, nav1_rad;
        uint8_t nav2_on, nav2_id, nav2_rad;
        uint8_t adf_on, adf_id;
        uint8_t dme_on, dme_nav;
        uint8_t autopilot_on, autopilot_hdg, autopilot_alt;
        uint8_t landing_gear;
        uint8_t flaps;
        uint8_t fuel;
        uint8_t throttle, prop_rpm, fuel_mixture;
        // console version
        uint8_t major_version, minor_version;
    }
    #ifdef _MSC_VER
    #pragma pack(pop)
    #else
    __attribute__((__packed__))
    #endif
    ;

    private:
    static const ConsoleState DEFAULT_INITIAL_STATE;
    SerialProvider &m_serial;
    unsigned int m_read_timeout;
    unsigned int m_write_timeout;

    bool wait_on_code(uint8_t code, int timeout);
    bool send_command(uint8_t command, bool wait);
    // Note: also causes console to take state readings (returned by get_status())
    bool reset_console(bool retry = true);
    bool try_get_controls_state(ControlsState &controls);
    bool get_controls_state(ControlsState &controls);
    bool setup_console(const ConsoleState &state);
    bool write_byte(uint8_t b);
    void fill_event(Event &event, uint8_t b1, uint8_t b2, uint8_t b3);
};

#endif

