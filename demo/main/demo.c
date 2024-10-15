#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdio.h>

#define LED_GPIO_PIN 2

static const char *TAG = "DEMO";

uint32_t delay_interval = 1000;
SemaphoreHandle_t xMutex;
const BaseType_t app_cpu = 0; // Define the app_cpu variable

void set_interval(void *pvParameter) {
  while (1) {
    // Random delay between 500ms and 1000ms
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      uint32_t random_delay = 500 + (esp_random() % 500);
      delay_interval = random_delay;
      ESP_LOGI(TAG, "New delay interval set: %d ms", delay_interval);
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Non-blocking delay
  }
}

// Task that controls GPIO Pin 2 to blink onboard LED
void blink_task(void *pvParameter) {
  while (1) {
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      ESP_LOGI(TAG, "Using %d ms delay interval", delay_interval);
      gpio_set_level(LED_GPIO_PIN, 1);
      vTaskDelay(delay_interval / portTICK_PERIOD_MS); // Non-blocking delay
      gpio_set_level(LED_GPIO_PIN, 0);
      vTaskDelay(delay_interval /
                 portTICK_PERIOD_MS); // RTOS based on tick timer
      xSemaphoreGive(xMutex);
    }
  }
}

void app_main(void) {
  esp_log_level_set("*", ESP_LOG_INFO); // Set log level to INFO for all tags
  xMutex = xSemaphoreCreateMutex();
  if (xMutex == NULL) {
    ESP_LOGE(TAG, "Failed to create mutex");
    return;
  }
  gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

  // Restrict only 1 core for demo purposes
  xTaskCreatePinnedToCore(
      &blink_task,  // Task
      "blink_task", // Task name
      1024,         // Stack size in bytes
      NULL,         // Parameter to pass to function
      5,            // Task to priority
      NULL,
      app_cpu); // Task Handle to check on status or memory usage or end it

  // Task for setting intervals
  xTaskCreatePinnedToCore(&set_interval, "set_interval_task", 1024, NULL, 10,
                          NULL, app_cpu);
}
