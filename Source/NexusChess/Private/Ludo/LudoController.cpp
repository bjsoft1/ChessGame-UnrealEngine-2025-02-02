#include "Ludo/LudoController.h"
#include "Ludo/LudoGameMode.h"
#include "Ludo/LudoDice.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

#pragma region System Function
ALudoController::ALudoController(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	// Enable replication
	bReplicates = true;
}

void ALudoController::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to game mode
	LudoGameMode = Cast<ALudoGameMode>(GetWorld()->GetAuthGameMode());

	// Initialize player
	InitializePlayer();
}

void ALudoController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind input actions
	InputComponent->BindAction("ThrowDice", IE_Pressed, this, &ALudoController::OnThrowDicePressed);
	InputComponent->BindAction("EndTurn", IE_Pressed, this, &ALudoController::OnEndTurnPressed);
}

void ALudoController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate player properties
	DOREPLIFETIME(ALudoController, PlayerIndex);
	DOREPLIFETIME(ALudoController, PlayerName);
	DOREPLIFETIME(ALudoController, bIsMyTurn);
	DOREPLIFETIME(ALudoController, PlayerColor);
}
#pragma endregion System Function

#pragma region Public Function
void ALudoController::SetPlayerIndex(int32 Index)
{
	if (HasAuthority())
	{
		Server_SetPlayerIndex(Index);
	}
	else
	{
		Server_SetPlayerIndex(Index);
	}
}

void ALudoController::SetPlayerName(const FString& Name)
{
	if (HasAuthority())
	{
		Server_SetPlayerName(Name);
	}
	else
	{
		Server_SetPlayerName(Name);
	}
}

void ALudoController::SetPlayerColor(const FLinearColor& Color)
{
	if (HasAuthority())
	{
		Server_SetPlayerColor(Color);
	}
	else
	{
		Server_SetPlayerColor(Color);
	}
}
#pragma endregion Public Function

#pragma region Protected Function
void ALudoController::InitializePlayer()
{
	// Set default player name if not set
	if (PlayerName.IsEmpty())
	{
		PlayerName = FString::Printf(TEXT("Player %d"), PlayerIndex + 1);
	}

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("Player %s initialized!"), *PlayerName));
}
#pragma endregion Protected Function

#pragma region Private Function
void ALudoController::OnThrowDicePressed()
{
	// Only allow dice throwing if it's this player's turn
	if (!bIsMyTurn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Not your turn!"));
		return;
	}

	// Get the game dice from game mode
	if (LudoGameMode)
	{
		ALudoDice* GameDice = LudoGameMode->GetGameDice();
		if (GameDice)
		{
			GameDice->ThrowDice();
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, FString::Printf(TEXT("%s threw the dice!"), *PlayerName));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("No dice found in game!"));
		}
	}
}

void ALudoController::OnEndTurnPressed()
{
	// Only allow ending turn if it's this player's turn
	if (!bIsMyTurn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Not your turn!"));
		return;
	}

	// End turn and move to next player
	if (LudoGameMode)
	{
		LudoGameMode->NextTurn();
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("%s ended their turn!"), *PlayerName));
	}
}

void ALudoController::Server_SetPlayerIndex_Implementation(int32 Index)
{
	PlayerIndex = Index;
	
	// Print debug message
	FString DebugMessage = FString::Printf(TEXT("Server: Player index set to %d"), Index);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, DebugMessage);
}

void ALudoController::Server_SetPlayerName_Implementation(const FString& Name)
{
	PlayerName = Name;
	
	// Print debug message
	FString DebugMessage = FString::Printf(TEXT("Server: Player name set to %s"), *Name);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, DebugMessage);
}

void ALudoController::Server_SetPlayerColor_Implementation(const FLinearColor& Color)
{
	PlayerColor = Color;
	
	// Print debug message
	FString DebugMessage = FString::Printf(TEXT("Server: Player color set to RGB(%.1f, %.1f, %.1f)"), Color.R, Color.G, Color.B);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, DebugMessage);
}
#pragma endregion Private Function

#pragma region Events Function
void ALudoController::OnRep_PlayerIndexChanged()
{
	// Called when player index is replicated to clients
	FString DebugMessage = FString::Printf(TEXT("Player Index Replicated: %d"), PlayerIndex);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, DebugMessage);
}

void ALudoController::OnRep_TurnStateChanged()
{
	// Called when turn state is replicated to clients
	FString TurnMessage = bIsMyTurn ? FString::Printf(TEXT("It's %s's turn!"), *PlayerName) : TEXT("Not your turn");
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TurnMessage);
}
#pragma endregion Events Function
