#include "Ludo/LudoDice.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

#pragma region System Function
ALudoDice::ALudoDice(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// Enable replication
	bReplicates = true;
	bAlwaysRelevant = true;

	// Create static mesh component
	DiceMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiceMeshComponent"));
	RootComponent = DiceMeshComponent;

	// Load the dice mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DiceMeshAsset(TEXT("/Game/LudoGame/Assets/SM_LudoDice.SM_LudoDice"));
	if (DiceMeshAsset.Succeeded())
	{
		DiceMeshComponent->SetStaticMesh(DiceMeshAsset.Object);
	}

	// Enable physics for throwing
	DiceMeshComponent->SetSimulatePhysics(true);
	DiceMeshComponent->SetEnableGravity(true);
	DiceMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DiceMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	
	// Enable physics replication
	DiceMeshComponent->SetIsReplicated(true);
}

void ALudoDice::BeginPlay()
{
	Super::BeginPlay();

	// Store original position and rotation
	OriginalLocation = GetActorLocation();
	OriginalRotation = GetActorRotation();

	// Bind replication callbacks
	if (HasAuthority())
	{
		// Server-side initialization
		CurrentDiceValue = 1;
		bIsThrowing = false;
		bHasFinishedThrowing = false;
	}
}

void ALudoDice::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// Only determine dice face on server when throwing is active
	if (HasAuthority() && bIsThrowing && !bHasFinishedThrowing)
	{
		// Check if dice has stopped moving
		FVector Velocity = DiceMeshComponent->GetPhysicsLinearVelocity();
		FVector AngularVelocity = DiceMeshComponent->GetPhysicsAngularVelocityInDegrees();
		
		if (Velocity.Size() < 0.5f && AngularVelocity.Size() < 0.5f)
		{
			// Dice has stopped, determine the face
			DetermineDiceFace();
		}
	}
}

void ALudoDice::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate dice properties
	DOREPLIFETIME(ALudoDice, CurrentDiceValue);
	DOREPLIFETIME(ALudoDice, bIsThrowing);
	DOREPLIFETIME(ALudoDice, bHasFinishedThrowing);
	
	// Replicate physics parameters for deterministic simulation
	DOREPLIFETIME(ALudoDice, ReplicatedThrowDirection);
	DOREPLIFETIME(ALudoDice, ReplicatedAngularImpulse);
	DOREPLIFETIME(ALudoDice, ReplicatedThrowForce);
	DOREPLIFETIME(ALudoDice, ReplicatedRotationForce);
}
#pragma endregion System Function

#pragma region Public Function
void ALudoDice::ThrowDice()
{
	if (HasAuthority())
	{
		Server_ThrowDice();
	}
	else
	{
		// Client calls server function
		Server_ThrowDice();
	}
}

void ALudoDice::ResetDice()
{
	if (HasAuthority())
	{
		// Reset position and rotation
		SetActorLocation(OriginalLocation);
		SetActorRotation(OriginalRotation);
		
		// Stop physics
		DiceMeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
		DiceMeshComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		
		// Reset state
		bIsThrowing = false;
		bHasFinishedThrowing = false;
		CurrentDiceValue = 1;
		
		// Clear timer
		GetWorld()->GetTimerManager().ClearTimer(ThrowTimerHandle);
		
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Dice Reset!"));
	}
}

void ALudoDice::ShowCurrentRotation()
{
	FRotator CurrentRotation = GetActorRotation();
	FString RotationMessage = FString::Printf(TEXT("Dice Rotation: Pitch=%.1f, Yaw=%.1f, Roll=%.1f"), 
		CurrentRotation.Pitch, CurrentRotation.Yaw, CurrentRotation.Roll);
	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, RotationMessage);
	UE_LOG(LogTemp, Warning, TEXT("Current Dice Rotation: %s"), *RotationMessage);
}
#pragma endregion Public Function

