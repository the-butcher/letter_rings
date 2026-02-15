✔️ reply to host

- android app
  - brightness slider
    ✔ have the catalog in a central place and reuse
    ✔️ learn how to parse the matchResult, including no-match cases
    ✔️ have continous parsing at specific intervals (i.e. 1 minute)
    ✔️ gatt connection to 2(!) devices, if that does not work maybe one device being server, one being client could be an alternative
    ✔️ find out about message size and how to split texts if needed
  - try to identify code that can fail and handle errors accordingly, read more BLE tutorials
    ✔ handle GATT disconnect (update UI), roughly in place, but needs revisit,
    ✔ a switch disconnect in the app does not disconnect the device!
    ✔ map raw bands to screen/matrix space
    ✔ build some sort of average from the most recent band values
    ✔ display delta from current measurement to average (aka less or more volume than previously)

- esp
  - any mode where altering brightness would add value?
    ✔ have proper matrix UP/DOWN flip in all modes
    ✔ unify method names Matrices vs. Matrix
    ✔ clearXXX function, execute properly after modus switch
  - dim TFT a while after the last button press https://forums.adafruit.com/viewtopic.php?t=174096
  - test with high volume and noise as good as possible, experiment with gain settings on the microphone
  - be able to switch modes
    ✔ through display
  - through gesture (?)
    ✔ through bluetooth / mobile
  - any other attractive modes?
    - voice recognition for 4 letter words ;)
  - ignore invalid titles for the runLabel
  - maybe have a "song" mode that will show artist and title for a given time, then go back to frequencies
  - any way to synchronize left and right in word mode?
    ✔ initial values of radio buttons after connect
    ✔ create a left UI version (?)
    ✔ screen could be oriented "upwards" to have an orientation common to left and right
    ✔ take care of readability (no font size = 1)
    ✔ byte to string parsing in the ESP32
    ✔️ microphone POC in the ESP32, preferrably amplifier style, but could also be led matrix brightness
    ✔ validate possibility of multiple led matrices on one i2c line
    ❌ could there be a generic blend function for labels?
    ❌ how would different lengths of labels be handled?
    ❌ original label longer than new label
    ❌ example:
    ❌ this is a very very very long label
    ❌ short label
    ❌ original label shorter than new label
    ✔ create some blend magic from one character to another
    ✔ could maybe work with offscreen canvas
    ✔ look at canvas.getBuffer()
    ✔ maybe a few random sequences including the number 0-63 and gradually changing from src to target values would do the trick
    ✔ check if there is a shuffle method for arrays in arduino
    ❌ rotate display upon hand position using blend effect
    ❌ current buffers of A and D (and B and C) would have to be swapped and rotated

- pcb
  ✔ order pcb after multi-matrix i2c validation

LOVE ICON VIBE HYPE CHIC COOL HOPE KISS MOON FEEL HELP DEAL FACT GAME KIND MIND DARE EARN GIVE GROW JUMP KEEP BOLD CALM DEEP EASY GOOD FULL GLAD HIGH NEAR ONCE PLUS SAVE FAST LIKE FREE HERO BEST BODY DEAR FACE FLOW LEAD USER YEAH BASE FUND LIST NEWS PAIR LIST NOTE SELF BASS FINE BLUE HALF LIVE LONG HOUR LOOK MARK MOVE EAST GOAL BAND TOUR CARE FILE FUND ROAD TEST UNIT CALL TEAM VIEW TURN HAVE LINK RISE RELY WISH WAIT TALK REST PICK LEND KNOW JOIN RIDE TIDY TALL REAR NICE OKAY FAIR NEAT TRUE LIFE

HATE FAIL GANG JUNK FAKE BAIT CAGE SCAR THUG PIMP PAIN PULP LUSH FIRE FUCK LESS LOSS BURN DENY FALL FEAR HURT KILL NONE EVIL DULL FOUL GRIM DAMN GORE GOSH HELL LATE DUST DARK DUTY RISK WARN SLIP UGLY WILD VAIN POOR MEAN RUDE SORE ZERO ANTI DIRE COPE COST BANG OUCH URGE RAIN ARMY NEED SEEM LONE VAST PALE LAZY RARE BEEF FOOL DEAF DRUG ITCH PITY LACK WANT SEEK TRIP DOWN GREY RULE WORK TASK STOP MUCH MUST AMMO NAIL TORN ONLY SOLO TRAP FLOP QUIT SUNK HOWL ENVY CYST MINE MORE RACE GLUE OVER HARD BEAT

---
