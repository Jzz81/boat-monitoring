#include "sleep.h"

#include "esp_sleep.h"

void deepSleepSeconds(uint32_t seconds)
{
    Serial.print("Going to deep sleep for ");
    Serial.print(seconds);
    Serial.println(" seconds.");

    Serial.flush();

    esp_sleep_enable_timer_wakeup(
        (uint64_t)seconds * 1000000ULL
    );

    esp_deep_sleep_start();
}

bool wokeFromTimer()
{
    return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
}