#pragma region Protected Function
void ALudoDice::OnThrowComplete()
{
	if (HasAuthority())
	{
		bIsThrowing = false;
		bHasFinishedThrowing = true;
		
		// Determine final dice face
		DetermineDiceFace();
		
		// Notify clients
		Client_OnThrowComplete();
	}
}
void ALudoDice::DetermineDiceFace()
{
	if (!HasAuthority()) return;

	// Get the current rotation of the dice
	FRotator DiceRotation = GetActorRotation();

	// Normalize rotation to 0-360 degrees
	float Pitch = FMath::Fmod(DiceRotation.Pitch + 360.0f, 360.0f);
	float Yaw = FMath::Fmod(DiceRotation.Yaw + 360.0f, 360.0f);
	float Roll = FMath::Fmod(DiceRotation.Roll + 360.0f, 360.0f);

	// Tolerance for angle detection (based on your data)
	const float Tolerance = 5.0f; // 5 degrees tolerance

	int32 NewDiceValue = 6; // Default face

	// Face detection based on your actual data patterns
	// Check Roll first since it's more distinctive in your data
	if (FMath::Abs(Roll - 90.0f) <= Tolerance)
	{
		// Face 4: Roll=90° (your data shows this clearly)
		NewDiceValue = 4;
	}
	else if (FMath::Abs(Roll - 270.0f) <= Tolerance || FMath::Abs(Roll - (-90.0f)) <= Tolerance)
	{
		// Face 3: Roll=270° or -90°
		NewDiceValue = 3;
	}
	else if (FMath::Abs(Roll - 180.0f) <= Tolerance || FMath::Abs(Roll - (-180.0f)) <= Tolerance)
	{
		// Face 1: Roll=180° or -180°
		NewDiceValue = 1;
	}
	else if (FMath::Abs(Pitch - 90.0f) <= Tolerance)
	{
		// Face 5: Pitch=90°
		NewDiceValue = 5;
	}
	else if (FMath::Abs(Pitch - 270.0f) <= Tolerance || FMath::Abs(Pitch - (-90.0f)) <= Tolerance)
	{
		// Face 2: Pitch=270° or -90°
		NewDiceValue = 2;
	}
	else if (FMath::Abs(Pitch) <= Tolerance && FMath::Abs(Roll) <= Tolerance)
	{
		// Face 6: Pitch=0°, Roll=0° (default)
		NewDiceValue = 6;
	}
	else
	{
		// Fallback: determine based on dominant rotation
		if (Pitch > 45.0f && Pitch <= 135.0f)
		{
			NewDiceValue = 5; // Face 5 up
		}
		else if (Pitch > 135.0f && Pitch <= 225.0f)
		{
			NewDiceValue = 1; // Face 1 up (opposite)
		}
		else if (Pitch > 225.0f && Pitch <= 315.0f)
		{
			NewDiceValue = 2; // Face 2 up
		}
		else
		{
			NewDiceValue = 6; // Default face 6
		}
	}

	// Set the dice value
	Server_SetDiceValue(NewDiceValue);

	// Print debug message with rotation info
	FString DebugMessage = FString::Printf(TEXT("Dice Face: %d (Pitch=%.1f, Yaw=%.1f, Roll=%.1f)"),
		NewDiceValue, Pitch, Yaw, Roll);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, DebugMessage);

	UE_LOG(LogTemp, Warning, TEXT("Dice Face Determined: %d (Pitch=%.1f, Yaw=%.1f, Roll=%.1f)"),
		NewDiceValue, Pitch, Yaw, Roll);
}
#pragma endregion Protected Function

