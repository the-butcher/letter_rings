#define DEVICE____________LEFT true

#if DEVICE____________LEFT == true
#define COMMAND_SERVICE___UUID "791320d5-7f0a-4b58-89f6-cc2031479da5"
#define DEVICE____________SIDE "L"
#define BLE_DEVICE_NAME "LETTER_RINGS_L"
#define ACCEPT__ROLE_DOWNGRADE false
#else
#define COMMAND_SERVICE___UUID "d0e5a78d-614b-4f8c-86cd-e782d832228f"
#define DEVICE____________SIDE "R"
#define BLE_DEVICE_NAME "LETTER_RINGS_R"
#define ACCEPT__ROLE_DOWNGRADE true
#endif

#define USE__FORCE_______GAMPM false
#define USE_SERIAL_LOOP_OUTPUT false // measure loop duration
#define USE__________CLIP_DRAW false // draw red clip areas on display
#define USE__FORCE_BACKLITE_ON false // when USE__FORCE_BACKLITE_ON = true, the display backlite will never turn off, even when the display is inactive
#define USE_____MICVALS_OUTPUT false
#define USE_SERIAL_SYNC_OUTPUT false // write sync related messages (send, revc, millis) to the console


#define COMMAND_LABEL_____UUID "067c3c93-eb63-4905-b292-478642f8ae99"  // for remote writing a label (moving over multiple matrices)
#define COMMAND_WORD______UUID "3dfde050-8432-4f2f-9366-de27c430c05c"  // for remote writing a word (static 4-letter word)
#define COMMAND_MODUS_____UUID "a8792ef9-c01c-47ee-a063-f8712bf8dd67"  // for remote reading/writing the modus
#define COMMAND_LIGHT_____UUID "28c64d42-4958-43f0-8845-5a106498bf1d"  // for remote reading/writing the light
#define COMMAND_COEFF_____UUID "cfb68865-9243-4646-af27-750d02d44a3a"  // for remote reading/writing the orientation coefficient

#define AUDIO__________SAMPLES 512    // Must be a power of 2
#define AUDIO____SAMPLING_FREQ 40000  // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AUDIO________NUM_BANDS 16
#define AUDIO________NUM_ORDER 3
#define AUDIO________MAX_SCALE 0.0015 // a bit louder than silence (PC fan and else) which yields 0.002
#define AUDIO_BASE_SCALE_DECAY 0.9999

#define BUTTON_DEBOUNCE_MILLIS 100

#define ACCELERATION___SAMPLES 32
#define ACCELERATION_THRESHOLD 0.8  // threshold for the correlation value considered to be good enough
#define ACCELERATION_SIG_THRES 7.5 // TODO :: find a usable value, maybe configurable, i.e. 10
#define COEFFICIENT_THRES__MIN 0.6
#define COEFFICIENT_THRES__MAX 0.9

#ifndef Define_h
#define Define_h

#include <Arduino.h>

static const uint16_t DISPLAY__WIDTH = 135;
static const uint16_t DISPLAY_HEIGHT = 240;

// left
// bleAddress: D0:CF:13:0A:E1:C9
// staAddress: D0:CF:13:0A:E1:C8

// right
// bleAddress: 64:E8:33:73:F1:49
// staAddress: 64:E8:33:73:F1:48

#if DEVICE____________LEFT == true
static const char* WORDS[] PROGMEM = { "LOVE", "ICON", "GAIN", "HYPE", "UNDO", "COOL", "HOPE", "KISS", "MOON", "FEEL", "HELP", "DEAL", "FACT", "GAME", "KIND", "MIND", "DARE", "EARN", "WELL", "GROW", "JUMP", "KEEP", "BOLD", "CALM", "DEEP", "EASY", "GOOD", "FULL", "GLAD", "HIGH", "NEAR", "PULL", "PLUS", "SAVE", "FAST", "LIKE", "FREE", "HERO", "BEST", "BODY", "DEAR", "FACE", "FLOW", "LEAD", "USER", "YEAH", "PEAK", "FUND", "LIST", "NEWS",
                                      "PAIR", "LIST", "NOTE", "SELF", "BASS", "FINE", "BLUE", "HALF", "LIVE", "LONG", "HOUR", "LOOK", "MARK", "MOVE", "LIFT", "GOAL", "BAND", "SOFT", "CARE", "PURE", "FUND", "STAY", "MORE", "WARM", "LAMB", "TEAM", "VIEW", "REAL", "HAVE", "LINK", "RISE", "RELY", "WISH", "WAIT", "TALK", "REST", "MANY", "RICH", "KNOW", "JOIN", "RIDE", "TIDY", "HUGE", "HEAL", "NICE", "OKAY", "FAIR", "NEAT", "TRUE", "LIFE" };
