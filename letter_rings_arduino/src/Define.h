#define DEVICE____________LEFT false

#if DEVICE____________LEFT == true
#define COMMAND_SERVICE___UUID "791320d5-7f0a-4b58-89f6-cc2031479da5"
#define DEVICE____________SIDE "L"
#define DEVICE____________NAME "LETTER_RINGS_L"
#define ACCEPT__ROLE_DOWNGRADE false
#else
#define COMMAND_SERVICE___UUID "d0e5a78d-614b-4f8c-86cd-e782d832228f"
#define DEVICE____________SIDE "R"
#define DEVICE____________NAME "LETTER_RINGS_R"
#define ACCEPT__ROLE_DOWNGRADE true
#endif

#define USE__FORCE_______GAMXX false // when true (and mode ACCEL) mode GAMPM will be active, regardless of device pairing
#define USE__________CLIP_DRAW false // draw red clip areas on display
#define USE__FORCE_BACKLITE_ON false // when true, the display backlite will never turn off, even when the display is inactive
#define USE_SERIAL_LOOP_OUTPUT false // measure loop duration and write to console
#define USE_SERIAL__MIC_OUTPUT false // write microphone related messages (basis, scale) to the console
#define USE_SERIAL_SYNC_OUTPUT false // write sync related messages (send, revc, millis) to the console

#define COMMAND_LABEL_____UUID "067c3c93-eb63-4905-b292-478642f8ae99"  // for remote writing a label (moving over multiple matrices)
#define COMMAND_WORD______UUID "3dfde050-8432-4f2f-9366-de27c430c05c"  // for remote writing a word (static 4-letter word)
#define COMMAND_MODUS_____UUID "a8792ef9-c01c-47ee-a063-f8712bf8dd67"  // for remote reading/writing the modus
#define COMMAND_LIGHT_____UUID "28c64d42-4958-43f0-8845-5a106498bf1d"  // for remote reading/writing the light
#define COMMAND_COEFP_____UUID "cfb68865-9243-4646-af27-750d02d44a3a"  // for remote reading/writing the coefP (pairing coefficient)

#define AUDIO__________SAMPLES 512    // Must be a power of 2
#define AUDIO____SAMPLING_FREQ 40000  // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AUDIO________NUM_BANDS 16
#define AUDIO________NUM_ORDER 3
#define AUDIO________MAX_SCALE 0.0015 // a bit louder than silence (PC fan and else) which yields 0.002
#define AUDIO_BASE_SCALE_DECAY 0.9999
#define AUDIO_PEAK_______DECAY 0.96
#define AUDIO_FIT________DECAY 0.001

#define BUTTON_DEBOUNCE_MILLIS 100

#define ACCELERATION___SAMPLES 32  // ~ 1.5 seconds at a sample rate of 50ms, maybe half of that would do as well
#define COEFF________THRES_MIN 0.6
#define COEFF________THRES_MAX 0.9
#define ACCELERATION_SIG_THRES 7.0
#define ACCELERATION_TAP_THRES 5.0

#define CHARS______________NUM 6
#define CHARS______FIELD_DIM_X 40
#define CHARS______FIELD_DIM_Y 10

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
static const uint8_t STA_ADDRESS_OUT[] = { 0x64, 0xE8, 0x33, 0x73, 0xF1, 0x48 }; // sta address of right device
static const int8_t BITMAPS_OFF = 1;                                             // 1 * ORIENTATION * 16 + 16 -> 0 when UP, 32 when DOWN, 1 * -2 + 2 -> 0
#else
static const char* WORDS[] PROGMEM = { "HATE", "FAIL", "GANG", "JUNK", "FAKE", "BAIT", "CAGE", "SCAR", "SLOW", "WOLF", "PAIN", "TINY", "LUSH", "FIRE", "FUCK", "LESS", "LOSS", "BURN", "DENY", "FALL", "FEAR", "HURT", "KILL", "NONE", "EVIL", "DULL", "FOUL", "GRIM", "DAMN", "GORE", "GOSH", "HELL", "LATE", "DUST", "DARK", "DUTY", "RISK", "WARN", "SLIP", "UGLY", "WILD", "VAIN", "POOR", "MEAN", "RUDE", "SORE", "VOID", "ANTI", "DIRE", "COPE",
                                      "COST", "PUSH", "MYTH", "URGE", "RAIN", "REDO", "NEED", "SEEM", "LONE", "VAST", "PALE", "SICK", "RARE", "BEEF", "FOOL", "DEAF", "DRUG", "ITCH", "PITY", "LACK", "WANT", "SEEK", "TRIP", "DOWN", "GREY", "RULE", "WORK", "TASK", "STOP", "LOUD", "MUST", "GIVE", "LOSE", "TORN", "ONLY", "SOLO", "TRAP", "FLOP", "QUIT", "SUNK", "HOWL", "ENVY", "CYST", "COLD", "DROP", "BASE", "FLEE", "OVER", "HARD", "PASS" };
