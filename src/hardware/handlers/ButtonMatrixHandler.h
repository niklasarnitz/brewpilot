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
  ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState)
      : buttonMatrixState(buttonMatrixState)
  {
    switch (HARDWARE_MODEL)
    {
    case MachineType::LA_CIMBALI_M29_SELECT:
      machineButtonMatrixHandler = new LaCimbaliM29ButtonMatrixHandler(buttonMatrixState);
      break;
    case MachineType::RANCILIO_S27:
      machineButtonMatrixHandler = new RancilioS27ButtonMatrixHandler(buttonMatrixState);
      break;
    }

    machineButtonMatrixHandler->initializePins();
  };

  void handle()
  {
    machineButtonMatrixHandler->handle();
  };
};

#endif // BREWPILOT_BUTTONMATRIXHANDLER_H