static const uint8_t WORD_COUNT = 100;
static const uint8_t STA_ADDRESS_OUT[] = { 0x64, 0xE8, 0x33, 0x73, 0xF1, 0x48 };  // sta address of right device
static const int8_t BITMAPS_OFF = 1;                                            // 1 * ORIENTATION * 16 + 16 -> 0 when UP, 32 when DOWN
#else
static const char* WORDS[] PROGMEM = { "HATE", "FAIL", "GANG", "JUNK", "FAKE", "BAIT", "CAGE", "SCAR", "SLOW", "WOLF", "PAIN", "TINY", "LUSH", "FIRE", "FUCK", "LESS", "LOSS", "BURN", "DENY", "FALL", "FEAR", "HURT", "KILL", "NONE", "EVIL", "DULL", "FOUL", "GRIM", "DAMN", "GORE", "GOSH", "HELL", "LATE", "DUST", "DARK", "DUTY", "RISK", "WARN", "SLIP", "UGLY", "WILD", "VAIN", "POOR", "MEAN", "RUDE", "SORE", "VOID", "ANTI", "DIRE", "COPE",
                                      "COST", "PUSH", "MYTH", "URGE", "RAIN", "REDO", "NEED", "SEEM", "LONE", "VAST", "PALE", "SICK", "RARE", "BEEF", "FOOL", "DEAF", "DRUG", "ITCH", "PITY", "LACK", "WANT", "SEEK", "TRIP", "DOWN", "GREY", "RULE", "WORK", "TASK", "STOP", "LOUD", "MUST", "GIVE", "LOSE", "TORN", "ONLY", "SOLO", "TRAP", "FLOP", "QUIT", "SUNK", "HOWL", "ENVY", "CYST", "COLD", "DROP", "BASE", "FLEE", "OVER", "HARD", "PASS" };
static const uint8_t WORD_COUNT = 100;
static const uint8_t STA_ADDRESS_OUT[] = { 0xD0, 0xCF, 0x13, 0x0A, 0xE1, 0xC8 };  // sta address of left device
static const int8_t BITMAPS_OFF = -1;                                          // -1 * ORIENTATION * 16 + 16 -> 32 when UP, 0 when DOWN
#endif

const gpio_num_t AUDIO______________PIN = GPIO_NUM_8;  // A5

const uint64_t ROLE_PRI____DURATION_MS = 15000;
const uint64_t WORD_UPDATE_INTERVAL_MS = 14000;
const uint64_t PARTY_LABEL_DURATION_MS = 20000;
const uint64_t DISP_ACTIVE_DURATION_MS = 1000 * 60 * 3; // 3 minutes
const uint64_t GAMOL_______DURATION_MS = 10000;

/**
 * the interval we want the primary loop to run at (50ms -> 20 times/second)
 */
const uint64_t MAIN_LOOP_______DEST_MS = 50;

typedef enum : uint8_t {
    BITMAP_PAC____OPEN_R = 0,
    BITMAP_PAC_____CLOSE = 1,
    BITMAP_GHOST_____A_R = 2,
    BITMAP_GHOST_____B_R = 3,
    BITMAP_GHOST_____C_R = 4
} bitmap________e;

