#define DEVICE____________LEFT true
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

#define COMMAND_LABEL_____UUID "067c3c93-eb63-4905-b292-478642f8ae99"  // for remote writing a label (moving over multiple matrices)
#define COMMAND_WORD______UUID "3dfde050-8432-4f2f-9366-de27c430c05c"  // for remote writing a word (static 4-letter word)
#define COMMAND_MODUS_____UUID "a8792ef9-c01c-47ee-a063-f8712bf8dd67"  // for remote reading/writing the modus
#define COMMAND_MODUS_DSC_UUID "ad965bf4-c9fc-4a29-94b0-c443259a06f6"  // for remote reading/writing the modus (description)
#define COMMAND_LIGHT_____UUID "28c64d42-4958-43f0-8845-5a106498bf1d"  // for remote reading/writing the light
#define COMMAND_LIGHT_DSC_UUID "a10d6f93-c4eb-445c-a154-24548909ccb3"  // for remote reading/writing the light (description)

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

#if DEVICE____________LEFT == true
static const char* WORDS[] PROGMEM = {"LOVE", "ICON", "GAIN", "HYPE", "UNDO", "COOL", "HOPE", "KISS", "MOON", "FEEL", "HELP", "DEAL", "FACT", "GAME", "KIND", "MIND", "DARE", "EARN", "WELL", "GROW", "JUMP", "KEEP", "BOLD", "CALM", "DEEP", "EASY", "GOOD", "FULL", "GLAD", "HIGH", "NEAR", "PULL", "PLUS", "SAVE", "FAST", "LIKE", "FREE", "HERO", "BEST", "BODY", "DEAR", "FACE", "FLOW", "LEAD", "USER", "YEAH", "PEAK", "FUND", "LIST", "NEWS",
                                      "PAIR", "LIST", "NOTE", "SELF", "BASS", "FINE", "BLUE", "HALF", "LIVE", "LONG", "HOUR", "LOOK", "MARK", "MOVE", "LIFT", "GOAL", "BAND", "SOFT", "CARE", "PURE", "FUND", "STAY", "MORE", "WARM", "LAMB", "TEAM", "VIEW", "REAL", "HAVE", "LINK", "RISE", "RELY", "WISH", "WAIT", "TALK", "REST", "MANY", "RICH", "KNOW", "JOIN", "RIDE", "TIDY", "HUGE", "HEAL", "NICE", "OKAY", "FAIR", "NEAT", "TRUE", "LIFE"};
static const uint8_t WORD_COUNT = 100;
#else
static const char* WORDS[] PROGMEM = {"HATE", "FAIL", "GANG", "JUNK", "FAKE", "BAIT", "CAGE", "SCAR", "SLOW", "WOLF", "PAIN", "TINY", "LUSH", "FIRE", "FUCK", "LESS", "LOSS", "BURN", "DENY", "FALL", "FEAR", "HURT", "KILL", "NONE", "EVIL", "DULL", "FOUL", "GRIM", "DAMN", "GORE", "GOSH", "HELL", "LATE", "DUST", "DARK", "DUTY", "RISK", "WARN", "SLIP", "UGLY", "WILD", "VAIN", "POOR", "MEAN", "RUDE", "SORE", "VOID", "ANTI", "DIRE", "COPE",
                                      "COST", "PUSH", "MYTH", "URGE", "RAIN", "REDO", "NEED", "SEEM", "LONE", "VAST", "PALE", "SICK", "RARE", "BEEF", "FOOL", "DEAF", "DRUG", "ITCH", "PITY", "LACK", "WANT", "SEEK", "TRIP", "DOWN", "GREY", "RULE", "WORK", "TASK", "STOP", "LOUD", "MUST", "GIVE", "LOSE", "TORN", "ONLY", "SOLO", "TRAP", "FLOP", "QUIT", "SUNK", "HOWL", "ENVY", "CYST", "COLD", "DROP", "BASE", "FLEE", "OVER", "HARD", "PASS"};
static const uint8_t WORD_COUNT = 100;
#endif

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
    MODUS________FREQU,
    MODUS________PARTY  // show label for a while then go back to frequency
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