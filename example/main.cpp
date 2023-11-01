#include <cstdio>

#include <FS200AC/FS200AC.hpp>
#include <serial/serial.h>

const char *control_name(FS200AC::Control control);

FS200AC::Radio current_radio = FS200AC::Radio_NAV1;

class SerialProvider : public FS200AC::SerialProvider {
    public:
    SerialProvider(serial::Serial &serial) : m_serial(serial) {
        // 9600 8N1
        serial.setBaudrate(9600);
        serial.setRTS(false);
        serial.setBytesize(serial::eightbits);
        serial.setParity(serial::parity_none);
        serial.setStopbits(serial::stopbits_one);
    }

    virtual bool read(uint8_t *buffer, std::size_t count) {
        return m_serial.read(buffer, count) == count;
    }

    virtual bool write(const uint8_t *buffer, std::size_t count) {
        return m_serial.write(buffer, count) == count;
    }

    virtual void setReadTimeout(unsigned int multiplier, unsigned int timeout_ms) {
        auto timeout = m_serial.getTimeout();
        timeout.read_timeout_multiplier = multiplier;
        timeout.read_timeout_constant = timeout_ms;
        m_serial.setTimeout(timeout);
    }

    virtual void setWriteTimeout(unsigned int multiplier) {
        auto timeout = m_serial.getTimeout();
        timeout.write_timeout_multiplier = multiplier;
        m_serial.setTimeout(timeout);
    }
    private:
    serial::Serial &m_serial;
};

bool handle_button(FS200AC::Control control) {
    printf("Button %s\n", control_name(control));
    if (control == FS200AC::Control::RMI) {
        printf("RMI pressed, exiting\n");
        return false;
    }
    return true;
}

bool handle_slider(FS200AC::Control control, uint8_t value) {
    printf("Slider %s: %d\n", control_name(control), value);
    return true;
}

void print_gear(uint8_t value) {
    switch (value) {
        case FS200AC::Gear_UP:
            printf("Landing gear UP\n");
            break;
        case FS200AC::Gear_DN:
            printf("Landing gear DOWN\n");
            break;
    }
}

void print_flaps(uint8_t value) {
    switch (value) {
        case FS200AC::Flaps_UP:
            printf("Flaps UP\n");
            break;
        case FS200AC::Flaps_APP:
            printf("Flaps APP\n");
            break;
        case FS200AC::Flaps_LDG:
            printf("Flaps LDG\n");
            break;
    }
}

bool handle_switch(FS200AC::Control control, uint8_t value) {
    printf("Switch %s: %d\n", control_name(control), value);
    switch (control) {
        case FS200AC::GEAR:
            print_gear(value);
            break;
        case FS200AC::FLAPS:
            print_flaps(value);
            break;
        case FS200AC::FREQ_SELECT_RADIO:
            current_radio = (FS200AC::Radio)value;
            break;
    }
    return true;
}

bool handle_toggle(FS200AC::Control control, bool value) {
    printf("Toggle %s: %s\n", control_name(control), value ? "on" : "off");
    return true;
}

void print_radio(FS200AC::Radio radio, uint16_t value) {
    switch (radio) {
        case FS200AC::Radio_NAV1:
        case FS200AC::Radio_NAV2:
        case FS200AC::Radio_COM:
            printf("%.2f MHz\n", value / 100.0f);
            break;
        case FS200AC::Radio_ADF:
            printf("%d KHz\n", value);
            break;
        case FS200AC::Radio_XPNDR:
            printf("%d %d\n", value & 0xff, value >> 8);
            break;
    }
}

bool handle_knob(FS200AC::Control control, uint16_t value) {
    printf("Knob %s: %d\n", control_name(control), value);
    switch (control) {
        case FS200AC::FREQ_TUNE_RADIO:
            print_radio(current_radio, value);
            break;
        case FS200AC::NAV1_COURSE_SELECTOR:
        case FS200AC::NAV2_OBS:
        case FS200AC::ADF_BRG:
        case FS200AC::AUTOPILOT_HEADING:
            printf("%d degrees\n", value);
            break;
        case FS200AC::BARO:
            printf("%d.%d inHg\n", value / 100, value % 100);
            break;
    }
    return true;
}

bool handle_event(const FS200AC::Event &event) {
    switch (event.type) {
        case FS200AC::None:
            return true;
        case FS200AC::Button:
            return handle_button(event.control);
        case FS200AC::Slider:
            return handle_slider(event.control, event.slider);
        case FS200AC::Switch:
            return handle_switch(event.control, event.switch_);
        case FS200AC::Toggle:
            return handle_toggle(event.control, event.toggle);
        case FS200AC::Knob:
            return handle_knob(event.control, event.knob);
    }
    return false;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <serial port>\n", argv[0]);
        return 1;
    }
    serial::Serial serial(argv[1]);
    SerialProvider provider(serial);
    FS200AC fs(provider);
    FS200AC::ControlsState controls;
    if (!fs.initialize(controls)) {
        fprintf(stderr, "Failed to initialize console\n");
        return 1;
    }
    printf("NAV1 ON:%d ID:%d RAD:%d\n", controls.nav1_on, controls.nav1_id, controls.nav1_rad);
    printf("NAV2 ON:%d ID:%d RAD:%d\n", controls.nav2_on, controls.nav2_id, controls.nav2_rad);
    printf("ADF ON:%d ID:%d\n", controls.adf_on, controls.adf_id);
    printf("DME ON:%d NAV:%d\n", controls.dme_on, controls.dme_nav);
    printf("AUTOPILOT ON:%d HDG:%d ALT:%d\n", controls.autopilot_on, controls.autopilot_hdg, controls.autopilot_alt);
    printf("GEAR:%d\n", controls.landing_gear);
    printf("FLAPS: %d\n", controls.flaps);
    printf("FUEL: %d\n", controls.fuel);
    printf("THROTTLE: %d\n", controls.throttle);
    printf("PROP RPM: %d\n", controls.prop_rpm);
    printf("MIXTURE: %d\n", controls.fuel_mixture);
    printf("Version: %d.%d\n", controls.major_version, controls.minor_version);

    bool done = false;
    FS200AC::Event event;
    while (!done) {
        int8_t roll, pitch, yaw;
        if (!fs.poll(roll, pitch, yaw, event)) {
            return 1;
        }
        done = !handle_event(event);
    }
    return 0;
}

