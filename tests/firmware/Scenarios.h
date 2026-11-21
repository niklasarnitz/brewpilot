#pragma once

// Scenarios defined in the suite files and run by state-machine.cpp.

// boot.cpp
void testFirmwareBoot();

// settings.cpp
void settingsKeyMapping();
void settingsInvalidAndLoadFailure();
void standaloneDeviceDrivers();

// main.cpp
void failedStoragePreservesSettings();
void programmingDoesNotCaptureOtherDrinks();
void programmingCancelPreservesLastDose();
void stopWinsOverBackflush();
void normalDosesAndBoilerStillStop();
void extractionHasNoDeadline();
void backflushRemainsBoundedWithoutFlow();

// group-state.cpp
void doseContracts();
void continuousAndOwnership();
void programmingContracts();
void backflushContracts();
void prolongedGroupExtractions();
void pulseBoundaryContracts();
void backflushExtractionRollover();
void backflushPauseRollover();

// boiler-tea-state.cpp
void boilerSamplingAndOverfill();
void boilerFillingHasNoDeadline();
void teaNormalOperation();
void teaProgramming();
void teaNormalRollover();
void teaLearningRollover();

// button-events.cpp
void runButtonEventTests();

// input-output.cpp
void runInputOutputTests();
