#pragma once

#include "CoreMinimal.h"
#include "Actions/CDoAction.h"
#include "CDoACtion_Melee.generated.h"

UCLASS()
class UNREALCPPPROJ2_API ACDoAction_Melee : public ACDoAction
{
	GENERATED_BODY()

public:
	virtual void DoAction() override;
	virtual void Begin_DoAction() override;
	virtual void End_DoAction() override;
	
private:
	bool bExist;
	bool bEnable;
	bool bLast;
	int32 Index;

public:
	FORCEINLINE void EnableCombo() { bEnable = true;}
	FORCEINLINE void DisableCombo() { bEnable = false; }
};
