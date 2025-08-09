#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LudoController.generated.h"

UCLASS()
class NEXUSCHESS_API ALudoController : public APlayerController
{
	GENERATED_BODY()

public:
#pragma region System Function
	ALudoController(const FObjectInitializer& objectInitializer);
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
#pragma endregion System Function

#pragma region Public Variables
	// Player index (0-3 for 4 players)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ludo Player")
	int32 PlayerIndex = -1;

	// Player name
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ludo Player")
	FString PlayerName = TEXT("Player");

	// Is this player's turn
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ludo Player")
	bool bIsMyTurn = false;

	// Player color
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ludo Player")
	FLinearColor PlayerColor = FLinearColor::Red;
#pragma endregion Public Variables

protected:
#pragma region Protected Variables
	// Reference to the game mode
	UPROPERTY()
	class ALudoGameMode* LudoGameMode;
#pragma endregion Protected Variables

public:
#pragma region Public Function
	// Set player index
	UFUNCTION(BlueprintCallable, Category = "Ludo Player")
	void SetPlayerIndex(int32 Index);

	// Set player name
	UFUNCTION(BlueprintCallable, Category = "Ludo Player")
	void SetPlayerName(const FString& Name);

	// Set player color
	UFUNCTION(BlueprintCallable, Category = "Ludo Player")
	void SetPlayerColor(const FLinearColor& Color);

	// Check if it's this player's turn
	UFUNCTION(BlueprintCallable, Category = "Ludo Player")
	bool IsMyTurn() const { return bIsMyTurn; }

	// Get player index
	UFUNCTION(BlueprintCallable, Category = "Ludo Player")
	int32 GetPlayerIndex() const { return PlayerIndex; }
#pragma endregion Public Function

protected:
#pragma region Protected Function
	// Initialize player
	virtual void InitializePlayer();
#pragma endregion Protected Function

private:
#pragma region Private Function
	// Handle dice throw input
	UFUNCTION()
	void OnThrowDicePressed();

	// Handle end turn input
	UFUNCTION()
	void OnEndTurnPressed();

	// Server function to set player index
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerIndex(int32 Index);

	// Server function to set player name
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerName(const FString& Name);

	// Server function to set player color
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerColor(const FLinearColor& Color);
#pragma endregion Private Function

#pragma region Events Function
	// Called when player index changes
	UFUNCTION()
	void OnRep_PlayerIndexChanged();

	// Called when turn state changes
	UFUNCTION()
	void OnRep_TurnStateChanged();
#pragma endregion Events Function
};
