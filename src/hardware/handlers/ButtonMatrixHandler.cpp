#include "ButtonMatrixHandler.h"

ButtonMatrixHandler::ButtonMatrixHandler(ButtonMatrixState *buttonMatrixState) : buttonMatrixState(buttonMatrixState)
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
ButtonMatrixHandler::~ButtonMatrixHandler()
{
    if (machineButtonMatrixHandler != nullptr)
    {
        delete machineButtonMatrixHandler;
        machineButtonMatrixHandler = nullptr;
    }
}

void ButtonMatrixHandler::handle()
{
    machineButtonMatrixHandler->handle();
};
