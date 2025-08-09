#include "Players/LudoPawn.h"
#include "Engine/Engine.h"

ALudoPawn::ALudoPawn(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALudoPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (!PlayerInputComponent) return;

	// Note: Dice throwing is now handled by ALudoController
	// This pawn can be used for movement and other player-specific actions
}