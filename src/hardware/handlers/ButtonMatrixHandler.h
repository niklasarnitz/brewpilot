//
// Created by Niklas Arnitz on 17.11.24.
//

#include "../../structs/ButtonMatrixState.h"
#include "../../utils/noop.h"
#include "machine-specific-handlers/MachineButtonMatrixHandler.h"
#include "../../UserConfig.h"
#include "machine-specific-handlers/LaCimbaliM29ButtonMatrixHandler.h"
#include "machine-specific-handlers/RancilioS27ButtonMatrixHandler.h"

#ifndef BREWPILOT_BUTTONMATRIXHANDLER_H
#define BREWPILOT_BUTTONMATRIXHANDLER_H

class ButtonMatrixHandler
{
private:
  ButtonMatrixState *buttonMatrixState;
  MachineButtonMatrixHandler *machineButtonMatrixHandler;

public:
  ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState);

  ~ButtonMatrixHandler();

  // Delete copy constructor and copy assignment operator
  ButtonMatrixHandler(const ButtonMatrixHandler &) = delete;
  ButtonMatrixHandler &operator=(const ButtonMatrixHandler &) = delete;

  // Delete move constructor and move assignment operator
  ButtonMatrixHandler(ButtonMatrixHandler &&) = delete;
  ButtonMatrixHandler &operator=(ButtonMatrixHandler &&) = delete;

  void handle();
};

#endif // BREWPILOT_BUTTONMATRIXHANDLER_H
