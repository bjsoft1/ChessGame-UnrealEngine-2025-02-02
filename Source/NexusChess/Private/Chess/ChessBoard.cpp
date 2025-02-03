#include "Chess/ChessBoard.h"
#include "Chess/ChessPiece.h"

#pragma region System Function
AChessBoard::AChessBoard(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Engine/BasicShapes/Plane"));

    this->_chessBoardMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ChessBoardMeshComponent");
    AActor::RootComponent = this->_chessBoardMeshComponent;
    this->_chessBoardMeshComponent->SetStaticMesh(meshAsset.Object);
    this->_chessBoardMeshComponent->SetRelativeScale3D(FVector(6.5f, 6.5f, 1.0f));

    ConstructorHelpers::FObjectFinder<UStaticMesh> piecePawn(TEXT("/Game/Nexus-Contents/Static-Mesh/SM_Pawn"));
    ConstructorHelpers::FObjectFinder<UStaticMesh> pieceRook(TEXT("/Game/Nexus-Contents/Static-Mesh/SM_Rook"));
    ConstructorHelpers::FObjectFinder<UStaticMesh> pieceKnight(TEXT("/Game/Nexus-Contents/Static-Mesh/SM_Knight"));
    ConstructorHelpers::FObjectFinder<UStaticMesh> pieceBishop(TEXT("/Game/Nexus-Contents/Static-Mesh/SM_Bishop"));
    ConstructorHelpers::FObjectFinder<UStaticMesh> pieceQueen(TEXT("/Game/Nexus-Contents/Static-Mesh/SM_Queen"));
    ConstructorHelpers::FObjectFinder<UStaticMesh> pieceKing(TEXT("/Game/Nexus-Contents/Static-Mesh/SM_King"));
    
    ConstructorHelpers::FObjectFinder<UMaterial> black(TEXT("/Game/Nexus-Contents/Materials/M_Black"));
    ConstructorHelpers::FObjectFinder<UMaterial> white(TEXT("/Game/Nexus-Contents/Materials/M_White"));

    //----------------------------
    this->_piecePawn = piecePawn.Object;
    this->_pieceRook = pieceRook.Object;
    this->_pieceKnight = pieceKnight.Object;
    this->_pieceBishop = pieceBishop.Object;
    this->_pieceQueen = pieceQueen.Object;
    this->_pieceKing = pieceKing.Object;
    //----------------------------
    this->_pieceWhiteMaterial = white.Object;
    this->_pieceBlackMaterial = black.Object;
    //----------------------------
}
#pragma endregion System Function

