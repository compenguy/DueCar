// SPDX-License-Identifier: MIT

#ifndef BLECENTRAL_H_INCLUDED
#define BLECENTRAL_H_INCLUDED

#include "bleuart.h"

class Central {
  public:
    enum class state_t { cUnknown, cReady, cConnecting, cConnected };

    Central(Modem &modem) : state(state_t::cUnknown), modem(modem) {}

    bool isUnknown();
    bool isReady();
    bool isConnecting();
    bool isConnected();

    bool ready();
    bool connectLast();
    bool connectId(uint8_t);
    bool connectAddress(const String &);
    bool disconnect();

    Modem &modem;

  private:
    state_t state;
};

#endif /* BLECENTRAL_H_INCLUDED */
