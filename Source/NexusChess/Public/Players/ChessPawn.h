// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChessPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UWidgetComponent;

UCLASS()
class NEXUSCHESS_API AChessPawn : public APawn
{
	GENERATED_BODY()


public:
#pragma region System Function
	AChessPawn(const FObjectInitializer& objectInitializer);
	//protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void PostInitializeComponents();
#pragma endregion System Function

#pragma region Public Variables

#pragma endregion Public Variables

#pragma region protected Variables
	UPROPERTY(Transient) TArray<UMaterialInstanceDynamic*> _allMaterials;
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
