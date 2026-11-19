# Electrical verification checklist

**Do not change a status to VERIFIED without recorded evidence and reviewer sign-off.** No live tests have been performed for this draft.

- [ ] **Power — BLOCKED:** Battery chemistry, series/parallel, full charge voltage documented. Evidence: Pack builder specification.
- [ ] **Power — BLOCKED:** BMS protected output and current ratings documented. Evidence: BMS label/manual.
- [ ] **Power — BLOCKED:** VESC input voltage and current limits confirmed. Evidence: VESC Tool/screenshots.
- [ ] **Protection — BLOCKED:** Main fuse location, DC interrupt and current ratings approved. Evidence: Reviewed schematic.
- [ ] **Protection — BLOCKED:** Anti-spark IN/OUT, disconnect/precharge and emergency isolation approved. Evidence: Manual and reviewer.
- [ ] **Protection — BLOCKED:** DC/DC branch fuse and return plan approved. Evidence: Reviewed schematic.
- [ ] **Low voltage — NOT TESTED:** DROK output polarity and 5 V under load verified before connecting MCU. Evidence: Meter readings.
- [ ] **Low voltage — BLOCKED:** ESP32 board variant and correct supply pin verified. Evidence: Carrier schematic.
- [ ] **Signals — BLOCKED:** VESC UART pinout, baud, 3.3 V levels, common ground and watchdog verified. Evidence: Manual/scope/VESC Tool.
- [ ] **Signals — BLOCKED:** PAS supply/output type, safe logic and measured pulses/rev verified. Evidence: Scope/logic capture.
- [ ] **Signals — BLOCKED:** OLED controller, address, resolution, supply and pullup levels verified. Evidence: I2C scan / module data.
- [ ] **Safety — BLOCKED:** Brake switch fitted; pressed and open-wire states inhibit assist. Evidence: Motor-disabled fault test.
- [ ] **Safety — BLOCKED:** Wheel speed source calibrated; cutoff behavior reviewed. Evidence: Test log.
- [ ] **Telemetry — BLOCKED:** SOC source verified or display remains SOC --%. Evidence: BMS protocol/current sensor.
- [ ] **Mechanical — NOT TESTED:** Wire gauges/connectors/strain relief/steering slack reviewed. Evidence: Bike inspection.
- [ ] **Testing — NOT TESTED:** Motor output disabled; no unexpected assist on reset/unplug/fault. Evidence: Signed test log.

## Suggested evidence filenames

`battery_pack_spec.pdf`, `bms_label.jpg`, `vesc_tool_limits.png`, `vesc_uart_pinout.pdf`, `dc_dc_5v_measurement.jpg`, `esp32_carrier_pinmap.pdf`, `pas_scope_capture.png`, `oled_i2c_scan.txt`, `brake_fault_test.csv`, `harness_routing_photos/`. Do not upload private or sensitive pack-builder information without permission.
