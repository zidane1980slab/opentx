/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define LANGUAGE_PACKS_DEFINITION

#include "opentx.h"

#define RADIO_SETUP_2ND_COLUMN         220
#define YEAR_SEPARATOR_OFFSET          42
#define MONTH_OFFSET                   55
#define MONTH_SEPARATOR_OFFSET         79
#define DAY_OFFSET                     91
#define HOUR_SEPARATOR_OFFSET          26
#define MINUTE_OFFSET                  36
#define MINUTE_SEPARATOR_OFFSET        63
#define SECOND_OFFSET                  75

int8_t editSlider(coord_t x, coord_t y, evt_t event, int8_t value, int8_t min, int8_t max, LcdFlags attr)
{
  drawHorizontalSlider(x, y, 100, value, min, max, 0, OPTION_SLIDER_DBL_COLOR|attr);
  return selectMenuItem(x, y, NULL, value, min, max, attr, event);
}

#define SLIDER_5POS(y, val, event, attr) val = editSlider(RADIO_SETUP_2ND_COLUMN, y, event, val, -2, +2, attr)

#if defined(SPLASH) && !defined(FSPLASH)
  #define CASE_SPLASH_PARAM(x) x,
#else
  #define CASE_SPLASH_PARAM(x)
#endif

enum menuGeneralSetupItems {
  ITEM_SETUP_DATE,
  ITEM_SETUP_TIME,
  ITEM_SETUP_BATT_RANGE,
  ITEM_SETUP_SOUND_LABEL,
  ITEM_SETUP_BEEP_MODE,
  ITEM_SETUP_GENERAL_VOLUME,
  ITEM_SETUP_BEEP_VOLUME,
  ITEM_SETUP_BEEP_LENGTH,
  ITEM_SETUP_SPEAKER_PITCH,
  ITEM_SETUP_WAV_VOLUME,
  ITEM_SETUP_BACKGROUND_VOLUME,
  CASE_VARIO(ITEM_SETUP_VARIO_LABEL)
  CASE_VARIO(ITEM_SETUP_VARIO_VOLUME)
  CASE_VARIO(ITEM_SETUP_VARIO_PITCH)
  CASE_VARIO(ITEM_SETUP_VARIO_RANGE)
  CASE_VARIO(ITEM_SETUP_VARIO_REPEAT)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_LABEL)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_MODE)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_LENGTH)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_STRENGTH)
  // ITEM_SETUP_CONTRAST,
  ITEM_SETUP_ALARMS_LABEL,
  ITEM_SETUP_BATTERY_WARNING,
  ITEM_SETUP_INACTIVITY_ALARM,
  // ITEM_SETUP_MEMORY_WARNING,
  ITEM_SETUP_ALARM_WARNING,
  ITEM_SETUP_BACKLIGHT_LABEL,
  ITEM_SETUP_BACKLIGHT_MODE,
  ITEM_SETUP_BACKLIGHT_DELAY,
  ITEM_SETUP_BRIGHTNESS,
  ITEM_SETUP_FLASH_BEEP,
  // CASE_SPLASH_PARAM(ITEM_SETUP_DISABLE_SPLASH)
  CASE_GPS(ITEM_SETUP_TIMEZONE)
  CASE_GPS(ITEM_SETUP_GPSFORMAT)
  CASE_PXX(ITEM_SETUP_COUNTRYCODE)
  ITEM_SETUP_LANGUAGE,
  ITEM_SETUP_IMPERIAL,
  // IF_FAI_CHOICE(ITEM_SETUP_FAI)
  // CASE_MAVLINK(ITEM_MAVLINK_BAUD)
  ITEM_SETUP_SWITCHES_DELAY,
  ITEM_SETUP_RX_CHANNEL_ORD,
  ITEM_SETUP_STICK_MODE,
  ITEM_SETUP_MAX
};

