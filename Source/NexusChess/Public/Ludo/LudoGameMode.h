#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LudoGameMode.generated.h"

UCLASS()
class NEXUSCHESS_API ALudoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
#pragma region System Function
	ALudoGameMode(const FObjectInitializer& objectInitializer);
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
#pragma endregion System Function

#pragma region Public Variables
	// Number of players for Ludo game
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ludo Game")
	int32 MaxPlayers = 4;

	// Current player turn
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ludo Game")
	int32 CurrentPlayerTurn = 0;

	// Game state
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ludo Game")
	bool bGameStarted = false;

	// Dice reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ludo Game")
	TSubclassOf<class ALudoDice> DiceClass;
#pragma endregion Public Variables

protected:
#pragma region Protected Variables
	// Array of player controllers
	UPROPERTY()
	TArray<class ALudoController*> PlayerControllers;

	// Game dice
	UPROPERTY()
	class ALudoDice* GameDice;
#pragma endregion Protected Variables

public:
#pragma region Public Function
	// Start the Ludo game
	UFUNCTION(BlueprintCallable, Category = "Ludo Game")
	void StartGame();

	// End the current turn and move to next player
	UFUNCTION(BlueprintCallable, Category = "Ludo Game")
	void NextTurn();

	// Get current player controller
	UFUNCTION(BlueprintCallable, Category = "Ludo Game")
	class ALudoController* GetCurrentPlayerController() const;

	// Get the game dice
	UFUNCTION(BlueprintCallable, Category = "Ludo Game")
	class ALudoDice* GetGameDice() const { return GameDice; }

	// Check if it's a specific player's turn
	UFUNCTION(BlueprintCallable, Category = "Ludo Game")
	bool IsPlayerTurn(int32 PlayerIndex) const;
#pragma endregion Public Function

protected:
#pragma region Protected Function
	// Initialize the game
	virtual void InitializeGame();

	// Spawn the dice
	void SpawnDice();

	// Setup player turn
	void SetupPlayerTurn();
#pragma endregion Protected Function

private:
#pragma region Private Function
	// Server function to start game
	UFUNCTION(Server, Reliable)
	void Server_StartGame();

	// Server function to next turn
	UFUNCTION(Server, Reliable)
	void Server_NextTurn();
#pragma endregion Private Function

#pragma region Events Function
	// Called when game starts
	UFUNCTION()
	void OnGameStarted();

	// Called when turn changes
	UFUNCTION()
	void OnTurnChanged();
#pragma endregion Events Function
};
