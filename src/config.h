#pragma once

#define USE_HARDWARE 1

// Подключение ESP32 к Wi-Fi сети (точка доступа телефона / роутера)
constexpr char WIFI_SSID[] = "Infinix NOTE 30i";
constexpr char WIFI_PASSWORD[] = "12345678";
constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 20000;

constexpr char AUTH_LOGIN[] = "admin";
constexpr char AUTH_PASSWORD[] = "lab3";

constexpr uint8_t LED_PIN = 2;
constexpr uint8_t LED_PIN_MODE = OUTPUT;
// false — HIGH включает LED (типично для DevKit). true — LOW включает.
constexpr bool LED_ACTIVE_LOW = false;

constexpr uint8_t I2C_SDA_PIN = 21;
constexpr uint8_t I2C_SCL_PIN = 22;
constexpr uint8_t AHT20_I2C_ADDRESS = 0x38;

constexpr size_t DATA_BUFFER_SIZE = 30;
constexpr unsigned long DATA_UPDATE_INTERVAL_MS = 1000;

// Часовой пояс для подписей времени (МСК = UTC+3)
constexpr long TIME_GMT_OFFSET_SEC = 3 * 3600;
constexpr int TIME_DAYLIGHT_OFFSET_SEC = 0;

constexpr char SESSION_COOKIE_NAME[] = "session";

constexpr float SIM_INITIAL_TEMPERATURE = 23.5f;
constexpr float SIM_INITIAL_HUMIDITY = 55.0f;
