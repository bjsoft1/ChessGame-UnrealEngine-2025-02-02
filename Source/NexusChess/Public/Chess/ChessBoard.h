#pragma once

#include "GameFramework/Actor.h"
#include "ChessComponents.h"
#include "ChessBoard.generated.h"

UCLASS()
class NEXUSCHESS_API AChessBoard : public AActor
{
	GENERATED_BODY()

public:
#pragma region System Function
	AChessBoard(const FObjectInitializer& objectInitializer);
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
	UStaticMeshComponent* _chessBoardMeshComponent;
	//----------------------------
	UMaterial* _pieceWhiteMaterial;
	UMaterial* _pieceBlackMaterial;
	//----------------------------
	UStaticMesh* _piecePawn;
	UStaticMesh* _pieceRook;
	UStaticMesh* _pieceKnight;
	UStaticMesh* _pieceBishop;
	UStaticMesh* _pieceQueen;
	UStaticMesh* _pieceKing;
	//----------------------------
	int32 _hoveredTileX = -1;
	int32 _hoveredTileY = -1;

	int32 _clickedTileX = -1;
	int32 _clickedTileY = -1;
	bool _isHovering = false;
	TArray<class AChessPiece*> _chessPieces;
	//----------------------------
#pragma endregion Private Variables

public:
#pragma region Public Function
	UStaticMesh* GetPieceMesh(EChessPieceTypes type);
	AChessPiece* FindChessPiece(int32 x, int32 y);
	void SetClickTile(int32 x, int32 y);
	FVector GetTileWorldPosition(int32 x, int32 y, AChessPiece* piece);
	bool IsFriendPiece(AChessPiece* a, AChessPiece* b);
	FORCEINLINE UMaterial* GetPieceMaterial(EChessPlayers player) { return player == EChessPlayers::White ? this->_pieceWhiteMaterial : this->_pieceBlackMaterial; };
	FORCEINLINE void SetHoverTile(int32 x, int32 y) { this->_hoveredTileX = x; this->_hoveredTileY = y; }
#pragma endregion Public Function

protected:
#pragma region Protected Function
#pragma endregion Protected Function

private:
#pragma region Private Function
	void DetectHoveredTile();
	AChessPiece* SpawnChessPiece(EChessPlayers owner, EChessPieceTypes type, int32 x, int32 y);
#pragma endregion Private Function

#pragma region Events Function
	UFUNCTION() void OnBeginMouseHover(UPrimitiveComponent* touchedComponent);
	UFUNCTION() void OnEndMouseHover(UPrimitiveComponent* touchedComponent);
	UFUNCTION() void OnMouseClick(UPrimitiveComponent* touchedComponent, FKey buttonPressed);
#pragma endregion Events Function
};
