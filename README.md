# PIC16F18877 Integrated MCU Multi-Peripheral System
### Lab 4: Integrated MCU Multi-Peripheral System

## 📝 Project Overview
This project is a comprehensive embedded system designed for the **PIC16F18877** microcontroller. It demonstrates the simultaneous integration of analog sensing, PWM power modulation, high-precision timing for tachometry, and asynchronous serial communication.

The system features a dual-mode interface (ADC vs. DIP) to control fan speed and display brightness, while providing real-time RPM telemetry to a PC.

---

## 🚀 Key Modules & Functionality

### 1. Dual-Control PWM Engine
* [cite_start]**Peripheral:** CCP2 using Timer4[cite: 175].
* [cite_start]**Function:** A single PWM signal on **RC1** controls both the power delivered to a DC fan and the brightness of the 7-segment display [cite: 171-176].
* **Dynamic Adjustment:** In ADC mode, the duty cycle scales linearly with the potentiometer position.

### 2. High-Precision Tachometer (RPM Sensing)
* [cite_start]**Peripheral:** CCP3 (Capture Mode) using Timer3[cite: 2, 176].
* [cite_start]**Logic:** Measures the exact time between rising edges of the fan's tachometer signal on **RC0**[cite: 81].
* [cite_start]**Mathematical Precision:** Uses a 24-bit binary long division algorithm ($RPM = 15,000,000 / Period$) to ensure accurate 4-digit RPM reporting without a floating-point unit [cite: 19-22, 106].


### 3. Threshold Alarm & Buzzer
* **Peripheral:** CCP1 (PWM) on **RB5**.
* **Trigger:** Activated when the mapped ADC value reaches **12 or higher**.
* **Pattern:** Produces a 1kHz tone with a 1-second ON/OFF repeating pattern, managed by a Timer1 heartbeat flag.

### 4. Serial UART Reporting
* **Protocol:** RS-232 over UART (TX only) via **RC6**.
* [cite_start]**Telemetry:** Transmits a clear string (`"RPM = 1234\r\n"`) to a PC every 1 second[cite: 4].
* **Visual Feedback:** LED **RB2** toggles to indicate active data transmission.

---

## 📂 File Structure

The firmware follows a modular assembly design for high maintainability:

| File | Role |
| :--- | :--- |
| `main.asm` | [cite_start]Master orchestration, main loop, and variable definitions[cite: 185]. |
| `config.inc` | [cite_start]System-wide I/O initialization and PPS routing [cite: 177-184]. |
| `tach.inc` | [cite_start]Capture logic and the 24-bit/16-bit division algorithm[cite: 106]. |
| `fanspeed.inc`| [cite_start]PWM setup for fan power and display dimming [cite: 170-176]. |
| `adc.inc` | ADC sampling and mapping logic for the potentiometer. |
| `mode.inc` | Button debounce and state machine for Mode Control (RE1). |
| `seven_seg.inc`| BCD to 7-Segment decoding table. |

---

## 📍 Hardware Configuration

### Pin Mapping
| Pin | Function | Peripheral | Logic |
| :--- | :--- | :--- | :--- |
| **RA0** | Potentiometer | ADC (AN0) | 0-5V Input |
| **RB0** | Mode Status LED | GPIO | **Active-Low** (DIP Mode) |
| **RB1** | Alarm LED | GPIO | Active-High |
| **RB2** | UART Transmit LED| GPIO | Active-High |
| **RC0** | Fan Tachometer | CCP3 (Input)| [cite_start]10k$\Omega$ Pull-up required [cite: 200] |
| **RC1** | Fan/Display PWM | CCP2 (Output)| Transistor Driver |
| **RC6** | UART TX | EUSART | 9600 Baud, 8N1 |
| **RE1** | Mode Toggle | GPIO | **Active-Low** Button |



---

## 📐 The Mathematics of Speed
[cite_start]To achieve exact RPM values, the system uses a **500kHz Timer3 clock** (1:2 prescaler)[cite: 26, 37]. 
Each count represents **2$\mu s$**. With 2 pulses per revolution, the formula is:
[cite_start]$$RPM = \frac{60 \times 1,000,000}{4 \times T} = \frac{15,000,000}{T}$$ [cite: 17-19]

[cite_start]The value **15,000,000 (0xE4E1C0)** is stored as a 24-bit dividend for the division routine [cite: 21-22].

---

## 🛠 Build Instructions
1. Open the project in **MPLAB X IDE**.
2. Select **pic-as** as the assembler.
3. Link all `.inc` files in the `main.asm` file.
4. Program the **PIC16F18877** using a PICkit 4 or Snap debugger.