static const uint8_t WORD_COUNT = 100;
static const uint8_t STA_ADDRESS_OUT[] = { 0xD0, 0xCF, 0x13, 0x0A, 0xE1, 0xC8 }; // sta address of left device
static const int8_t BITMAPS_OFF = -1; // -1 * ORIENTATION * 16 + 16 -> 32 when UP, 0 when DOWN, -1 * -2 + 2 -> 4
static const gpio_num_t TOUCH______________PIN = GPIO_NUM_12; // only on the right device
static const uint32_t   TOUCH________THRESHOLD = 100000;
#endif

const gpio_num_t AUDIO______________PIN = GPIO_NUM_8;  // A5


const uint64_t ROLE_PRI____DURATION_MS = 15000;
const uint64_t WORD_UPDATE_INTERVAL_MS = 14000;
const uint64_t PARTY_LABEL_DURATION_MS = 20000;
const uint64_t DISP_ACTIVE_DURATION_MS = 1000 * 60 * 3; // 3 minutes
const uint64_t GAMOL_______DURATION_MS = 10000;
const uint8_t  WORD_____TRANSITION_NUM = 15; // the number of random words when transitioning from one word to another

/**
 * the interval we want the primary loop to run at (50ms -> 20 times/second)
 */
const uint64_t MAIN_LOOP_______DEST_MS = 50;

typedef enum : uint8_t {
    BITMAP_PAC____OPEN_R = 0,
    BITMAP_PAC_____CLOSE = 1,
    BITMAP_________EMPTY = 2
} bitmap________e;



const uint8_t PROGMEM BITMAP_STORE[3][8] = { {
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
                                                B00000000,  // BITMAP_________EMPTY
                                                B00000000,  //
                                                B00000000,  //
                                                B00000000,  //
                                                B00000000,  //
                                                B00000000,  //
                                                B00000000,  //
                                                B00000000   //
                                            } };

typedef enum : int8_t {
    ORIENTATION______UP = -1,  // fingers pointing up, matrix bottom is where the pins are
    ORIENTATION____DOWN = 1    // fingers pointing down, matrix top is where the pins are
} orientation___e;

typedef enum : uint8_t {
    CLEAR_MATRIX_CANVAS = 1,  // clear the internal canvas elements
    CLEAR_MATRIX___DISP = 2   // clear the actual display
    // next value would have to be 4
} clear_matrix__e;

/**
 * configurable actions
 * configurable can mean through display interaction, bluetooth or both
 */
typedef enum : uint8_t {
    BUTTON_ACTION_MODUS,
    BUTTON_ACTION_DECAY,  // the decay of the frequency bands
    BUTTON_ACTION_LIGHT,
    BUTTON_ACTION_COEFP
} button_action_e;

typedef enum : uint8_t {
    /**
     * show the chars of my first name
     */
    MODUS________CHARS,
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
     * pacman, only after pairing in accel mode (magnitude similarity)
     */
    MODUS____GAMPM_PRI,
    MODUS____GAMPM_SEC,
    /**
     * game of life, after a significant bump has occured
     */
    MODUS________GAMOL,
    /**
     * kitt anim, only after specific interaction, TODO :: define interaction (could be touch sensor on the right device, activated in a "look at watch" gesture)
     */
    MODUS____GAMKN_PRI,
    MODUS____GAMKN_SEC
} modus_________e;

typedef enum : uint8_t {
    DTERM_________NONE,
    DTERM________GAMPM,
    DTERM________GAMKN
} dterm_________e;

