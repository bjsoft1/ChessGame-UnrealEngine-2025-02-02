#pragma once

#include "NexusPawn.h"
#include "LudoPawn.generated.h"

UCLASS()
class NEXUSCHESS_API ALudoPawn : public ANexusPawn
{
	GENERATED_BODY()

public:
#pragma region System Function
	ALudoPawn(const FObjectInitializer& objectInitializer);
	virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent) override;
#pragma endregion System Function

#pragma region Public Variables
#pragma endregion Public Variables

protected:
#pragma region Protected Variables
#pragma endregion Protected Variables

private:
#pragma region Private Variables
#pragma endregion Private Variables

public:
#pragma region Public Function
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
