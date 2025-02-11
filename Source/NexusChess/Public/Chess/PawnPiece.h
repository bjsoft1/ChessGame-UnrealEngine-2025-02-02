#pragma once
#include "Chess/ChessPiece.h"
#include "PawnPiece.generated.h"

UCLASS()
class NEXUSCHESS_API APawnPiece : public AChessPiece
{
	GENERATED_BODY()
	

public:
#pragma region System Function
	APawnPiece(const FObjectInitializer& objectInitializer);
#pragma endregion System Function

#pragma region Public Variables
#pragma endregion Public Variables

protected:
#pragma region protected Variables
#pragma endregion protected Variables

private:
#pragma region Private Variables
#pragma endregion Private Variables

public:
#pragma region Public Function
	bool GetPosibleEnpassant();
#pragma endregion Public Function

protected:
#pragma region Protected Function
#pragma endregion Protected Function

private:
#pragma region Private Function
#pragma endregion Private Function

#pragma region Events Function
#pragma endregion Events Function
};
