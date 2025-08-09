#pragma once
#include "GameFramework/Pawn.h"
#include "NexusPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UWidgetComponent;

UCLASS()
class NEXUSCHESS_API ANexusPawn : public APawn
{
	GENERATED_BODY()

public:
#pragma region System Function
	ANexusPawn(const FObjectInitializer& objectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent) override;
#pragma endregion System Function

#pragma region Public Variables

#pragma endregion Public Variables

#pragma region protected Variables
#pragma endregion protected Variables

private:
#pragma region Private Variables
	UCameraComponent* _characterCamera;
	USpringArmComponent* _cameraBoom;
	//-------------------------------
	//-------------------------------
	// This is for Smooth Interpulation butween run, jog, walk, Turn-LeftRight
	//-------------------------------
	float _currentSpeedValue = 0;
	float _currentDirectionValue = 0;
	bool _isActiveMouseAxis = false;
	float _currentTargetArmValue = 0;
	float _currentCameraFOV = 0;
	//-------------------------------
	//UPROPERTY(Replicated) AChessTile* _clickedTile;
#pragma endregion Private Variables

public:
#pragma region Public Function
#pragma endregion Public Function

#pragma region Protected Function

#pragma endregion Protected Function

private:
#pragma region Private Function
#pragma endregion Private Function

private:
#pragma region Events Function
	void IE_MoveForwardBackward(float value);
	void IE_MoveRightLeft(float value);
	void IE_TurnLeftRight(float value);
	void IE_LookUpDown(float value);
	void IE_TragetArmLength(float value);
	void IE_CameraFOV(float value);
#pragma endregion Events Function
};