#pragma region Public Function
UStaticMesh* AChessBoard::GetPieceMesh(EChessPieceTypes type)
{
    switch (type)
    {
    case EChessPieceTypes::Rook: return this->_pieceRook;
    case EChessPieceTypes::Knight: return this->_pieceKnight;
    case EChessPieceTypes::Bishop: return this->_pieceBishop;
    case EChessPieceTypes::Queen: return this->_pieceQueen;
    case EChessPieceTypes::King: return this->_pieceKing;
    default: return this->_piecePawn;
    }
}
AChessPiece* AChessBoard::FindChessPiece(int32 x, int32 y)
{
    for (AChessPiece* piece : this->_chessPieces)
    {
        if (piece != nullptr && piece->GetTileX() == x && piece->GetTileY() == y) return piece;
    }

    return nullptr;
}
void AChessBoard::SetClickTile(int32 x, int32 y)
{
    AChessPiece* oldSelectedPiece = FindChessPiece(this->_clickedTileX, this->_clickedTileY);
    this->_clickedTileX = x;
    this->_clickedTileY = y;

    if (oldSelectedPiece != nullptr)
    {        
        AChessPiece* existingPiece = FindChessPiece(x, y);
        if (!IsFriendPiece(oldSelectedPiece, existingPiece))
        {
            if (existingPiece != nullptr)
            {
                this->_chessPieces.Remove(existingPiece);
                existingPiece->Destroy(true);
            }

            // If Pawn piece then we need to determine is able to bron?
            if (oldSelectedPiece->GetPieceType() == EChessPieceTypes::Pawn)
            {
                if (oldSelectedPiece->GetPieceOwner() == EChessPlayers::White)
                {
                    if (y == 7) oldSelectedPiece->SetPieceType(EChessPieceTypes::Queen);
                }
                else if (oldSelectedPiece->GetPieceOwner() == EChessPlayers::Black)
                {
                    if (y == 0) oldSelectedPiece->SetPieceType(EChessPieceTypes::Queen);
                }
            }
            oldSelectedPiece->SetTileIndex(x, y);
        }

        this->_clickedTileX = -1;
        this->_clickedTileY = -1;
    }

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Clicked Tile: X=%d, Y=%d"), x, y));
}
FVector AChessBoard::GetTileWorldPosition(int32 x, int32 y, AChessPiece* piece)
{
    float divider = 2.0f;
    EChessPieceTypes type = piece->GetPieceType();
    switch (type)
    {
    case EChessPieceTypes::Rook:
    case EChessPieceTypes::Knight:
    case EChessPieceTypes::Bishop:
        divider = 2.50f;
        break;
    case EChessPieceTypes::Queen:
        divider = 2.70f;
        break;
    case EChessPieceTypes::King:
        divider = 2.60f;
        break;
    default:
        divider = 2.0f;
        break;
    }
    // Get the bounds of the chess piece mesh
    UStaticMeshComponent* chessPieceMesh = piece->GetMeshComponent();
    FVector pieceBounds = chessPieceMesh->GetStaticMesh()->GetBounds().BoxExtent / divider;

    FVector meshBounds = _chessBoardMeshComponent->GetStaticMesh()->GetBounds().BoxExtent * 2.0f;
    float boardSize = FMath::Max(meshBounds.X, meshBounds.Y);
    float tileSize = boardSize / 8;

    // Calculate the local position of the tile
    FVector localTilePosition = FVector((x * tileSize) - boardSize / 2, (y * tileSize) - boardSize / 2, 0);
    FVector adjustedLocalPosition = localTilePosition + FVector(pieceBounds.X / 2, pieceBounds.Y / 2, 0);

    // Convert the local position to world space using the chessboard's transform
    FVector worldTilePosition = _chessBoardMeshComponent->GetComponentTransform().TransformPosition(adjustedLocalPosition);

    return worldTilePosition;
}
bool AChessBoard::IsFriendPiece(AChessPiece* a, AChessPiece* b)
{
    if (a == nullptr || b == nullptr) return false;

    return a->GetPieceOwner() == b->GetPieceOwner();
}
#pragma endregion Public Function

#pragma region Protected Function
void AChessBoard::BeginPlay()
{
    Super::BeginPlay();
    this->_chessBoardMeshComponent->OnBeginCursorOver.AddDynamic(this, &ThisClass::OnBeginMouseHover);
    this->_chessBoardMeshComponent->OnEndCursorOver.AddDynamic(this, &ThisClass::OnEndMouseHover);
    this->_chessBoardMeshComponent->OnClicked.AddDynamic(this, &ThisClass::OnMouseClick);

    // Spawn Pawn Pices
    for (int i = 0; i < 8; i++)
    {
        SpawnChessPiece(EChessPlayers::White, EChessPieceTypes::Pawn, i, 1);
        SpawnChessPiece(EChessPlayers::Black, EChessPieceTypes::Pawn, i, 6);
    }

    //-------------------------------
    //Spawn Rooks
    //-------------------------------
    SpawnChessPiece(EChessPlayers::White, EChessPieceTypes::Rook, 0, 0);
    SpawnChessPiece(EChessPlayers::White, EChessPieceTypes::Rook, 7, 0);
    SpawnChessPiece(EChessPlayers::Black, EChessPieceTypes::Rook, 0, 7);
    SpawnChessPiece(EChessPlayers::Black, EChessPieceTypes::Rook, 7, 7);
    //-------------------------------

    //-------------------------------
    //Spawn Rooks
    //-------------------------------
    SpawnChessPiece(EChessPlayers::White, EChessPieceTypes::Knight, 1, 0);
    SpawnChessPiece(EChessPlayers::White, EChessPieceTypes::Knight, 6, 0);
    SpawnChessPiece(EChessPlayers::Black, EChessPieceTypes::Knight, 1, 7);
    SpawnChessPiece(EChessPlayers::Black, EChessPieceTypes::Knight, 6, 7);
    //-------------------------------

    //-------------------------------
    //Spawn Bishop
    //-------------------------------
    SpawnChessPiece(EChessPlayers::White, EChessPieceTypes::Bishop, 2, 0);
    SpawnChessPiece(EChessPlayers::White, EChessPieceTypes::Bishop, 5, 0);
    SpawnChessPiece(EChessPlayers::Black, EChessPieceTypes::Bishop, 2, 7);
    SpawnChessPiece(EChessPlayers::Black, EChessPieceTypes::Bishop, 5, 7);
    //-------------------------------

    //-------------------------------
    //Spawn Queen
    //-------------------------------
    SpawnChessPiece(EChessPlayers::White, EChessPieceTypes::Queen, 4, 0);
    SpawnChessPiece(EChessPlayers::Black, EChessPieceTypes::Queen, 4, 7);
    //-------------------------------

    //-------------------------------
    //Spawn King
    //-------------------------------
    SpawnChessPiece(EChessPlayers::White, EChessPieceTypes::King, 3, 0);
    SpawnChessPiece(EChessPlayers::Black, EChessPieceTypes::King, 3, 7);
    //-------------------------------

}
void AChessBoard::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    DetectHoveredTile();
}
#pragma endregion Protected Function

