✔️ reply to host

- android app
  ✔ address does not hide when connection is lost
  ✔ generally no further bluetooth scans are done once a device connection is lost
  ✔ brightness slider
  ✔ have the catalog in a central place and reuse
  ✔️ learn how to parse the matchResult, including no-match cases
  ✔️ have continous parsing at specific intervals (i.e. 1 minute)
  ✔️ gatt connection to 2(!) devices, if that does not work maybe one device being server, one being client could be an alternative
  ✔️ find out about message size and how to split texts if needed
  - try to identify code that can fail and handle errors accordingly, read more BLE tutorials
    - when the device disconnects before the gatt-services are in place there is no event or other notification
    - attempting to connect from this state returns the app to a stable state (gatt connect fails and the UI shows the button again after that)
      ✔ handle GATT disconnect (update UI), roughly in place, but needs revisit,
      ✔ a switch disconnect in the app does not disconnect the device!
      ✔ map raw bands to screen/matrix space
      ✔ build some sort of average from the most recent band values
      ✔ display delta from current measurement to average (aka less or more volume than previously)

- esp
  - check any thinkable maximum in terms of device orientation
  ✔ it seems like high frequencies are over-represented in the spectrum
  ✔ check if order 3 regression on frequencies is OK :: seems to work better than order 2
  ✔ separate word sets for left and right
  - have a mode (name?) where after a new label and some timeout it goes back to frequ
    - could be standard, labels do not make much sense without recognition, therefore a new label could just put the decive in temp label mode
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
    - "collab" mode where people can "inject" labels
    - would there be a non-backend solution?
    - enter labels on a public website
    - app fetches label and
    - could this be a simple php solution working with a text or json file for storage?
    - "kitt mode"
    - a single dot travelling left to right and back
    - any way to synchronize left and right?
    - any way to determine when left and right are held in alignment?
      ❌ voice recognition for 4 letter words ;)
      ✔ ignore invalid titles for the runLabel
  - maybe have a "song" mode that will show artist and title for a given time, then go back to frequencies
    ✔ any way to synchronize left and right in word mode?
    ✔ let the android app have the central role in words mode (only if connected)
    ✔ let the android app send pairs of words in regular intervals
    ✔ change words mode so that there always is a target word in words mode and a prepicked set of transition words
    ✔ when not connnected the devices can pick target words themselves, when connected the devices will wait for target words coming in over bluetooth
    ✔ define pairs of opposing words
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

LOVE ICON GAIN HYPE UNDO COOL HOPE KISS MOON FEEL HELP DEAL FACT GAME KIND MIND DARE EARN WELL GROW JUMP KEEP BOLD CALM DEEP EASY GOOD FULL GLAD HIGH NEAR PULL PLUS SAVE FAST LIKE FREE
HERO BEST BODY DEAR FACE FLOW LEAD USER YEAH PEAK FUND LIST NEWS PAIR LIST NOTE SELF BASS FINE BLUE HALF LIVE LONG HOUR LOOK MARK MOVE LIFT GOAL BAND SOFT CARE PURE FUND STAY MORE WARM
LAMB TEAM VIEW REAL HAVE LINK RISE RELY WISH WAIT TALK REST MANY RICH KNOW JOIN RIDE TIDY HUGE HEAL NICE OKAY FAIR NEAT TRUE LIFE

HATE FAIL GANG JUNK FAKE BAIT CAGE SCAR SLOW WOLF PAIN TINY LUSH FIRE FUCK LESS LOSS BURN DENY FALL FEAR HURT KILL NONE EVIL DULL FOUL GRIM DAMN GORE GOSH HELL LATE DUST DARK DUTY RISK
WARN SLIP UGLY WILD VAIN POOR MEAN RUDE SORE VOID ANTI DIRE COPE COST PUSH MYTH URGE RAIN REDO NEED SEEM LONE VAST PALE SICK RARE BEEF FOOL DEAF DRUG ITCH PITY LACK WANT SEEK TRIP DOWN
GREY RULE WORK TASK STOP LOUD MUST GIVE LOSE TORN ONLY SOLO TRAP FLOP QUIT SUNK HOWL ENVY CYST COLD DROP BASE FLEE OVER HARD PASS

https://www.powerthesaurus.org/huge/antonyms

STEW SOUP
WOKE MAGA

LOVE HATE
REAL FAKE
WARM COLD
MANY NONE
FAST SLOW
KIND GRIM
GOOD EVIL
KEEP GIVE
WELL SICK
HOPE FEAR
FACT MYTH
RISE FALL
MORE LESS
MOVE STOP
REST WORK
HEAL HURT
LIFT DROP
SOFT HARD
CALM LOUD
RICH POOR
PURE FOUL
NICE MEAN
PEAK BASE
PULL PUSH
LAMB WOLF
HUGE TINY
FULL VOID
STAY FLEE
UNDO REDO
LIVE PASS
EARN LOSE
GAIN LOSS

---

- Lackstift gelb für Kabelmarkierungen @ müller
  - punkt bei SCL auf der LED Matrix
  - kurzes stück gelber schraumpfschlauch am SCL kabel, beidseitig
  - und / oder dupont stecker auf der SCL seite gelb markiert
