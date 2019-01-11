EPD_DISPLAY_IMAGE* epd_image = NULL;
String epaper_messagetext = "";
uint16_t epaper_messagetext_color = GxEPD_BLACK;
int displayPage = 0;

void updateDisplay(DISPLAY_UPDATE_TYPE d) {
  updateDisplay(d, "", GxEPD_BLACK);
}

void updateDisplay(DISPLAY_UPDATE_TYPE d, String messagetext, uint16_t messagecolor) {
  displayPage = 0;
  
  DEBUG_PRT.println("Updating Display");
  ePaper.init(); // disable diagnostic output on Serial (use 115200 as parameter to enable diagnostic output on Serial)
  DEBUG_PRT.println("Init Display");

  if (d == display_reading) {
    DEBUG_PRT.println("display_reading");
    
    //ePaper.setMode(LUT_MODE_CLEAR);
    //ePaper.eraseDisplay(true);
    
  
    //ePaper.setMode(LUT_MODE_COMPOSITE);

    int epd_contrast = Config::GetEPDContrast();
    
    switch (diskfont._FontHeader.antialias_level)
    {
      case 4:
        ePaper.resetRefreshNumber(LUT_MODE_3BPP);
        break;

      case 2:
        ePaper.resetRefreshNumber(LUT_MODE_2BPP);
        epd_contrast = epd_contrast >> 1;
        break;

      case 1:
      default:
        ePaper.resetRefreshNumber(LUT_MODE_1BPP);
        epd_contrast = 1;
        break;
    }

    do {
      //Serial.printf("refresh number = %d\n", ePaper.getRefreshNumber());
      ePaper.drawPaged(epaperUpdate);
      DEBUG_PRT.println();
    } while(ePaper.decRefreshNumber() && ePaper.getRefreshNumber() >= epd_contrast);
    
    return;
  } 
//  if (d == display_reading) {
//    DEBUG_PRT.println("display_reading");
//    ePaper.drawPaged(epaperUpdate);
//    return;    
//  }

  ePaper.resetRefreshNumber(LUT_MODE_1BPP);

  epaper_messagetext = messagetext;
  epaper_messagetext_color = messagecolor;
  
  switch(d) {
  case battery_recharge:
    DEBUG_PRT.println("display battery_recharge image");
    epd_image = &battery_recharge_image;
    break;

  case connect_power: 
    DEBUG_PRT.println("display connect_power image");
    epd_image = &connect_power_image;
    break;

  case wps_connect:
    DEBUG_PRT.println("display wps_connect image");
    epd_image = &wps_connect_image;
    break;

  case clock_not_set:
    DEBUG_PRT.println("display clock_not_set image");
    epd_image = &clock_not_set_image;
    break;

  case sd_card_not_inserted:
    DEBUG_PRT.println("display sd_card_not_inserted image");
    epd_image = &sd_card_not_inserted_image;
    break;

  case wireless_network_connected:
    DEBUG_PRT.println("display wireless_network_connected image");
    epd_image = &wireless_network_connected_image;
    break;

  case wps_setup_failed:
    DEBUG_PRT.println("display wps_setup_failed image");
    epd_image = &wps_setup_failed_no_network_image;
    break;

  case font_missing:
    DEBUG_PRT.println("display font_missing image");
    epd_image = &font_missing_image;
    break;

  case crash_bug:
    DEBUG_PRT.println("display crash_bug image");
    epd_image = &crash_bug_image;
    break;
    
  }
  
  ePaper.drawPaged(epaperDisplayImage);
}

void epaperUpdate() {
  wdt_reset();
  ePaper.eraseDisplay();
  ePaper.setRotation(EPD_ROTATION); //90 degrees 
  
  tb.render(ePaper, diskfont, displayPage);
  
  int charheight = diskfont._FontHeader.charheight;
  ePaper.drawFastHLine(0, charheight, PANEL_SIZE_X, GxEPD_BLACK);

  displayPage++;
  if (displayPage >= PAGE_COUNT) displayPage = 0;
  
  DEBUG_PRT.print(F("."));
}

void epaperDisplayImage() {
  if (epd_image == NULL) return;

  int image_size_x = 264;
  int image_size_y = 176;

  int image_offset_x = (PANEL_SIZE_X - image_size_x) / 2;
  int image_offset_y = (PANEL_SIZE_Y - image_size_y) / 2;
  
  wdt_reset();
  ePaper.setRotation(EPD_ROTATION == 1 ? 0 : 3); //These pictures are pre-rotated 90 degrees, for use on the 2.7 in display, byte order of which is portrait mode
  ePaper.eraseDisplay();
  if (epd_image->bitmap_black != NULL) ePaper.drawBitmap(epd_image->bitmap_black, image_offset_y, image_offset_x, image_size_y, image_size_x, GxEPD_BLACK, GxEPD::bm_transparent); 
  if (epd_image->bitmap_red   != NULL) ePaper.drawBitmap(epd_image->bitmap_red,   image_offset_y, image_offset_x, image_size_y, image_size_x, GxEPD_RED,   GxEPD::bm_transparent); 

  if (epaper_messagetext != "") {
    //int strwidth = diskfont.GetTextWidth(epaper_messagetext);
    ePaper.setRotation(EPD_ROTATION); //90 degrees       
    //diskfont.DrawStringAt(PANEL_SIZE_X - strwidth, 0, epaper_messagetext, ePaper, epaper_messagetext_color, false);

    ePaper.setFont(&FreeMonoBold9pt7b);

    int strend = 0;
    int strstart = 0;
    int cursor_x = image_offset_x + 0;
    int cursor_y = image_offset_y + 12;
    String substr = "";

    String charheightstr = "Ag";
    int16_t x1, y1;
    uint16_t w, h;
    ePaper.getTextBounds(charheightstr, 0, 0, &x1, &y1, &w, &h);
    
    int fontheight = h;

    if (!epaper_messagetext.endsWith("\n")) epaper_messagetext += "\n";
    
    while (strend != -1) {
      strend = epaper_messagetext.indexOf("\n", strstart);

      if (strend != -1) {
        substr = epaper_messagetext.substring(strstart, strend);
      }
      else {
        substr = epaper_messagetext.substring(strstart);
      }

      if (substr != "") {
        ePaper.setTextColor(GxEPD_WHITE);
        ePaper.setCursor(cursor_x - 1, cursor_y - 1);
        ePaper.print(substr);
  
        ePaper.setCursor(cursor_x + 1, cursor_y + 1);
        ePaper.print(substr);
  
        ePaper.setCursor(cursor_x - 1, cursor_y + 1);
        ePaper.print(substr);
  
        ePaper.setCursor(cursor_x + 1, cursor_y - 1);
        ePaper.print(substr);

        ePaper.setTextColor(epaper_messagetext_color);
        ePaper.setCursor(cursor_x, cursor_y);
        ePaper.print(substr);

        cursor_y += fontheight; // if the substring has a \n at the end, Y cursor should have been moved to the next line by the GFX library
      }
      strstart = strend + 1; // ready to move onto the next substring
    }
  }
  DEBUG_PRT.print(F("."));
}
