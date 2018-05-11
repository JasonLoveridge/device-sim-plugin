/*
 ==============================================================================
 
 Device Simulation Plugin - DeviceEnumValues.h
 Author: Jason Loveridge
 Date: 05/2018
 BBC Research & Development

 ==============================================================================
 */

#pragma once

// Enumerated values for each device category and then also for each device within each category
enum Categories {
    PHONE = 0,
    LAPTOP = 1,
    TELEVISION = 2,
    SPEAKER = 3
};

enum Phones {
    IPHONE_7_PLUS = 0
};

enum Laptops {
    MACBOOK_PRO_2013 = 0
};

enum Televisions {
    PANASONIC_TXL47ET5B = 0
};

enum Speakers {
    SONY_SRSX11 = 0,
    GENELEC_6010_PAIR = 1
};