#pragma region Private Function
TArray<F2DPoint> AChessBoard::GetPosibleMoveIndexs(AChessPiece* piece)
{
    TArray<F2DPoint> positions;

    return positions;
}
void AChessBoard::DetectHoveredTile()
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController) return;

    FVector WorldLocation, WorldDirection;
    if (!PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection)) return;

    FHitResult HitResult;
    FVector EndTrace = WorldLocation + (WorldDirection * 10000.0f);
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);
    if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, EndTrace, ECC_Visibility, TraceParams))
    {
        FVector ClickedPosition = HitResult.ImpactPoint;
        FVector LocalPos = this->_chessBoardMeshComponent->GetComponentTransform().InverseTransformPosition(ClickedPosition);

        FVector MeshBounds = _chessBoardMeshComponent->GetStaticMesh()->GetBounds().BoxExtent * 2.0f;
        float BoardSize = FMath::Max(MeshBounds.X, MeshBounds.Y);
        float TileSize = BoardSize / 8;

        int32 TileX = FMath::Clamp(FMath::FloorToInt((LocalPos.X + BoardSize / 2) / TileSize), 0, 7);
        int32 TileY = FMath::Clamp(FMath::FloorToInt((LocalPos.Y + BoardSize / 2) / TileSize), 0, 7);

        // Only update if the hovered tile changes
        if (TileX != _hoveredTileX || TileY != _hoveredTileY)
        {
            _hoveredTileX = TileX;
            _hoveredTileY = TileY;

            //UE_LOG(LogTemp, Warning, TEXT("Hovered Tile: X=%d, Y=%d"), _hoveredTileX, _hoveredTileY);
            //GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Hovered Tile: X=%d, Y=%d"), _hoveredTileX, _hoveredTileY));
            
            //TODO: Apply hover effect
            //UMaterialInstanceDynamic* DynamicMaterial = _chessBoardMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
            //if (DynamicMaterial)
            //{
            //    DynamicMaterial->SetScalarParameterValue(TEXT("_hoveredTileX"), TileX);
            //    DynamicMaterial->SetScalarParameterValue(TEXT("_hoveredTileY"), TileY);
            //}
        }
    }
}
#pragma endregion Private Function
AChessPiece* AChessBoard::SpawnChessPiece(EChessPlayers owner, EChessPieceTypes type, int32 x, int32 y)
{
    if (UWorld* world = GetWorld())
    {
        FActorSpawnParameters spawnParams;
        spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        AChessPiece* newPiece = world->SpawnActor<AChessPiece>(AChessPiece::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, spawnParams);
        newPiece->SetChessBoard(this);
        newPiece->SetPieceOwner(owner);
        newPiece->SetPieceType(type);
        newPiece->SetTileIndex(x, y);
        this->_chessPieces.Add(newPiece);

        return newPiece;
    }
    return nullptr;
}
#pragma region Events Function
void AChessBoard::OnBeginMouseHover(UPrimitiveComponent* touchedComponent)
{
    AActor::SetActorTickEnabled(true);
    //UE_LOG(LogTemp, Warning, TEXT("Yes:%d"), this->_isHovering);
}
void AChessBoard::OnEndMouseHover(UPrimitiveComponent* touchedComponent)
{
    AActor::SetActorTickEnabled(false);
    this->_hoveredTileX = -1;
    this->_hoveredTileY = -1;
    //UE_LOG(LogTemp, Warning, TEXT("No:%d"), this->_isHovering);
}
void AChessBoard::OnMouseClick(UPrimitiveComponent* touchedComponent, FKey buttonPressed)
{
    if (IsValidTileIndex(this->_hoveredTileX, this->_hoveredTileY)) SetClickTile(this->_hoveredTileX, this->_hoveredTileY);
}
#pragma endregion Events Function
