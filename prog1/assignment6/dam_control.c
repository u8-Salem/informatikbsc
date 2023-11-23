#include "base.h"

enum dam_mode_e {
    DM_IDLE,
    DM_PUMP_ONE,
    DM_PUMP_TWO,
    DM_PUMP_EMERGENCY,
};

typedef enum dam_mode_e DamMode;

DamMode dam_control(double water_level) {
    // TODO
    return DM_IDLE;
}

void dam_control_test(void) {
    // TODO
    test_equal_i(dam_control(0.), DM_IDLE);
    test_equal_i(dam_control(20.), DM_PUMP_ONE);
    test_equal_i(dam_control(40.), DM_PUMP_TWO);
    test_equal_i(dam_control(123557.), DM_PUMP_EMERGENCY);
}

int main(void) {
    dam_control_test();
    return 0;
}
