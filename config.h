#pragma once

// 0 — светодиод только в Serial (для отладки логики без GPIO).
// 1 — штатный LED на GPIO 2.
#define USE_HARDWARE 1

// --- Wi-Fi точка доступа ---
constexpr char AP_SSID[] = "ESP32_Lab3";
constexpr char AP_PASSWORD[] = "12345678";  // минимум 8 символов для WPA2

// --- Аутентификация web-интерфейса ---
constexpr char AUTH_LOGIN[] = "admin";
constexpr char AUTH_PASSWORD[] = "lab3";

// --- Светодиод (штатный LED на большинстве DevKit) ---
constexpr uint8_t LED_PIN = 2;
constexpr uint8_t LED_PIN_MODE = OUTPUT;

// На ESP32 DevKit встроенный LED часто активен по LOW (HIGH = выключен).
constexpr bool LED_ACTIVE_LOW = true;

// --- Циклический буфер данных ---
constexpr size_t DATA_BUFFER_SIZE = 30;
constexpr unsigned long DATA_UPDATE_INTERVAL_MS = 1000;

// Имя cookie сессии после успешного входа.
constexpr char SESSION_COOKIE_NAME[] = "session";
