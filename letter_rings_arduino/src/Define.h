#define DEVICE____________LEFT false
// #define DEVICE___________RIGHT !DEVICE____________LEFT

#if DEVICE____________LEFT == true
#define COMMAND_SERVICE___UUID "791320d5-7f0a-4b58-89f6-cc2031479da5"
#define DEVICE____________SIDE "L"
#define BLE_DEVICE_NAME "LETTER_RINGS_L"
#else
#define COMMAND_SERVICE___UUID "d0e5a78d-614b-4f8c-86cd-e782d832228f"
#define DEVICE____________SIDE "R"
#define BLE_DEVICE_NAME "LETTER_RINGS_R"
#endif

#define COMMAND_LABEL_____UUID "067c3c93-eb63-4905-b292-478642f8ae99"
#define COMMAND_MODUS_____UUID "a8792ef9-c01c-47ee-a063-f8712bf8dd67"  // for remote reading/writing the modus
#define COMMAND_MODUS_DSC_UUID "ad965bf4-c9fc-4a29-94b0-c443259a06f6"  // for remote reading/writing the modus (description)

#define AUDIO__________SAMPLES 512    // Must be a power of 2
#define AUDIO____SAMPLING_FREQ 40000  // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AUDIO________NUM_BANDS 16

#define BUTTON_DEBOUNCE_MILLIS 100

#define AUDIO________NUM_ORDER 3

#ifndef Define_h
#define Define_h

#include <Arduino.h>

static const uint16_t DISPLAY__WIDTH = 135;
static const uint16_t DISPLAY_HEIGHT = 240;

static const char* WORDS[] PROGMEM = {"HATE", "FAIL", "GANG", "JUNK", "FAKE", "BAIT", "CAGE", "SCAR", "VAMP", "COKE", "THUG", "PIMP", "PAIN", "PULP", "LUSH", "FIRE", "FUCK", "LESS", "ARMY", "LOSS", "BURN", "DENY", "FALL", "FEAR", "HURT", "KILL", "NONE", "EVIL", "DUMB", "FOUL", "GRIM", "DAMN", "GORE", "GOSH", "HELL", "SITH", "LATE", "DUST", "DARK", "DUTY",
                                      "RISK", "WARN", "SLIP", "UGLY", "WILD", "VAIN", "POOR", "MEAN", "RUDE", "SORE", "ZERO", "ANTI", "DIRE", "LAVA", "WOKE", "COPE", "COST", "BANG", "OOPS", "OUCH", "URGE", "WEST", "RAIN", "ARMY", "NEED", "LOSE", "SEEM", "LONE", "VAST", "PALE", "LAZY", "RARE", "BOLT", "BEEF", "FOOL", "DEAF", "DRUG", "ITCH", "PITY", "JURY",
                                      "HULK", "LACK", "WANT", "SEEK", "SLAY", "TRIP", "LEFT", "NERD", "DOWN", "GREY", "RULE", "WORK", "TASK", "STOP", "MUCH", "MUST", "AMMO", "NAIL", "TORN", "ONLY", "SOLO", "TRAP", "FLOP", "QUIT", "SUNK", "HOWL", "EXAM", "ENVY", "CYST", "MINE", "MORE", "RACE", "LINE", "GLUE", "OVER", "HARD", "BEAT", "AWAY", "SIGN", "MALE"};

const gpio_num_t AUDIO______________PIN = GPIO_NUM_8;  // A5

typedef enum : int8_t {
    ORIENTATION______UP = -1,  // fingers pointing up, matrix bottom is where the pins are
    ORIENTATION____DOWN = 1    // fingers pointing down, matrix top is where the pins are
} orientation___e;

typedef enum : uint8_t {
    BUTTON_ACTION_MODUS,
    BUTTON_ACTION_DECAY,  // the decay of the frequency bands
    BUTTON_ACTION_LIGHT
} button_action_e;

typedef enum : uint8_t {
    MODUS________WORDS,
    MODUS________LABEL,
    MODUS________FREQU
} modus_________e;

typedef enum : uint8_t {
    TEXT_HALIGN___LEFT,
    TEXT_HALIGN_CENTER,
    TEXT_HALIGN__RIGHT
} text_halign___e;

typedef enum : uint32_t {
    COLOR____WHITE = 0x333333,
    COLOR______RED = 0xFF0000,
    COLOR___ORANGE = 0x375A00,
    COLOR____GREEN = 0x00FF00,
    COLOR___YELLOW = 0x333300,
    COLOR_____BLUE = 0x0000FF,
    COLOR____OCEAN = 0x000011,
    COLOR_____CYAN = 0x003333,
    COLOR__MAGENTA = 0x660066,
    COLOR____BLACK = 0x000000,
    COLOR_____GRAY = 0x060606
} color_________e;

typedef struct {
    double x;
    double y;
    double z;
} vector________t;

typedef struct {
    double a;
    double b;
    double c;
} quadfit_______t;

#endif