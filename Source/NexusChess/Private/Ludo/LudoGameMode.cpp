#include "Ludo/LudoGameMode.h"
#include "Ludo/LudoController.h"
#include "Ludo/LudoDice.h"
#include "Players/LudoPawn.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

#pragma region System Function
ALudoGameMode::ALudoGameMode(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	// Set default classes
	PlayerControllerClass = ALudoController::StaticClass();
	DefaultPawnClass = ALudoPawn::StaticClass();

	// Enable replication
	bReplicates = true;
}

void ALudoGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Initialize game on server
		InitializeGame();
	}
}

void ALudoGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ALudoController* LudoController = Cast<ALudoController>(NewPlayer);
	if (LudoController)
	{
		// Add to player controllers array
		PlayerControllers.Add(LudoController);

		// Set player index based on order of joining
		int32 PlayerIndex = PlayerControllers.Num() - 1;
		LudoController->SetPlayerIndex(PlayerIndex);

		// Set player colors
		FLinearColor PlayerColors[4] = {
			FLinearColor::Red,
			FLinearColor::Green,
			FLinearColor::Blue,
			FLinearColor::Yellow
		};

		if (PlayerIndex < 4)
		{
			LudoController->SetPlayerColor(PlayerColors[PlayerIndex]);
		}

		// Set player name
		FString PlayerName = FString::Printf(TEXT("Player %d"), PlayerIndex + 1);
		LudoController->SetPlayerName(PlayerName);

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Player %d joined the game!"), PlayerIndex + 1));

		// Start game when enough players join
		if (PlayerControllers.Num() >= 2 && !bGameStarted)
		{
			StartGame();
		}
	}
}

void ALudoGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ALudoController* LudoController = Cast<ALudoController>(Exiting);
	if (LudoController)
	{
		PlayerControllers.Remove(LudoController);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Player left the game!"));
	}
}

void ALudoGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate game state
	DOREPLIFETIME(ALudoGameMode, CurrentPlayerTurn);
	DOREPLIFETIME(ALudoGameMode, bGameStarted);
}
#pragma endregion System Function

#pragma region Public Function
void ALudoGameMode::StartGame()
{
	if (HasAuthority())
	{
		Server_StartGame();
	}
	else
	{
		Server_StartGame();
	}
}

void ALudoGameMode::NextTurn()
{
	if (HasAuthority())
	{
		Server_NextTurn();
	}
	else
	{
		Server_NextTurn();
	}
}

ALudoController* ALudoGameMode::GetCurrentPlayerController() const
{
	if (PlayerControllers.IsValidIndex(CurrentPlayerTurn))
	{
		return PlayerControllers[CurrentPlayerTurn];
	}
	return nullptr;
}

bool ALudoGameMode::IsPlayerTurn(int32 PlayerIndex) const
{
	return CurrentPlayerTurn == PlayerIndex;
}
#pragma endregion Public Function

#pragma region Protected Function
void ALudoGameMode::InitializeGame()
{
	// Spawn dice
	SpawnDice();

	// Setup initial turn
	SetupPlayerTurn();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Ludo Game Initialized!"));
}

void ALudoGameMode::SpawnDice()
{
	if (HasAuthority() && DiceClass)
	{
		// Spawn dice at a specific location
		FVector DiceLocation = FVector(0.0f, 0.0f, 100.0f);
		FRotator DiceRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GameDice = GetWorld()->SpawnActor<ALudoDice>(DiceClass, DiceLocation, DiceRotation, SpawnParams);

		if (GameDice)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Dice spawned successfully!"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Failed to spawn dice!"));
		}
	}
}

void ALudoGameMode::SetupPlayerTurn()
{
	if (HasAuthority())
	{
		// Reset all players' turn state
		for (ALudoController* Controller : PlayerControllers)
		{
			if (Controller)
			{
				Controller->bIsMyTurn = false;
			}
		}

		// Set current player's turn
		if (PlayerControllers.IsValidIndex(CurrentPlayerTurn))
		{
			ALudoController* CurrentController = PlayerControllers[CurrentPlayerTurn];
			if (CurrentController)
			{
				CurrentController->bIsMyTurn = true;
			}
		}

		// Notify all players about turn change
		OnTurnChanged();
	}
}
#pragma endregion Protected Function

#pragma region Private Function
void ALudoGameMode::Server_StartGame_Implementation()
{
	if (bGameStarted) return;

	bGameStarted = true;
	CurrentPlayerTurn = 0;

	// Setup first player's turn
	SetupPlayerTurn();

	// Notify all players
	OnGameStarted();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Ludo Game Started!"));
}

void ALudoGameMode::Server_NextTurn_Implementation()
{
	if (!bGameStarted) return;

	// Move to next player
	CurrentPlayerTurn = (CurrentPlayerTurn + 1) % PlayerControllers.Num();

	// Setup new player's turn
	SetupPlayerTurn();

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::Printf(TEXT("Turn changed to Player %d"), CurrentPlayerTurn + 1));
}
#pragma endregion Private Function

#pragma region Events Function
void ALudoGameMode::OnGameStarted()
{
	// Notify all clients about game start
	for (ALudoController* Controller : PlayerControllers)
	{
		if (Controller)
		{
			// You can add client-specific game start notifications here
		}
	}
}

void ALudoGameMode::OnTurnChanged()
{
	// Notify all clients about turn change
	for (ALudoController* Controller : PlayerControllers)
	{
		if (Controller)
		{
			// You can add client-specific turn change notifications here
		}
	}
}
#pragma endregion Events Function