typedef enum : uint8_t {
    TEXT_HALIGN___LEFT,
    TEXT_HALIGN_CENTER,
    TEXT_HALIGN__RIGHT
} text_halign___e;

typedef struct {
    float x;
    float y;
    float z;
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
} magnitudes___t;

typedef struct {
    float valuesX[ACCELERATION___SAMPLES];
    float valuesY[ACCELERATION___SAMPLES];
    float valuesZ[ACCELERATION___SAMPLES];
} acceleration_t;

const acceleration_t ACCEL_W = {
    { 0.440, 0.483, 0.379,-0.041,-0.996,-1.680,-1.183,-0.124, 0.996, 1.183, 0.389,-0.269,-1.036,-1.629,-1.996,-1.447, 0.397, 2.007, 2.369, 0.377,-0.866,-1.026},
    {-0.434,-0.673,-0.814,-0.149, 1.239, 2.611, 1.363,-0.093,-0.897,-1.284,-0.057,-0.390, 0.659, 1.859, 2.084, 0.684, 0.244,-1.483,-0.749, 0.310,-0.021,-0.187},
    {-0.307,-0.298,-0.190, 0.009, 0.444, 0.486, 0.041,-0.011,-0.083,-0.456,-0.030,-0.320, 0.019, 0.293, 0.149, 0.370,-0.034,-0.757,-1.081, 0.143, 0.266, 0.016}
};
const acceleration_t ACCEL_C = {
    { 0.003, 0.214, 0.294, 0.444, 0.581, 0.810, 1.061, 1.270, 1.534, 1.699, 1.494, 0.830,-0.419,-2.227,-3.739,-3.900,-2.744,-0.947, 0.927, 2.294, 1.539, 0.670},
    { 0.269, 0.183, 0.046,-0.011, 0.220, 0.177, 0.174, 0.196, 0.287,-0.361,-0.673,-1.554,-1.976,-1.270,-0.139, 1.467, 1.816, 1.764, 1.211, 0.196,-1.329,-0.481},
    {-0.067,-0.001, 0.051, 0.116, 0.154,-0.086,-0.124,-0.459,-0.254,-0.389,-0.359,-0.156, 0.116, 0.893, 0.790, 0.869, 0.927, 0.054,-0.694,-1.550,-0.927,-0.179}
};
const acceleration_t ACCEL_F = {
    { 1.336, 3.204, 3.423, 2.484, 1.239, 0.240,-0.313,-0.159, 0.063, 0.219,-0.086,-1.160,-2.257,-3.403,-3.354,-1.530, 1.721, 3.393, 3.159, 0.763,-0.394,-0.520},
    { 0.279, 0.294,-0.151,-0.756,-0.839,-1.220,-0.750,-0.373, 0.696, 1.076, 1.341, 1.504, 1.461,-0.214,-0.119,-0.004,-0.687,-0.404,-0.690,-0.246,-0.350,-0.273},
    {-0.809,-0.440,-0.331, 0.190, 0.596, 0.860, 0.681, 0.001,-0.793,-1.359,-1.141,-0.566, 0.217, 0.719, 1.100, 0.939,-1.279,-1.059,-0.997, 0.429, 0.537, 0.101}
};

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
    bitmap________t bitmapC;      // reserve
    bitmap________t bitmapD;      // reserve
    orientation___e orientation;  // the orientation of the primary device
} bitmaps_______t;
// const a = sizeof(bitmaps_______t); // 5

typedef struct {
    // uint8_t values[8];
    int64_t offset;
    uint16_t padding;
} ledbar________t;
// const a = sizeof(ledbar________t); // 8

typedef struct {
    char character;
    vector________t position;
    vector________t velocity;
    int8_t matrixX;
    int8_t matrixY;
} char__________t;

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
    device_role___e deviceRole;
    magnitudes___t magnitudes;
} device_data___t;
// const a = sizeof(device_data___t); // 144

typedef struct {
    device_role___e deviceRole;
    ledbar________t ledbar;
    magnitudes___t magnitudes;
} device_data_l_t;
// const b = sizeof(device_data_l_t); // 160

typedef struct {
    device_role___e deviceRole;
    bitmaps_______t bitmaps;
    magnitudes___t magnitudes;
} device_data_b_t;
// const c = sizeof(device_data_b_t); // 152

#endif