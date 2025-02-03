#include "Chess/ChessPiece.h"
#include "Chess/ChessBoard.h"

#pragma region System Function
AChessPiece::AChessPiece(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	this->_chessPieceMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ChessPieceMeshComponent");
	AActor::RootComponent = this->_chessPieceMeshComponent;
}
#pragma endregion System Function

#pragma region Public Function
void AChessPiece::SetPieceOwner(EChessPlayers player)
{
	this->_pieceOwner = &player;
	this->_chessPieceMeshComponent->SetMaterial(0, this->_chessBoard->GetPieceMaterial(player));
}
void AChessPiece::SetPieceType(EChessPieceTypes type)
{
	this->_pieceType = &type;
	this->_chessPieceMeshComponent->SetStaticMesh(this->_chessBoard->GetPieceMesh(type));
}
#pragma endregion Public Function

#pragma region Protected Function
void AChessPiece::BeginPlay()
{
	Super::BeginPlay();
}
void AChessPiece::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}
#pragma endregion Protected Function

#pragma region Private Function
#pragma endregion Private Function

#pragma region Events Function
#pragma endregion Events Function
