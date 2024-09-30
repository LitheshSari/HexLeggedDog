//
// Created by biao on 24-9-10.
//

#include "unitree_guide_controller/FSM/StateFixedStand.h"

#include <cmath>

StateFixedStand::StateFixedStand(CtrlComponent &ctrlComp): FSMState(
    FSMStateName::FIXEDSTAND, "fixed stand", ctrlComp) {
    duration_ = ctrl_comp_.frequency_ * 1.2;
}

void StateFixedStand::enter() {
    for (int i = 0; i < 12; i++) {
        start_pos_[i] = ctrl_comp_.joint_position_state_interface_[i].get().get_value();
    }
    ctrl_comp_.control_inputs_.get().command = 0;
}

void StateFixedStand::run() {
    percent_ += 1 / duration_;
    phase = std::tanh(percent_);
    for (int i = 0; i < 12; i++) {
        ctrl_comp_.joint_position_command_interface_[i].get().set_value(
            phase * target_pos_[i] + (1 - phase) * start_pos_[i]);
        ctrl_comp_.joint_velocity_command_interface_[i].get().set_value(0);
        ctrl_comp_.joint_torque_command_interface_[i].get().set_value(0);
        ctrl_comp_.joint_kp_command_interface_[i].get().set_value(
            phase * 60.0 + (1 - phase) * 20.0);
        ctrl_comp_.joint_kd_command_interface_[i].get().set_value(3.5);
    }
}

void StateFixedStand::exit() {
    percent_ = 0;
}

FSMStateName StateFixedStand::checkChange() {
    if (percent_ < 1.5) {
        return FSMStateName::FIXEDSTAND;
    }
    switch (ctrl_comp_.control_inputs_.get().command) {
        case 1:
            return FSMStateName::FIXEDDOWN;
        case 3:
            return FSMStateName::FREESTAND;
        case 4:
            return FSMStateName::TROTTING;
        case 9:
            return FSMStateName::SWINGTEST;
        case 10:
            return FSMStateName::BALANCETEST;
        default:
            return FSMStateName::FIXEDSTAND;
    }
}
