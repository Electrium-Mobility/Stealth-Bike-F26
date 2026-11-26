# docs(electrical): add photographed hardware review and proposed harness verification

## Summary
- Add preliminary electrical architecture diagram (SVG/PNG/PDF and editable Graphviz DOT).
- Record component markings from team photos, including DROK 5 V converter, ESP32-S, Flipsky MINI V6.7 PRO and Anti-spark Pro V3.0.
- Add connector/harness register and sign-off checklist with all unverified pins and safety gates clearly marked.
- Highlight ESP32 versus ESP8266 documentation inconsistency for team review.

## Scope
Documentation only. No firmware change, hardware connection, energization, VESC setting modification, or bench test is claimed.

## Blocking questions for reviewers
1. Battery/BMS chemistry, max charge voltage and discharge limits?
2. VESC model/firmware, max voltage/current, UART pinout/baud and watchdog?
3. Fuse/disconnect/precharge, branch protection, and grounding approval?
4. Brake-cutoff hardware, wheel-speed calibration and SOC source?
5. Which MCU carrier and OLED controller are installed on the bike?

## Review requested
Electrical lead + firmware lead. Keep this draft marked **NOT APPROVED FOR WIRING OR POWER-UP** until evidence and signatures are complete.