bool menuGeneralSetup(evt_t event)
{
#if defined(RTCLOCK)
  struct gtm t;
  gettime(&t);

  if ((menuVerticalPosition==ITEM_SETUP_DATE || menuVerticalPosition==ITEM_SETUP_TIME) &&
      (s_editMode>0) &&
      (event==EVT_KEY_FIRST(KEY_ENTER) || event==EVT_KEY_BREAK(KEY_ENTER) || event==EVT_KEY_LONG(KEY_ENTER) || event==EVT_KEY_FIRST(KEY_EXIT))) {
    // set the date and time into RTC chip
    rtcSetTime(&t);
  }
#endif

#if defined(FAI_CHOICE)
  if (warningResult) {
    warningResult = 0;
    g_eeGeneral.fai = true;
    storageDirty(EE_GENERAL);
  }
#endif

  MENU(STR_MENURADIOSETUP, LBM_RADIO_ICONS, menuTabGeneral, e_Setup, ITEM_SETUP_MAX, {
    2|NAVIGATION_LINE_BY_LINE, 2|NAVIGATION_LINE_BY_LINE, 1|NAVIGATION_LINE_BY_LINE,
    LABEL(SOUND), 0, 0, 0, 0, 0, 0, 0,
    CASE_VARIO(LABEL(VARIO)) CASE_VARIO(0) CASE_VARIO(0) CASE_VARIO(0) CASE_VARIO(0)
    CASE_HAPTIC(LABEL(HAPTIC)) CASE_HAPTIC(0) CASE_HAPTIC(0) CASE_HAPTIC(0)
    LABEL(ALARMS), 0, 0, 0, CASE_GPS(0) CASE_GPS(0) CASE_PXX(0) 0, 0, 0, 0, 0, 0, 1/*to force edit mode*/ });

  int sub = menuVerticalPosition;

  for (int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    int k = i + menuVerticalOffset;
    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_SETUP_DATE:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_DATE);
        LcdFlags flags = 0;
        if (attr && menuHorizontalPosition < 0) {
          flags |= INVERS;
          lcdDrawSolidFilledRect(RADIO_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN+1, 100, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }
        lcdDrawText(RADIO_SETUP_2ND_COLUMN+YEAR_SEPARATOR_OFFSET, y, "-", flags);
        lcdDrawText(RADIO_SETUP_2ND_COLUMN+MONTH_SEPARATOR_OFFSET, y, "-", flags);
        for (uint8_t j=0; j<3; j++) {
          uint8_t rowattr = (menuHorizontalPosition==j ? attr : 0);
          switch (j) {
            case 0:
              lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, t.tm_year+1900, LEFT|flags|rowattr);
              if (rowattr && s_editMode>0) t.tm_year = checkIncDec(event, t.tm_year, 112, 200, 0);
              break;
            case 1:
              lcdDrawNumber(RADIO_SETUP_2ND_COLUMN+MONTH_OFFSET, y, t.tm_mon+1, LEFT|flags|rowattr|LEADING0, 2);
              if (rowattr && s_editMode>0) t.tm_mon = checkIncDec(event, t.tm_mon, 0, 11, 0);
              break;
            case 2:
            {
              int16_t year = 1900 + t.tm_year;
              int8_t dlim = (((((year%4==0) && (year%100!=0)) || (year%400==0)) && (t.tm_mon==1)) ? 1 : 0);
              static const pm_uint8_t dmon[] PROGMEM = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
              dlim += pgm_read_byte(&dmon[t.tm_mon]);
              lcdDrawNumber(RADIO_SETUP_2ND_COLUMN+DAY_OFFSET, y, t.tm_mday, LEFT|flags|rowattr|LEADING0, 2);
              if (rowattr && s_editMode>0) t.tm_mday = checkIncDec(event, t.tm_mday, 1, dlim, 0);
              break;
            }
          }
        }
        if (attr && checkIncDec_Ret) {
          g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
        }
        break;
      }

      case ITEM_SETUP_TIME:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TIME);
        LcdFlags flags = 0;
        if (attr && menuHorizontalPosition < 0) {
          flags |= INVERS;
          lcdDrawSolidFilledRect(RADIO_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN+1, 100, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }
        lcdDrawText(RADIO_SETUP_2ND_COLUMN+HOUR_SEPARATOR_OFFSET, y, ":", flags);
        lcdDrawText(RADIO_SETUP_2ND_COLUMN+MINUTE_SEPARATOR_OFFSET, y, ":", flags);
        for (uint8_t j=0; j<3; j++) {
          uint8_t rowattr = (menuHorizontalPosition==j ? attr : 0);
          switch (j) {
            case 0:
              if (rowattr && s_editMode>0) t.tm_hour = checkIncDec(event, t.tm_hour, 0, 23, 0);
              lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, t.tm_hour, flags|rowattr|LEFT|LEADING0, 2);
              break;
            case 1:
              if (rowattr && s_editMode>0) t.tm_min = checkIncDec(event, t.tm_min, 0, 59, 0);
              lcdDrawNumber(RADIO_SETUP_2ND_COLUMN+MINUTE_OFFSET, y, t.tm_min, flags|rowattr|LEFT|LEADING0, 2);
              break;
            case 2:
              if (rowattr && s_editMode>0) t.tm_sec = checkIncDec(event, t.tm_sec, 0, 59, 0);
              lcdDrawNumber(RADIO_SETUP_2ND_COLUMN+SECOND_OFFSET, y, t.tm_sec, flags|rowattr|LEFT|LEADING0, 2);
              break;
          }
        }
        if (attr && checkIncDec_Ret)
          g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
        break;
      }

      case ITEM_SETUP_BATT_RANGE:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BATTERY_RANGE);
        LcdFlags flags = 0;
        if (attr && menuHorizontalPosition < 0) {
          flags |= INVERS;
          lcdDrawSolidFilledRect(RADIO_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN+1, 100, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, 90+g_eeGeneral.vBatMin, flags|(menuHorizontalPosition==0 ? attr : 0)|PREC1|LEFT);
        lcdDrawText(lcdNextPos+8, y, "-", flags);
        lcdDrawNumber(lcdNextPos+8, y, 120+g_eeGeneral.vBatMax, flags|(menuHorizontalPosition>0 ? attr : 0)|PREC1|LEFT);
        lcdDrawText(lcdNextPos+1, y, "V", flags);
        if (attr && s_editMode>0) {
          if (menuHorizontalPosition==0)
            CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMin, -50, g_eeGeneral.vBatMax+29); // min=4.0V
          else
            CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMax, g_eeGeneral.vBatMin-29, +40); // max=16.0V
        }
        break;
      }

      case ITEM_SETUP_SOUND_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SOUND_LABEL);
        break;

      case ITEM_SETUP_BEEP_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SPEAKER);
        g_eeGeneral.beepMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_VBEEPMODE, g_eeGeneral.beepMode, -2, 1, attr, event);