const uint8_t PROGMEM BITMAP_STORE[5][8] = { {
                                                B00111100,  // PACMAN_______OPEN_R
                                                B01111110,  //
                                                B11111000,  //
                                                B11100000,  //
                                                B11100000,  //
                                                B11111000,  //
                                                B01111110,  //
                                                B00111100   //
                                            },
                                            {
                                                B00111100,  // PACMAN________CLOSE
                                                B01111110,  //
                                                B11111111,  //
                                                B11111111,  //
                                                B11111111,  //
                                                B11111111,  //
                                                B01111110,  //
                                                B00111100   //
                                            },
                                            {
                                                B00111100,  // BITMAP_GHOST_____A_R
                                                B01111110,  //
                                                B11011011,  //
                                                B11001001,  //
                                                B11111111,  //
                                                B11111111,  //
                                                B11111111,  //
                                                B11011011   //
                                            },
                                            {
                                                B00111100,  // BITMAP_GHOST_____B_R
                                                B01111110,  //
                                                B11011011,  //
                                                B11001001,  //
                                                B11111111,  //
                                                B11111111,  //
                                                B11111111,  //
                                                B10110110   //
                                            },
                                            {
                                                B00111100,  // BITMAP_GHOST_____C_R
                                                B01111110,  //
                                                B11011011,  //
                                                B11001001,  //
                                                B11111111,  //
                                                B11111111,  //
                                                B11111111,  //
                                                B01101101   //
                                            } };

typedef enum : int8_t {
    ORIENTATION______UP = -1,  // fingers pointing up, matrix bottom is where the pins are
    ORIENTATION____DOWN = 1    // fingers pointing down, matrix top is where the pins are
} orientation___e;

typedef enum : uint8_t {
    CLEAR_MATRIX_CANVAS = 1,  // clear the internal canvas elements
    CLEAR_MATRIX___DISP = 2   // clear the actual display
} clear_matrix__e;

/**
 * configurable actions
 * configurable can mean through display interaction, bluetooth or both
 */
typedef enum : uint8_t {
    BUTTON_ACTION_MODUS,
    BUTTON_ACTION_DECAY,  // the decay of the frequency bands
    BUTTON_ACTION_LIGHT,
    BUTTON_ACTION_COEFF
} button_action_e;

typedef enum : uint8_t {
    /**
     * show single 4-digit words
     */
    MODUS________WORDS,
    /**
     * show a label running across the screen
     */
    MODUS________LABEL,
    /**
     * show frequency bars
     */
    MODUS________FREQU,
    /**
     * show a single dot moven across the screen
     */
    MODUS________BREAK,
    /**
     * show label for a while then go back to frequency
     */
    MODUS________PARTY,
    /**
     * check for acceleration similarity and display some combined animation in that case, prev modus otherwise
     */
    MODUS________ACCEL,
    /**
     * pacman, only after pairing in accel mode (acceleration similarity)
     */
    MODUS____GAMPM_PRI,
    MODUS____GAMPM_SEC,
    /**
     * game of life, when a significant bump has occured
     */
    MODUS________GAMOL
} modus_________e;

typedef enum : uint8_t {
    TEXT_HALIGN___LEFT,
    TEXT_HALIGN_CENTER,
    TEXT_HALIGN__RIGHT
} text_halign___e;

typedef struct {
    double x;
    double y;
    double z;
} vector________t;

typedef struct {
    uint16_t xMin;
    uint16_t yMin;
    uint16_t xMax;
    uint16_t yMax;
} extent________t;

typedef struct {
    float values[ACCELERATION___SAMPLES];
    int64_t millisWait;
} acceleration__t;

typedef struct {
    /**
     * a reference to a bitmap enum
     */
    bitmap________e bitmap;
    /**
     * the offset at which the bitmap is to be drawn
     * when paired the device will add additional offset to account for left or right
     */
    int8_t offset;
} bitmap________t;

typedef struct {
    bitmap________t bitmapA;      // erasing
    bitmap________t bitmapB;      // drawing
    orientation___e orientation;  // the orientation of the primary device
} bitmaps_______t;

typedef enum : uint8_t {
    /**
     * the device is not paired
     */
    DEVICE_ROLE_____ANY = 0,
    /**
     * the device is paired and has the primary role, meaning it is in charge of sending bitmaps
     */
    DEVICE_ROLE_____PRI = 1,
    /**
     * the device is paired and has the secondary role, meaning it does receive bitmaps
     */
    DEVICE_ROLE_____SEC = 2
} device_role___e;

typedef struct {
    device_role___e deviceRole; // the own role, TODO :: adapt in recv callback
    bitmaps_______t bitmaps;
    acceleration__t acceleration;
} device_data___t;
// const a = sizeof(device_data___t); // 144

#endif