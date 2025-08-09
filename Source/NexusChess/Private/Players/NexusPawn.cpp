#include "Players/NexusPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
//---------------------------------------

ANexusPawn::ANexusPawn(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// Camera Boom Setup
	this->_cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	this->_cameraBoom->SetupAttachment(AActor::RootComponent);
	this->_cameraBoom->TargetArmLength = 800.0f;  // Higher value for top-down view
	this->_cameraBoom->bDoCollisionTest = false;  // Prevents camera from getting blocked
	this->_cameraBoom->bUsePawnControlRotation = true;
	this->_cameraBoom->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f)); // Direct top-down angle

	// Camera Setup
	this->_characterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CharacterCamera"));
	this->_characterCamera->SetupAttachment(this->_cameraBoom, USpringArmComponent::SocketName);
	this->_characterCamera->bUsePawnControlRotation = false;
	this->_characterCamera->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f)); // No tilt needed


	//------------------------------------
	// Character Base, Camera, Rotation Basic Setup
	//------------------------------------
	// Don't rotate when the controller rotates. Let that just affect the camera.
	//------------------------------------
	APawn::bUseControllerRotationPitch = false;
	APawn::bUseControllerRotationYaw = false;
	APawn::bUseControllerRotationRoll = false;
	//------------------------------------
}
void ANexusPawn::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != NM_DedicatedServer)
	{
		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			pc->SetShowMouseCursor(true);
			pc->bEnableClickEvents = true;
			pc->bEnableMouseOverEvents = true;
			pc->bEnableTouchOverEvents = true;
			pc->bEnableTouchEvents = true;

			this->AddControllerPitchInput(-90.0f);
			//this->AddControllerYawInput(-90.0f);
			//this->AddControllerRollInput(90.0f);
		}
	}
}
void ANexusPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ANexusPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (!PlayerInputComponent) return;

	//-----------------Axis Maping-----------------
	PlayerInputComponent->BindAxis("MoveForward/Backwards", this, &ANexusPawn::IE_MoveForwardBackward);
	PlayerInputComponent->BindAxis("MoveRight/Left", this, &ANexusPawn::IE_MoveRightLeft);
	PlayerInputComponent->BindAxis("LookLeft/Right", this, &ANexusPawn::IE_TurnLeftRight);
	PlayerInputComponent->BindAxis("LookUp/Down", this, &ANexusPawn::IE_LookUpDown);
	PlayerInputComponent->BindAxis("TragetArmLength", this, &ThisClass::IE_TragetArmLength);
	PlayerInputComponent->BindAxis("CameraFOV", this, &ThisClass::IE_CameraFOV);
	//-----------------Axis Maping-----------------
}
#pragma endregion System Function

#pragma region Public Function
#pragma endregion Public Function

#pragma region Protected Function
#pragma endregion Protected Function

#pragma region Private Function
#pragma endregion Private Function

#pragma region Events Function
void ANexusPawn::IE_MoveForwardBackward(float value)
{
	if (!APawn::Controller) return;

	if (value > 0.0f && this->_currentSpeedValue != 1.0f)
	{
		if (this->_currentSpeedValue <= 0.8f)
			this->_currentSpeedValue = FMath::FInterpTo(this->_currentSpeedValue, 1.0f, GetWorld()->GetDeltaSeconds(), 2.50f);
		else
			this->_currentSpeedValue = 1;
	}
	if (value < 0.0f && this->_currentSpeedValue != 1.0f)
	{
		if (this->_currentSpeedValue >= -0.8f)
			this->_currentSpeedValue = FMath::FInterpTo(this->_currentSpeedValue, -1.0f, GetWorld()->GetDeltaSeconds(), 2.50f);
		else this->_currentSpeedValue = -1;
	}
	else if (value == 0.0f && this->_currentSpeedValue != 0.0f)
	{
		if (this->_currentSpeedValue > 0.2f)
			this->_currentSpeedValue = FMath::FInterpTo(this->_currentSpeedValue, 0.0f, GetWorld()->GetDeltaSeconds(), 2.5f);
		else this->_currentSpeedValue = 0;
	}
	else if (this->_currentSpeedValue == 0.0f) return;

	const FRotator yawRotation(0.0f, APawn::Controller->GetControlRotation().Yaw, 0.0f);
	const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X));
	AddMovementInput(direction, this->_currentSpeedValue);
}
void ANexusPawn::IE_MoveRightLeft(float value)
{
	if (!APawn::Controller) return;

	if (value > 0.0f && this->_currentDirectionValue != 1.0f)
	{
		if (this->_currentDirectionValue <= 0.8f)
			this->_currentDirectionValue = FMath::FInterpTo(this->_currentDirectionValue, 1.0f, GetWorld()->GetDeltaSeconds(), 2.50f);
		else
			this->_currentDirectionValue = 1;
	}
	if (value < 0.0f && this->_currentDirectionValue != 1.0f)
	{
		if (this->_currentDirectionValue >= -0.8f)
			this->_currentDirectionValue = FMath::FInterpTo(this->_currentDirectionValue, -1.0f, GetWorld()->GetDeltaSeconds(), 2.50f);
		else
			this->_currentDirectionValue = -1;
	}
	else if (value == 0.0f && this->_currentDirectionValue != 0.0f)
	{
		if (this->_currentDirectionValue > 0.2f)
			this->_currentDirectionValue = FMath::FInterpTo(this->_currentDirectionValue, 0.0f, GetWorld()->GetDeltaSeconds(), 2.5f);
		else
			this->_currentDirectionValue = 0;
	}
	else if (this->_currentDirectionValue == 0.0f) return;

	const FRotator yawRotation(0.0f, APawn::Controller->GetControlRotation().Yaw, 0.0f);
	const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y));
	AddMovementInput(direction, this->_currentDirectionValue);
}
void ANexusPawn::IE_TurnLeftRight(float value)
{
	if (APawn::Controller && value != 0.0f)
	{
		AddControllerYawInput(value);
	}
}
void ANexusPawn::IE_LookUpDown(float value)
{
	if (APawn::Controller && value != 0.0f)
	{
		AddControllerPitchInput(value);
	}
}
void ANexusPawn::IE_TragetArmLength(float value)
{
	if (!APawn::Controller || (this->_currentTargetArmValue == 0.0f && value == 0.0f)) return;

	this->_currentTargetArmValue = FMath::FInterpTo(this->_currentTargetArmValue, value, GetWorld()->GetDeltaSeconds(), 2.50f);
	float targetArm = this->_cameraBoom->TargetArmLength += (_currentTargetArmValue * -15.0f);

	if (targetArm > 3000.0f) targetArm = 3000.0f;
	else if (targetArm < 200.0f) targetArm = 200.0f;

	this->_cameraBoom->TargetArmLength = targetArm;
}
void ANexusPawn::IE_CameraFOV(float value)
{
	if (!APawn::Controller || (this->_currentCameraFOV == 0.0f && value == 0.0f)) return;

	this->_currentCameraFOV = FMath::FInterpTo(this->_currentCameraFOV, value, GetWorld()->GetDeltaSeconds(), 1.50f);
	float fov = this->_characterCamera->FieldOfView += (_currentCameraFOV * -1.0f);
	
	if (fov > 120.0f) fov = 120.0f;
	else if (fov < 30.0f) fov = 30.0f;

	this->_characterCamera->SetFieldOfView(fov);
}
#pragma endregion Events Function