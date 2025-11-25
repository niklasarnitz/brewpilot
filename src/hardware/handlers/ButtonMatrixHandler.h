//
// Created by Niklas Arnitz on 17.11.24.
//

#include "../../structs/ButtonMatrixState.h"
#include "../../utils/noop.h"
#include "machine-specific-handlers/MachineButtonMatrixHandler.h"
#include "../../UserConfig.h"
#include "machine-specific-handlers/LaCimbaliM29ButtonMatrixHandler.h"

#ifndef BREWPILOT_BUTTONMATRIXHANDLER_H
#define BREWPILOT_BUTTONMATRIXHANDLER_H

class ButtonMatrixHandler
{
private:
  ButtonMatrixState *buttonMatrixState;
  MachineButtonMatrixHandler machineButtonMatrixHandler;

public:
  ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState)
      : buttonMatrixState(buttonMatrixState)
  {
    switch (HARDWARE_MODEL)
    {
    case MachineType::LA_CIMBALI_M29_SELECT:
      machineButtonMatrixHandler = LaCimbaliM29ButtonMatrixHandler();
      break;
    }

    machineButtonMatrixHandler.initializePins();
  };

  void handle()
  {
    machineButtonMatrixHandler.handle();
  };
};

#endif // BREWPILOT_BUTTONMATRIXHANDLER_H
