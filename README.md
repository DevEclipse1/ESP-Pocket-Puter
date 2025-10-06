# ESP Pocket Puter

A compact **ESP32-based Flipper Zero alternative** with a display, buttons, buzzer, infrared transmitter, and CC1101 RF support.

<img src="https://i.imgur.com/P2nhIcZ.jpeg" width="300">

---

## ✨ Features

### 🧠 Core
- Menu-driven firmware built for the ESP32 platform
- Compact UI for display + buttons
- Modular system (Wi-Fi, Bluetooth, IR, RF, Music)

### 📡 Wi-Fi
- Scan for nearby networks  
- Beacon spam / fake AP creation  

### 📶 Bluetooth
- Device scanner  
- Fastpair spam  
- “Sour Apple” spam  

### 📺 Infrared
- TV-B-Gone functionality  
- Universal remote for TVs, projectors, etc.  
- IR spammer (“jammer”)  

### 📻 RF (CC1101)
- Frequency identifier  
- Jammer *(⚠️ illegal in most countries — use responsibly)*  
- Scan & copy RF codes  
- Replay saved codes  
- Tesla charge port opener  

### 🎵 Music
- Beep test  
- Music player (DOOM theme, Nokia ringtone, Tetris, etc.)  
- Mini 3-key piano  

---

## 🧩 Hardware Requirements

- ESP32-C3 super mini (any other esp32 should work)
- SSD1306 I2C 128x64 Display
- Buzzer (Not needed, but its fun)
- IR Transmitter
- CC1101 RF transceiver module  
- Button inputs (Up, Ok, Down)  
- Battery & Battery protection circuit (Recommended, for convenience)

! Every pin definition is [Here](https://github.com/DevEclipse1/ESP-Pocket-Puter/blob/main/src/global.hpp) !

---

## ⚙️ Flashing

1. Clone the repo:
   ```bash
   git clone https://github.com/DevEclipse1/ESP-Pocket-Puter.git
   cd ESP-Pocket-Puter
   ```
2. Install platformio for vscode if you havent already
3. Change pinouts if needed, see [The global.hpp for pin defs](https://github.com/DevEclipse1/ESP-Pocket-Puter/blob/main/src/global.hpp)
4. Change platformio.ini if needed (if you are using a different board)
5. Upload!