#if defined(FRSKY)
        if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
        break;

      case ITEM_SETUP_GENERAL_VOLUME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SPEAKER_VOLUME);
        g_eeGeneral.speakerVolume = editSlider(RADIO_SETUP_2ND_COLUMN, y, event, g_eeGeneral.speakerVolume, -VOLUME_LEVEL_DEF, VOLUME_LEVEL_MAX-VOLUME_LEVEL_DEF, attr);
        break;

      case ITEM_SETUP_BEEP_VOLUME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BEEP_VOLUME);
        g_eeGeneral.beepVolume = editSlider(RADIO_SETUP_2ND_COLUMN, y, event, g_eeGeneral.beepVolume, -2, +2, attr);
        break;
      case ITEM_SETUP_WAV_VOLUME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_WAV_VOLUME);
        g_eeGeneral.wavVolume = editSlider(RADIO_SETUP_2ND_COLUMN, y, event, g_eeGeneral.wavVolume, -2, +2, attr);
        break;
      case ITEM_SETUP_BACKGROUND_VOLUME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BG_VOLUME);
        g_eeGeneral.backgroundVolume = editSlider(RADIO_SETUP_2ND_COLUMN, y, event, g_eeGeneral.backgroundVolume, -2, +2, attr);
        break;

      case ITEM_SETUP_BEEP_LENGTH:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BEEP_LENGTH);
        SLIDER_5POS(y, g_eeGeneral.beepLength, event, attr);
        break;

      case ITEM_SETUP_SPEAKER_PITCH:
        lcdDrawText(MENUS_MARGIN_LEFT,  y, STR_SPKRPITCH);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.speakerPitch*15, attr|LEFT, 0, "+", "Hz");
        if (attr) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.speakerPitch, 0, 20);
        }
        break;

#if defined(VARIO)
      case ITEM_SETUP_VARIO_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_VARIO);
        break;
      case ITEM_SETUP_VARIO_VOLUME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, TR_SPEAKER_VOLUME);
        g_eeGeneral.varioVolume = editSlider(RADIO_SETUP_2ND_COLUMN, y, event, g_eeGeneral.varioVolume, -2, +2, attr);
        break;
      case ITEM_SETUP_VARIO_PITCH:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PITCH_AT_ZERO);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, VARIO_FREQUENCY_ZERO+(g_eeGeneral.varioPitch*10), attr|LEFT, 0, NULL, "Hz");
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.varioPitch, -40, 40);
        break;
      case ITEM_SETUP_VARIO_RANGE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PITCH_AT_MAX);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, VARIO_FREQUENCY_ZERO+(g_eeGeneral.varioPitch*10)+VARIO_FREQUENCY_RANGE+(g_eeGeneral.varioRange*10), attr|LEFT, 0, NULL, "Hz");
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.varioRange, -80, 80);
        break;
      case ITEM_SETUP_VARIO_REPEAT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_REPEAT_AT_ZERO);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, VARIO_REPEAT_ZERO+(g_eeGeneral.varioRepeat*10), attr|LEFT, 0, NULL, STR_MS);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.varioRepeat, -30, 50);
        break;
