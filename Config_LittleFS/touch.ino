// calibration data. Change the name to start a new calibration.
#define CALIBRATION_FILE "/TouchCalData3"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false
//#define BLACK_SPOT

// Switch position and size
#define FRAME_X 30
#define FRAME_Y 265
#define FRAME_W 120
#define FRAME_H 50

// Red zone size
#define REDBUTTON_X FRAME_X
#define REDBUTTON_Y FRAME_Y
#define REDBUTTON_W (FRAME_W/2)
#define REDBUTTON_H FRAME_H

// Green zone size
#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
#define GREENBUTTON_Y FRAME_Y
#define GREENBUTTON_W (FRAME_W/2)
#define GREENBUTTON_H FRAME_H

bool SwitchOn = false;
ButtonWidget btnL = ButtonWidget(&tft);
ButtonWidget btnR = ButtonWidget(&tft);
ButtonWidget btnS = ButtonWidget(&tft);

#define BUTTON_W 70
#define BUTTON_H 30

// Create an array of button instances to use in for() loops
// This is more useful where large numbers of buttons are employed
ButtonWidget* btn[] = {&btnL , &btnR, &btnS};;
uint8_t buttonCount = sizeof(btn) / sizeof(btn[0]);

void btnL_pressAction(void)
{
  if (btnL.justPressed()) {
    Serial.println("Info button just pressed");
    btnL.drawSmoothButton(true);
    btnR.drawSmoothButton(false);
    btnS.drawSmoothButton(false);
  }
}

void btnL_releaseAction(void)
{

}

void btnR_pressAction(void)
{
  if (btnR.justPressed()) {
    Serial.println("Main button just pressed");
    btnL.drawSmoothButton(false);
    btnR.drawSmoothButton(true);
    btnS.drawSmoothButton(false);
  }
}

void btnR_releaseAction(void)
{
  // Not action
}

void btnS_pressAction(void)
{
  if (btnS.justPressed()) {
    Serial.println("Main button just pressed");
    btnL.drawSmoothButton(false);
    btnR.drawSmoothButton(false);
    btnS.drawSmoothButton(true);
  }
  }

void btnS_releaseAction(void)
{
}


void initButtons() {
  btnL.initButtonUL(10,285, BUTTON_W, BUTTON_H, TFT_DARKGREEN, TFT_DARKGREEN , TFT_GREEN, "INFO", 1);
  btnL.setPressAction(btnL_pressAction);
 // btnL.setReleaseAction(btnL_releaseAction);
  btnL.drawSmoothButton(true, 0, TFT_GREEN); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

  btnR.initButtonUL(85, 285, BUTTON_W, BUTTON_H, TFT_DARKGREEN, TFT_DARKGREEN , TFT_GREEN, "MAIN", 1);
  btnR.setPressAction(btnR_pressAction);
  //btnR.setReleaseAction(btnR_releaseAction);
  btnR.drawSmoothButton(false, 0, TFT_GREEN); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

  btnS.initButtonUL(160, 285, BUTTON_W, BUTTON_H, TFT_DARKGREEN, TFT_DARKGREEN , TFT_GREEN, "SETUP", 1);
  btnS.setPressAction(btnS_pressAction);
  //btnR.setReleaseAction(btnS_releaseAction);
  btnS.drawSmoothButton(false, 0, TFT_GREEN); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing
}


bool initTOUCH() {


touch_calibrate();

  return false;
  }


void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;



  // check if calibration file exists and size is correct
  if (LittleFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      LittleFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = LittleFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = LittleFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}


void drawFrame()
{
  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, TFT_BLACK);
}

void checkTouchloop()
{
  uint16_t x, y;

  // See if there's any touch data for us
  if (tft.getTouch(&x, &y))
  {
    // Draw a block spot to show where touch was calculated to be
    #ifdef BLACK_SPOT
      tft.fillCircle(x, y, 2, TFT_BLACK);
    #endif
    
    bool pressed = tft.getTouch(&t_x, &t_y);
    for (uint8_t b = 0; b < buttonCount; b++) {
      if (pressed) {
        if (btn[b]->contains(t_x, t_y)) {
          btn[b]->press(true);
          btn[b]->pressAction();
        }
      }
      else {
        btn[b]->press(false);
        btn[b]->releaseAction();
      }
    }

   // Serial.println(SwitchOn);

  }
}
