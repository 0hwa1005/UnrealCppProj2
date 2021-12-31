#pragma once

#include "CoreMinimal.h"
#include "Actions/CDoAction.h"
#include "CDoAction_Warp.generated.h"

UCLASS()
class UNREALCPPPROJ2_API ACDoAction_Warp : public ACDoAction
{
	GENERATED_BODY()

public:
	virtual void DoAction() override;
	virtual void Begin_DoAction()override;
	virtual void End_DoAction() override;
	virtual void Tick(float DeltaTime) override; 

protected :
	virtual void BeginPlay() override; 

private :
	class UDecalComponent* Decal; 
	FVector		Location; 

private :
	bool GetCursorLocationAndRotation(FVector& OutLocation, FRotator& OutRotator); 
};