#pragma region Private Function
void ALudoDice::Server_ThrowDice_Implementation()
{
	if (bIsThrowing) return; // Already throwing
	
	// Reset dice state
	bIsThrowing = true;
	bHasFinishedThrowing = false;
	CurrentDiceValue = 1;
	
	// Generate deterministic throw parameters
	FVector ThrowDirection = FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 1.0f);
	ThrowDirection.Normalize();
	
	FVector AngularImpulse = FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f));
	AngularImpulse.Normalize();
	
	// Set replicated parameters for all clients
	ReplicatedThrowDirection = ThrowDirection;
	ReplicatedAngularImpulse = AngularImpulse;
	ReplicatedThrowForce = ThrowForce;
	ReplicatedRotationForce = RotationForce;
	
	// Reset position
	SetActorLocation(OriginalLocation);
	SetActorRotation(OriginalRotation);
	
	// Stop any existing physics
	DiceMeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
	DiceMeshComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	
	// Apply forces on server
	DiceMeshComponent->AddImpulse(ThrowDirection * ThrowForce, NAME_None, true);
	DiceMeshComponent->AddAngularImpulseInDegrees(AngularImpulse * RotationForce, NAME_None, true);
	
	// Send physics parameters to all clients for deterministic simulation
	Client_SimulateDicePhysics(ThrowDirection, AngularImpulse, ThrowForce, RotationForce);
	
	// Set timer for throw completion
	GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle, this, &ALudoDice::OnThrowComplete, ThrowDuration, false);
	
	// Notify all clients about the throw
	Client_OnDiceThrown();
	
	// Print debug message
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Dice Thrown!"));
	UE_LOG(LogTemp, Warning, TEXT("Dice Thrown on Server"));
}

void ALudoDice::Server_SetDiceValue_Implementation(int32 NewValue)
{
	CurrentDiceValue = FMath::Clamp(NewValue, 1, 6);
	
	// Print debug message
	FString DebugMessage = FString::Printf(TEXT("Server: Dice Value Set to %d"), CurrentDiceValue);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, DebugMessage);
	UE_LOG(LogTemp, Warning, TEXT("Server: Dice Value = %d"), CurrentDiceValue);
}

void ALudoDice::Client_OnDiceThrown_Implementation()
{
	// Client-side dice throw notification
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, TEXT("Dice is Rolling!"));
	UE_LOG(LogTemp, Warning, TEXT("Client: Dice Thrown Animation"));
}

void ALudoDice::Client_SimulateDicePhysics_Implementation(FVector ThrowDir, FVector AngularImp, float ThrowF, float RotationF)
{
	// Only simulate on clients (not server)
	if (HasAuthority()) return;
	
	// Reset position and physics
	SetActorLocation(OriginalLocation);
	SetActorRotation(OriginalRotation);
	DiceMeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
	DiceMeshComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	
	// Apply the same forces as server for deterministic simulation
	DiceMeshComponent->AddImpulse(ThrowDir * ThrowF, NAME_None, true);
	DiceMeshComponent->AddAngularImpulseInDegrees(AngularImp * RotationF, NAME_None, true);
	
	UE_LOG(LogTemp, Warning, TEXT("Client: Simulating Dice Physics"));
}

void ALudoDice::Client_OnThrowComplete_Implementation()
{
	// Client-side throw completion
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("Dice Throw Complete!"));
	UE_LOG(LogTemp, Warning, TEXT("Client: Dice Throw Complete"));
}
#pragma endregion Private Function

#pragma region Events Function
void ALudoDice::OnRep_DiceValueChanged()
{
	// Called when dice value is replicated to clients
	FString DebugMessage = FString::Printf(TEXT("Dice Value Replicated: %d"), CurrentDiceValue);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, DebugMessage);
	UE_LOG(LogTemp, Warning, TEXT("Dice Value Replicated: %d"), CurrentDiceValue);
}

void ALudoDice::OnRep_ThrowingStateChanged()
{
	// Called when throwing state is replicated to clients
	FString StateMessage = bIsThrowing ? TEXT("Dice is Throwing...") : TEXT("Dice Stopped Throwing");
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, StateMessage);
	UE_LOG(LogTemp, Warning, TEXT("Throwing State Changed: %s"), bIsThrowing ? TEXT("Throwing") : TEXT("Stopped"));
}

void ALudoDice::OnRep_PhysicsParametersChanged()
{
	// Called when physics parameters are replicated to clients
	if (!HasAuthority() && bIsThrowing)
	{
		// Simulate physics on client when parameters are received
		Client_SimulateDicePhysics(ReplicatedThrowDirection, ReplicatedAngularImpulse, ReplicatedThrowForce, ReplicatedRotationForce);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Physics Parameters Replicated"));
}
#pragma endregion Events Function