#endif

#if defined(HAPTIC)
      case ITEM_SETUP_HAPTIC_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_HAPTIC_LABEL);
        break;

      case ITEM_SETUP_HAPTIC_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODE);
        g_eeGeneral.hapticMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_VBEEPMODE, g_eeGeneral.hapticMode, -2, 1, attr, event);
        break;

      case ITEM_SETUP_HAPTIC_LENGTH:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_LENGTH);
        SLIDER_5POS(y, g_eeGeneral.hapticLength, event, attr);
        break;

      case ITEM_SETUP_HAPTIC_STRENGTH:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_HAPTICSTRENGTH);
        SLIDER_5POS(y, g_eeGeneral.hapticStrength, event, attr);
        break;
#endif

#if 0
      case ITEM_SETUP_CONTRAST:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CONTRAST);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.contrast, attr|LEFT);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.contrast, CONTRAST_MIN, CONTRAST_MAX);
          lcdSetContrast();
        }
        break;
#endif

      case ITEM_SETUP_ALARMS_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ALARMS_LABEL);
        break;

      case ITEM_SETUP_BATTERY_WARNING:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BATTERYWARNING);
        putsValueWithUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.vBatWarn, UNIT_VOLTS, attr|PREC1|LEFT);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatWarn, 27, 42); // 2.7-4.2V
        break;

#if 0
      case ITEM_SETUP_MEMORY_WARNING:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MEMORYWARNING);
        uint8_t b = 1-g_eeGeneral.disableMemoryWarning;
        g_eeGeneral.disableMemoryWarning = 1 - editCheckBox(b, RADIO_SETUP_2ND_COLUMN, y, attr, event);
        break;
      }
#endif

      case ITEM_SETUP_ALARM_WARNING:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ALARMWARNING);
        uint8_t b = 1-g_eeGeneral.disableAlarmWarning;
        g_eeGeneral.disableAlarmWarning = 1 - editCheckBox(b, RADIO_SETUP_2ND_COLUMN, y, attr, event);
        break;
      }

#if defined(PCBSKY9X)
      case ITEM_SETUP_CAPACITY_WARNING:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CAPAWARNING);
        putsValueWithUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.mAhWarn*50, UNIT_MAH, attr|LEFT) ;
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.mAhWarn, 0, 100);
        break;
#endif

#if defined(PCBSKY9X)
      case ITEM_SETUP_TEMPERATURE_WARNING:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TEMPWARNING);
        putsValueWithUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.temperatureWarn, UNIT_TEMPERATURE, attr|LEFT) ;
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.temperatureWarn, 0, 120); // 0 means no alarm
        break;
#endif

      case ITEM_SETUP_INACTIVITY_ALARM:
        lcdDrawText(MENUS_MARGIN_LEFT,  y,STR_INACTIVITYALARM);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.inactivityTimer, attr|LEFT, 0, NULL, "m");
        if (attr) g_eeGeneral.inactivityTimer = checkIncDec(event, g_eeGeneral.inactivityTimer, 0, 250, EE_GENERAL); //0..250minutes
        break;

      case ITEM_SETUP_BACKLIGHT_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BACKLIGHT_LABEL);
        break;

      case ITEM_SETUP_BACKLIGHT_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODE);
        g_eeGeneral.backlightMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_VBLMODE, g_eeGeneral.backlightMode, e_backlight_mode_off, e_backlight_mode_on, attr, event);
        break;

      case ITEM_SETUP_FLASH_BEEP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ALARM);
        g_eeGeneral.alarmsFlash = editCheckBox(g_eeGeneral.alarmsFlash, RADIO_SETUP_2ND_COLUMN, y, attr, event ) ;
        break;

      case ITEM_SETUP_BACKLIGHT_DELAY:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BLDELAY);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.lightAutoOff*5, attr|LEFT, 0, NULL, "s");
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.lightAutoOff, 0, 600/5);
        break;

      case ITEM_SETUP_BRIGHTNESS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BRIGHTNESS);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, 100-g_eeGeneral.backlightBright, attr|LEFT) ;
        if (attr) {
          uint8_t b = 100 - g_eeGeneral.backlightBright;
          CHECK_INCDEC_GENVAR(event, b, 0, 100);
          g_eeGeneral.backlightBright = 100 - b;
        }
        break;

