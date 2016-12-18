#pragma once

#define LEDS_NUMBER 1

#define SER_CON_RX_PIN              14   // UART RX pin number.
#define SER_CON_TX_PIN              15   // UART TX pin number.
#define SER_CON_CTS_PIN             11   // UART Clear To Send pin number.
#define SER_CON_RTS_PIN             12   // UART Request To Send pin number.

#define LED_0          0

// #define NRF_CLOCK_LFCLKSRC {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
//                             .rc_ctiv       = 0,                                \
//                             .rc_temp_ctiv  = 0,                                \
//                             .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}

#define NRF_CLOCK_LFCLKSRC { \
    .source        = NRF_CLOCK_LF_SRC_RC, \
    .rc_ctiv       = 16, \
    .rc_temp_ctiv  = 2, \
    .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM}
