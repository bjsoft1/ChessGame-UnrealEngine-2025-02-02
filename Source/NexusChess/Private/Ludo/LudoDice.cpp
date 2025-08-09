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
		
		if (Velocity.Size() < 10.0f && AngularVelocity.Size() < 10.0f)
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
	
	// Convert to degrees for easier calculation
	float Pitch = FMath::Fmod(DiceRotation.Pitch + 360.0f, 360.0f);
	float Roll = FMath::Fmod(DiceRotation.Roll + 360.0f, 360.0f);
	float Yaw = FMath::Fmod(DiceRotation.Yaw + 360.0f, 360.0f);
	
	// Determine which face is up based on rotation
	// This is a simplified calculation - you may need to adjust based on your dice model
	int32 NewDiceValue = 1;
	
	// Simple face determination based on pitch (forward/backward tilt)
	if (Pitch > 315.0f || Pitch <= 45.0f)
	{
		NewDiceValue = 1; // Face 1 up
	}
	else if (Pitch > 45.0f && Pitch <= 135.0f)
	{
		NewDiceValue = 2; // Face 2 up
	}
	else if (Pitch > 135.0f && Pitch <= 225.0f)
	{
		NewDiceValue = 3; // Face 3 up
	}
	else if (Pitch > 225.0f && Pitch <= 315.0f)
	{
		NewDiceValue = 4; // Face 4 up
	}
	
	// Adjust based on Yaw (left/right rotation) for more faces
	if (Yaw > 45.0f && Yaw <= 135.0f)
	{
		NewDiceValue = (NewDiceValue % 6) + 1;
	}
	else if (Yaw > 135.0f && Yaw <= 225.0f)
	{
		NewDiceValue = ((NewDiceValue + 2) % 6) + 1;
	}
	else if (Yaw > 225.0f && Yaw <= 315.0f)
	{
		NewDiceValue = ((NewDiceValue + 3) % 6) + 1;
	}
	
	// Set the dice value
	Server_SetDiceValue(NewDiceValue);
	
	// Print debug message
	FString DebugMessage = FString::Printf(TEXT("Dice Thrown! Face Value: %d"), NewDiceValue);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, DebugMessage);
	
	UE_LOG(LogTemp, Warning, TEXT("Dice Face Determined: %d"), NewDiceValue);
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
	
	// Reset position
	SetActorLocation(OriginalLocation);
	SetActorRotation(OriginalRotation);
	
	// Stop any existing physics
	DiceMeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
	DiceMeshComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	
	// Apply random force for throwing
	FVector ThrowDirection = FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 1.0f);
	ThrowDirection.Normalize();
	
	// Apply linear force
	DiceMeshComponent->AddImpulse(ThrowDirection * ThrowForce, NAME_None, true);
	
	// Apply angular force for spinning
	FVector AngularImpulse = FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f));
	AngularImpulse.Normalize();
	DiceMeshComponent->AddAngularImpulseInDegrees(AngularImpulse * RotationForce, NAME_None, true);
	
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
#pragma endregion Events Function