#if 0
      case ITEM_SETUP_DISABLE_SPLASH:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SPLASHSCREEN);
        if (SPLASH_NEEDED()) {
          lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, SPLASH_TIMEOUT/100, attr|LEFT, 0, NULL, "s");
        }
        else {
          lcdDrawTextAtIndex(RADIO_SETUP_2ND_COLUMN, y, STR_MMMINV, 0, attr); // TODO define
        }
        if (attr) g_eeGeneral.splashMode = -checkIncDecGen(event, -g_eeGeneral.splashMode, -3, 4);
        break;
      }
#endif

      case ITEM_SETUP_TIMEZONE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TIMEZONE);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.timezone, attr|LEFT);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.timezone, -12, 12);
        break;

      case ITEM_SETUP_GPSFORMAT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_GPSCOORD);
        g_eeGeneral.gpsFormat = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_GPSFORMAT, g_eeGeneral.gpsFormat, 0, 1, attr, event);
        break;

      case ITEM_SETUP_COUNTRYCODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_COUNTRYCODE);
        g_eeGeneral.countryCode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_COUNTRYCODES, g_eeGeneral.countryCode, 0, 2, attr, event);
        break;

      case ITEM_SETUP_LANGUAGE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_VOICELANG);
        lcdDrawText(RADIO_SETUP_2ND_COLUMN, y, currentLanguagePack->name, attr);
        if (attr) {
          currentLanguagePackIdx = checkIncDec(event, currentLanguagePackIdx, 0, DIM(languagePacks)-2, EE_GENERAL);
          if (checkIncDec_Ret) {
            currentLanguagePack = languagePacks[currentLanguagePackIdx];
            strncpy(g_eeGeneral.ttsLanguage, currentLanguagePack->id, 2);
          }
        }
        break;

      case ITEM_SETUP_IMPERIAL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_UNITSSYSTEM);
        g_eeGeneral.imperial = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_VUNITSSYSTEM, g_eeGeneral.imperial, 0, 1, attr, event);
        break;

#if 0
      case ITEM_SETUP_FAI:
        lcdDrawText(MENUS_MARGIN_LEFT, y, PSTR("FAI Mode"));
        g_eeGeneral.fai = editCheckBox(g_eeGeneral.fai, RADIO_SETUP_2ND_COLUMN, y, attr, event);
        if (attr && checkIncDec_Ret) {
          if (g_eeGeneral.fai)
            POPUP_WARNING(PSTR("FAI\001mode blocked!"));
          else
            POPUP_CONFIRMATION(PSTR("FAI mode?"));
        }
        break;
#endif

#if defined(MAVLINK)
      case ITEM_MAVLINK_BAUD:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MAVLINK_BAUD_LABEL);
        g_eeGeneral.mavbaud = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MAVLINK_BAUDS, g_eeGeneral.mavbaud, 0, 7, attr, event);
        break;
#endif

      case ITEM_SETUP_SWITCHES_DELAY:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SWITCHES_DELAY);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, 10*SWITCHES_DELAY(), attr|LEFT, 0, NULL, STR_MS);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.switchesDelay, -15, +15);
        break;

      case ITEM_SETUP_RX_CHANNEL_ORD:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_RXCHANNELORD); // RAET->AETR
        char s[5];
        for (uint8_t i=0; i<4; i++) {
          s[i] = STR_RETA123[channel_order(i+1)];
        }
        s[4] = '\0';
        lcdDrawText(RADIO_SETUP_2ND_COLUMN, y, s, attr);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.templateSetup, 0, 23);
        break;
      }

      case ITEM_SETUP_STICK_MODE:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_MODE));
        char s[2] = " ";
        s[0] = '1'+g_eeGeneral.stickMode;
        lcdDrawText(RADIO_SETUP_2ND_COLUMN, y, s, attr);
        for (uint8_t i=0; i<4; i++) {
          putsMixerSource(RADIO_SETUP_2ND_COLUMN + 40 + 50*i, y, MIXSRC_Rud + pgm_read_byte(modn12x3 + 4*g_eeGeneral.stickMode + i));
        }
        if (attr && s_editMode>0) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.stickMode, 0, 3);
        }
        else if (stickMode != g_eeGeneral.stickMode) {
          pausePulses();
          stickMode = g_eeGeneral.stickMode;
          checkTHR();
          resumePulses();
          clearKeyEvents();
        }
        break;
      }
    }
  }

  return true;
}
