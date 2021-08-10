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


//sun spot fade
#define SETUP_FADE_UP_INTERVAL 1000
#define SETUP_RED_INTERVAL 500
#define SETUP_FADE_DELAY 4000
byte setupFadeFace;
Timer setupFadeTimer;
word backgroundTime;

void setup() {

  // put your setup code here, to run once:
  randomize();

}

void loop() {
  silverLiningDisplay();

}



void silverLiningDisplay() {

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
    byte saturation = map(delta, 0, 300, 0, 255);
    Color faceColor_lining = makeColorHSB(0, 0, brightness);
    Color faceColor_cloud = makeColorHSB(160, saturation, 255);
    silverLining(faceColor_cloud, faceColor_lining);
  }
}



void silverLining(Color faceColor_Cloud, Color faceColor_Lining) {

  Color fadeColor;
  byte saturation;

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
        setupFadeFace = (setupFadeFace + random(f)) % f; //choose faces within clouds but sometimes exit
        backgroundTime = SETUP_RED_INTERVAL + random(SETUP_RED_INTERVAL / 2);
        setupFadeTimer.set(backgroundTime + SETUP_FADE_UP_INTERVAL + random(SETUP_FADE_DELAY));
      }


      if (setupFadeTimer.getRemaining() < backgroundTime + SETUP_FADE_UP_INTERVAL) {//we are inside the animation
        if (setupFadeTimer.getRemaining() < SETUP_FADE_UP_INTERVAL) {//we are fading from white
          saturation = 255 - map(setupFadeTimer.getRemaining(), 0, SETUP_FADE_UP_INTERVAL, 0, 255);
          fadeColor = makeColorHSB(sunSpot_hue, saturation, 255);  //sun spots
        } else {//we are clouds
          fadeColor = makeColorHSB(0, 0, bri);
        }

        setColorOnFace(fadeColor, setupFadeFace);
      }
      Color faceColor = makeColorHSB(160, 150, bri);
      setColorOnFace(faceColor, f);
    }


    else {
      setColorOnFace(faceColor_Lining, f); // if not within the cloud, display the lining color

    }

  }
}
