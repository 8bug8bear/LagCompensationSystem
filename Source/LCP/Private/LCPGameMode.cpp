// Copyright Epic Games, Inc. All Rights Reserved.

#include "LCPGameMode.h"
#include "LCPCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALCPGameMode::ALCPGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
