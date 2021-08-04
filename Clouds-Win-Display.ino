uint32_t timeOfSend = 0;
uint32_t timeOfPress = 0; // use this to show the button was pressed in connect mode

Color SILVER_ORANGE = makeColorHSB(30, 255, 255);
Color SILVER_CYAN = makeColorHSB(120, 255, 255);

byte sunSpot_hue = 30;
byte lining_hue = 120;

byte sparkleOffset[6] = {0, 3, 5, 1, 4, 2};

byte neighbors[6] = {0, 0, 0, 0, 0, 0}; // 0 is no one here

Timer animTimer;

#define ANIM_FADE 100;
#define SPARKLE_OFFSET 600;
#define SPARKLE_DURATION 600;
#define SPARKLE_CYCLE_DURATION 200;

void setup() {


}

void loop() {


  silverLiningDisplay();


  // put your main code here, to run repeatedly:

  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { //something there
      neighbors[f] = 1;
    }
    else {
      neighbors[f] = 0;
    }
  }
}



void silverLiningDisplay() {
  // go full white and then fade to new color
  uint32_t delta = millis() - timeOfSend;


  if (delta > 300) {
    delta = 300;
  }

  FOREACH_FACE(f) {
    // minimum of 125, maximum of 255
    byte phaseShift = 60 * f;
    byte amplitude = 55;
    byte midline = 185;
    byte rate = 4;
    byte brightness = midline + (amplitude * sin8_C ( (phaseShift + millis() / rate) % 255)) / 255;
    byte saturation = map(delta, 0, 300, 0, 255);

    Color faceColor_lining = makeColorHSB(120, saturation, brightness);
    Color faceColor_cloud = WHITE;
    silverLining(faceColor_cloud, faceColor_lining);
  }
}

bool randomTrueOrFalse() {
  byte randTrueOrFalse = random(1); // random value 0 or 1
  if (randTrueOrFalse == 0) {
    return false;
  }
  else {
    return true;
  }
}

void silverLining(Color faceColor_Cloud, Color faceColor_Lining) {
    animTimer.set( 500 );
  if (!animTimer.isExpired()) {
    FOREACH_FACE(f) {
      if (!isValueReceivedOnFaceExpired(f)) { //if something there
        if (neighbors[f] == 0) {

          byte sat = 255 - (255 * animTimer.getRemaining() ) / 500;
          setColorOnFace(makeColorHSB(30,sat, 255),f);
        }
      }


      else {
        setColorOnFace(faceColor_Lining, f);

      }
    }
  }

}
