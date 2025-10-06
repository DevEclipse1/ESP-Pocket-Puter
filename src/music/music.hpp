#include "../global.hpp"
#include "pitches.hpp"

#include "../selection.hpp"
#include "songs/doom.hpp"
#include "songs/mario.hpp"
#include "songs/nokia.hpp"
#include "songs/tetris.hpp"
#include "songs/christmas.hpp"

void Music_Callback()
{
    String melody_names[] = {
        "Doom",
        "Mario",
        "Nokia",
        "Tetris",
        "Christmas",
        "[Exit]"
    };

    String selected = SelectionMenu(melody_names, sizeof(melody_names) / sizeof(String));
    selected.toLowerCase();

    const int *srcMelody;
    const int *srcDurations;
    int numNotes;

    if (selected == "doom") {
        srcMelody = doom_melody;
        srcDurations = doom_durations;
        numNotes = sizeof(doom_melody) / sizeof(int);
    } else if (selected == "mario") {
        srcMelody = mario_melody;
        srcDurations = mario_durations;
        numNotes = sizeof(mario_melody) / sizeof(int);
    } else if (selected == "nokia") {
        srcMelody = nokia_melody;
        srcDurations = nokia_durations;
        numNotes = sizeof(nokia_melody) / sizeof(int);
    } else if (selected == "tetris") {
        srcMelody = tetris_melody;
        srcDurations = tetris_durations;
        numNotes = sizeof(tetris_melody) / sizeof(int);
    } else if (selected == "christmas") {
        srcMelody = christmas_melody;
        srcDurations = christmas_durations;
        numNotes = sizeof(christmas_melody) / sizeof(int);
    } else {
        return;
    }

    display.clearDisplay();
    Display_PrintCentered("Playing:\n%s", selected.c_str());
    display.display();

    for (int note = 0; note < numNotes; note++) {
        if (ReadButton(BUTTON_CENTER)) {
            HaltTillRelease(BUTTON_CENTER);
            noTone(BUZZER_PIN);
            return;
        }

        int melodyNote = pgm_read_word(&srcMelody[note]);
        int noteDuration = pgm_read_word(&srcDurations[note]);
        int duration = 1000 / noteDuration;

        if (melodyNote != REST) {
            tone(BUZZER_PIN, melodyNote, duration);
        }

        int pauseBetweenNotes = duration * 1.30;
        delay(pauseBetweenNotes);

        noTone(BUZZER_PIN);
    }
}
