/*
   Shared Game Mode Across All Blinks
   This can work for as many different game modes as you need
   Switching between them simply requires a call to changeMode()
   The signalState takes care of making sure all other Blinks change too
*/

enum signalStates {INERT, GO, RESOLVE};
byte signalState = INERT;

enum gameModes {MODE1, MODE2, MODE3};//these modes will simply be different colors
byte gameMode = MODE1;//the default mode when the game begins

//time values for delta
uint32_t timeOfSend = 0;
uint32_t timeOfPress = 0; // use this to show the button was pressed in connect mode

//Colors
Color SILVER_ORANGE = makeColorHSB(30, 255, 255); //sun ppot
Color SILVER_CYAN = makeColorHSB(120, 255, 0);  //lining

//hues
byte sunSpot_hue = 30;
byte lining_hue = 120;
byte cloud_hue = 0 ;


//lining Fade
#define SPARKLE_OFFSET          80
#define SPARKLE_DURATION        800
#define SPARKLE_CYCLE_DURATION  1600

bool win = false;
//sun spot fade
#define SETUP_FADE_UP_INTERVAL 1000
#define SETUP_RED_INTERVAL 500
#define SETUP_FADE_DELAY 4000
byte setupFadeFace;
Timer setupFadeTimer;
word backgroundTime;


#define FADE_TO_WIN 500

Timer fadeToBright;
#define FADE_TO_BRIGHT_DELAY 2000

Timer fadeToCloud;
#define FADE_TO_CLOUD_DELAY 3000

Timer sunSpotFade;
#define SUN_SPOT_FADE 1000

Timer fadeToDark;
#define FADE_TO_DARK_DELAY 1000

enum modeStates {LOSE, WIN};
byte currentMode;


void setup() {
  randomize();
}

void loop() {

  // The following listens for and updates game state across all Blinks
  switch (signalState) {
    case INERT:
      inertLoop();
      break;
    case GO:
      goLoop();
      break;
    case RESOLVE:
      resolveLoop();
      break;
  }

  // The following is loops for each of our game states
  switch (gameMode) {
    case MODE1:
      mode1Loop();
      break;
    case MODE2:
      mode2Loop();
      break;
    case MODE3:
      mode3Loop();
      break;
  }

  // communicate with neighbors
  // share both signalState (i.e. when to change) and the game mode
  byte sendData = (signalState << 2) + (gameMode);
  setValueSentOnAllFaces(sendData);
}

/*
   Mode 1
*/
void mode1Loop() {

  if (buttonSingleClicked()) {
    changeMode(MODE2);
  }

  setColor(WHITE);
}

/*
   Mode 2
*/
void mode2Loop() {

  if (buttonSingleClicked()) {
    changeMode(MODE1);
  }

  setColor(WHITE);
  fadeToNoLight();
  if (fadeToDark.isExpired()) {
    silverLiningDisplay();
  }
}

/*
   Mode 3
*/
void mode3Loop() {

  if (buttonSingleClicked()) {
    changeMode(MODE1);
  }

  setColor(BLUE);
}


/*
   pass this a game mode to switch to
*/
void changeMode( byte mode ) {
  if (mode == MODE2) {
    setupFadeTimer.set(backgroundTime + SETUP_FADE_UP_INTERVAL + random(SETUP_FADE_DELAY));
    fadeToBright.set(FADE_TO_BRIGHT_DELAY);
    fadeToDark.set(FADE_TO_DARK_DELAY);
    fadeToCloud.set(FADE_TO_CLOUD_DELAY);
  }
  gameMode = mode;  // change my own mode
  signalState = GO; // signal my neighbors
}


/*
   This loop looks for a GO signalState
   Also gets the new gameMode
*/
void inertLoop() {

  //listen for neighbors in GO
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getSignalState(getLastValueReceivedOnFace(f)) == GO) {//a neighbor saying GO!
        signalState = GO;
        changeMode(getGameMode(getLastValueReceivedOnFace(f)));
      }
    }
  }
}

/*
   If all of my neighbors are in GO or RESOLVE, then I can RESOLVE
*/
void goLoop() {
  signalState = RESOLVE;//I default to this at the start of the loop. Only if I see a problem does this not happen

  //look for neighbors who have not heard the GO news
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getSignalState(getLastValueReceivedOnFace(f)) == INERT) {//This neighbor doesn't know it's GO time. Stay in GO
        signalState = GO;
      }
    }
  }
}

/*
   This loop returns me to inert once everyone around me has RESOLVED
   Now receive the game mode
*/
void resolveLoop() {
  signalState = INERT;//I default to this at the start of the loop. Only if I see a problem does this not happen

  //look for neighbors who have not moved to RESOLVE
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getSignalState(getLastValueReceivedOnFace(f)) == GO) {//This neighbor isn't in RESOLVE. Stay in RESOLVE
        signalState = RESOLVE;
      }
    }
  }
}

