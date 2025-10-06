#include "../global.hpp"
#include "pitches.hpp"

void Beep_Callback()
{
    tone(BUZZER_PIN, 1000, 100);
    delay(100);
    noTone(BUZZER_PIN);
}