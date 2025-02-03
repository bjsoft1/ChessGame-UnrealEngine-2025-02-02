#include "Controls/ChessModeBase.h"
#include "Controls/ChessController.h"
#include "Players/ChessPawn.h"

#pragma region System Function
AChessModeBase::AChessModeBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	DefaultPawnClass = AChessPawn::StaticClass();
	PlayerControllerClass = AChessController::StaticClass();
}
#pragma endregion System Function

#pragma region Public Function

#pragma endregion Public Function

#pragma region Protected Function
#pragma endregion Protected Function

#pragma region Private Function
#pragma endregion Private Function

#pragma region Events Function
#pragma endregion Events Function
