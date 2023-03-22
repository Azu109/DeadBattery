// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeadBatteryGameMode.h"
#include "DeadBatteryCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADeadBatteryGameMode::ADeadBatteryGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
