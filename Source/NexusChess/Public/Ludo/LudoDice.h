#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "LudoDice.generated.h"

UCLASS()
class NEXUSCHESS_API ALudoDice : public AActor
{
	GENERATED_BODY()

public:
#pragma region System Function
	ALudoDice(const FObjectInitializer& objectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion System Function

#pragma region Public Variables
	// Dice face values (1-6)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Dice")
	int32 CurrentDiceValue = 1;

	// Is dice currently being thrown
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Dice")
	bool bIsThrowing = false;

	// Has dice finished throwing and showing result
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Dice")
	bool bHasFinishedThrowing = false;

	// Deterministic physics parameters (replicated to all clients)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Dice")
	FVector ReplicatedThrowDirection = FVector::ZeroVector;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Dice")
	FVector ReplicatedAngularImpulse = FVector::ZeroVector;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Dice")
	float ReplicatedThrowForce = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Dice")
	float ReplicatedRotationForce = 0.0f;
#pragma endregion Public Variables

protected:
#pragma region Protected Variables
#pragma endregion Protected Variables

private:
#pragma region Private Variables
	// Static mesh component for the dice
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DiceMeshComponent;

	// Material for the dice
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* DiceMaterial;

	// Throwing force
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice", meta = (AllowPrivateAccess = "true"))
	float ThrowForce = 500.0f;

	// Rotation force for spinning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice", meta = (AllowPrivateAccess = "true"))
	float RotationForce = 1000.0f;

	// Time to wait before determining final face
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice", meta = (AllowPrivateAccess = "true"))
	float ThrowDuration = 3.0f;

	// Timer handle for throw completion
	FTimerHandle ThrowTimerHandle;

	// Original location for reset
	FVector OriginalLocation;
	FRotator OriginalRotation;
#pragma endregion Private Variables

public:
#pragma region Public Function
	// Throw the dice (call this to start throwing)
	UFUNCTION(BlueprintCallable, Category = "Dice")
	void ThrowDice();

	// Reset dice to original position
	UFUNCTION(BlueprintCallable, Category = "Dice")
	void ResetDice();

	// Get the current dice value
	UFUNCTION(BlueprintCallable, Category = "Dice")
	int32 GetDiceValue() const { return CurrentDiceValue; }

	// Check if dice has finished throwing
	UFUNCTION(BlueprintCallable, Category = "Dice")
	bool IsDiceFinished() const { return bHasFinishedThrowing; }

	// Debug function to show current dice rotation
	UFUNCTION(BlueprintCallable, Category = "Dice")
	void ShowCurrentRotation();
#pragma endregion Public Function

protected:
#pragma region Protected Function
	// Called when throw timer expires
	UFUNCTION()
	void OnThrowComplete();

	// Determine which face is up and set the dice value
	void DetermineDiceFace();
#pragma endregion Protected Function

private:
#pragma region Private Function
	// Server function to handle dice throwing
	UFUNCTION(Server, Reliable)
	void Server_ThrowDice();

	// Server function to set dice value
	UFUNCTION(Server, Reliable)
	void Server_SetDiceValue(int32 NewValue);

	// Client function to handle throw completion
	UFUNCTION(Client, Reliable)
	void Client_OnThrowComplete();

	// Client function to handle dice thrown animation
	UFUNCTION(Client, Reliable)
	void Client_OnDiceThrown();

	// Client function to simulate deterministic physics
	UFUNCTION(Client, Reliable)
	void Client_SimulateDicePhysics(FVector ThrowDir, FVector AngularImp, float ThrowF, float RotationF);
#pragma endregion Private Function

#pragma region Events Function
	// Called when dice value changes
	UFUNCTION()
	void OnRep_DiceValueChanged();

	// Called when throwing state changes
	UFUNCTION()
	void OnRep_ThrowingStateChanged();

	// Called when physics parameters are replicated
	UFUNCTION()
	void OnRep_PhysicsParametersChanged();
#pragma endregion Events Function
};
