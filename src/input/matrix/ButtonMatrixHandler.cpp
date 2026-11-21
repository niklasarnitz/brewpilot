#include "input/matrix/ButtonMatrixHandler.h"

#include "config/BuildConfig.h"
#include "input/matrix/LaCimbaliM29ButtonMatrixHandler.h"
#include "input/matrix/RancilioS27ButtonMatrixHandler.h"

ButtonMatrixHandler::ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState)
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
}

void ButtonMatrixHandler::handle()
{
    machineButtonMatrixHandler->handle();
}
