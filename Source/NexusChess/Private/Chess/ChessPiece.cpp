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
	this->_pieceOwner = player;
	this->_chessPieceMeshComponent->SetMaterial(0, this->_chessBoard->GetPieceMaterial(player));
}
void AChessPiece::SetPieceType(EChessPieceTypes type)
{
	this->_pieceType = type;
	this->_chessPieceMeshComponent->SetStaticMesh(this->_chessBoard->GetPieceMesh(type));
}
void AChessPiece::SetTileIndex(int32 x, int32 y)
{
	this->_newLocation = this->_chessBoard->GetTileWorldPosition(x, y, this);
	this->_tileX = x;
	this->_tileY = y;
	SetActorTickEnabled(true); 
}
#pragma endregion Public Function

#pragma region Protected Function
void AChessPiece::BeginPlay()
{
	Super::BeginPlay();

	this->_chessPieceMeshComponent->OnClicked.AddDynamic(this, &ThisClass::OnMouseClick);
}
void AChessPiece::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	const FVector oldLocation = GetActorLocation();
	const FVector newLocation = FMath::VInterpConstantTo(oldLocation, _newLocation, deltaTime, 300.0f);
	SetActorLocation(newLocation);

	if (newLocation == this->_newLocation)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Location:%s"), *newLocation.ToString()));
		SetActorTickEnabled(false);
	}
}
#pragma endregion Protected Function

#pragma region Private Function
#pragma endregion Private Function

#pragma region Events Function
void AChessPiece::OnMouseClick(UPrimitiveComponent* touchedComponent, FKey buttonPressed)
{
	this->_chessBoard->SetClickTile(this->_tileX, this->_tileY);
}
#pragma endregion Events Function