void fadeToLight() {

  uint32_t delta = millis() - timeOfSend;


  FOREACH_FACE(f) {
    // minimum of 125, maximum of 255
    byte phaseShift = 60 * f;
    byte amplitude = 55;
    byte midline = 185;
    byte rate = 6;
    byte brightness = midline + (amplitude * sin8_C ( (phaseShift + millis() / rate) % 255)) / 255;
    byte saturation = 255 -  map(fadeToBright.getRemaining(), 0, FADE_TO_BRIGHT_DELAY, 0, 255);
    Color faceColor = makeColorHSB(0, 0, saturation);
    setColorOnFace(faceColor, f);
  }
}


void fadeToNoLight() {

  uint32_t delta = millis() - timeOfSend;


  FOREACH_FACE(f) {
    // minimum of 125, maximum of 255
    byte phaseShift = 60 * f;
    byte amplitude = 55;
    byte midline = 185;
    byte rate = 6;
    byte brightness = midline + (amplitude * sin8_C ( (phaseShift + millis() / rate) % 255)) / 255;
    byte saturation = map(fadeToDark.getRemaining(), 0, FADE_TO_DARK_DELAY, 0, 255);
    Color faceColor = makeColorHSB(0, 0, saturation);
    setColorOnFace(faceColor, f);
  }
}

void silverLiningDisplay() {
  Color faceColor_lining;
  Color faceColor_cloud;
  uint32_t delta = millis() - timeOfSend;


  if (delta > 300) {
    delta = 300;
  }

  FOREACH_FACE(f) {
    // minimum of 125, maximum of 255
    byte phaseShift = 60 * f;
    byte amplitude = 55;
    byte midline = 185;
    byte rate = 10;
    byte brightness = midline + (amplitude * sin8_C ( (phaseShift + millis() / rate) % 255)) / 255;
    byte saturation = 255 -  map(fadeToBright.getRemaining(), 0, FADE_TO_BRIGHT_DELAY, 0, 255);
     byte cloudSaturation = 255 -  map(fadeToCloud.getRemaining(), 0, FADE_TO_CLOUD_DELAY, 0, 255);
    if (!fadeToCloud.isExpired()) {

       faceColor_lining = makeColorHSB(0, 0, saturation);
      faceColor_cloud = makeColorHSB(160, saturation, cloudSaturation);

    }

    if (fadeToBright.isExpired()) {
      faceColor_lining = makeColorHSB(0, 0, 255);
      faceColor_cloud = makeColorHSB(160, 0, 255);

    }
    silverLining(faceColor_cloud, faceColor_lining);
  }
}




void silverLining(Color faceColor_Cloud, Color faceColor_Lining) {


  //setColor(faceColor_Cloud);
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { //if something there aka if within the cloud


      // have the color on the Blink raise and lower to feel more alive
      byte bri = 185 + sin8_C( (millis() / 14) % 255) * 70 / 255; // oscillate between values 185and 255


      // lets do a celebration on each face in an order
      word delta = millis() % SPARKLE_CYCLE_DURATION; // 2 second cycle

      if (delta > SPARKLE_DURATION) {
        delta = SPARKLE_DURATION;
      }


      if (setupFadeTimer.isExpired()) {
        setupFadeFace = f;
        backgroundTime = SETUP_RED_INTERVAL + random(SETUP_RED_INTERVAL / 2);
        setupFadeTimer.set(backgroundTime + SETUP_FADE_UP_INTERVAL + random(SETUP_FADE_DELAY));
      }
      Color fadeColor;
      byte saturation;
      byte hue;

      if (setupFadeTimer.getRemaining() < backgroundTime + SETUP_FADE_UP_INTERVAL) {//we are inside the animation
        if (setupFadeTimer.getRemaining() < SETUP_FADE_UP_INTERVAL) {//we are fading from white
          saturation = 255 - map(setupFadeTimer.getRemaining(), 0, SETUP_FADE_UP_INTERVAL, 0, 255);
          fadeColor = makeColorHSB(160, saturation, bri);
        } else {
          sunSpotFade.set(SUN_SPOT_FADE);
          if (!sunSpotFade.isExpired()) {
            saturation =  map(sunSpotFade.getRemaining(), SUN_SPOT_FADE, 0, 0, 255);

            fadeColor = makeColorHSB(sunSpot_hue, saturation, bri);
          }

        }

        setColorOnFace(fadeColor, setupFadeFace);
      }

      fadeColor = makeColorHSB(160, 255, bri);
      setColorOnFace(fadeColor, f);
    }


    else {
      setColorOnFace(faceColor_Lining, f); // if not within the cloud, display the lining color

    }

  }
}
byte getGameMode(byte data) {
  return (data & 3);//returns bits E and F
}

byte getSignalState(byte data) {
  return ((data >> 2) & 3);//returns bits C and D
}
