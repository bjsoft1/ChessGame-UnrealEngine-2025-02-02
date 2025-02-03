#pragma once
#include "GameFramework/Actor.h"
#include "ChessComponents.h"
#include "ChessPiece.generated.h"
UCLASS()
class NEXUSCHESS_API AChessPiece : public AActor
{
	GENERATED_BODY()

public:
#pragma region System Function
	AChessPiece(const FObjectInitializer& objectInitializer);
#pragma endregion System Function

#pragma region Public Variables
#pragma endregion Public Variables

protected:
#pragma region protected Variables
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
#pragma endregion protected Variables

private:
#pragma region Private Variables
	class AChessBoard* _chessBoard;
	UStaticMeshComponent* _chessPieceMeshComponent;
	EChessPieceTypes _pieceType;
	EChessPlayers _pieceOwner;
	//---------------------------
	int32 _tileX = -1;
	int32 _tileY = -1;
	//---------------------------
	FVector _newLocation;
	//---------------------------
#pragma endregion Private Variables

public:
#pragma region Public Function
	void SetPieceOwner(EChessPlayers player);
	void SetPieceType(EChessPieceTypes type);
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() { return this->_chessPieceMeshComponent; }
	FORCEINLINE void SetChessBoard(AChessBoard* chessBoard) { this->_chessBoard = chessBoard; };
	void SetTileIndex(int32 x, int32 y);
	FORCEINLINE EChessPieceTypes GetPieceType() { return this->_pieceType; }
	FORCEINLINE EChessPlayers GetPieceOwner() { return this->_pieceOwner; }
	FORCEINLINE int32 GetTileX() { return this->_tileX; }
	FORCEINLINE int32 GetTileY() { return this->_tileY; }
#pragma endregion Public Function

protected:
#pragma region Protected Function
#pragma endregion Protected Function

private:
#pragma region Private Function
#pragma endregion Private Function

#pragma region Events Function
	UFUNCTION() void OnMouseClick(UPrimitiveComponent* touchedComponent, FKey buttonPressed);
#pragma endregion Events Function
};

