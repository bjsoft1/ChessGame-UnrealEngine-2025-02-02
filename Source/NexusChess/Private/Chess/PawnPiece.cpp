#include "Chess/PawnPiece.h"

APawnPiece::APawnPiece(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{

}

bool APawnPiece::GetPosibleEnpassant()
{
	int32 y = GetTileY();
	EChessPlayers color = GetPieceOwner();
	UE_LOG(LogTemp, Warning, TEXT("Pawn-Piece: 0000000000000000000000 || X: %d, Y: %d || Color: %s"), GetTileX(), y, *UEnum::GetValueAsString(color));

	if (GetMoveCount() == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn-Piece: 1111111111111111111111111"));
		if ((GetPieceOwner() == EChessPlayers::White && y == 3) || (GetPieceOwner() == EChessPlayers::Black && y == 4))
			return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Pawn-Piece: 2222222222222222222222222222222"));
	return false;
}
