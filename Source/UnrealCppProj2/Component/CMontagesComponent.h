#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Component/CStateComponent.h"		// << : 
#include "Engine/DataTable.h"			
#include "CMontagesComponent.generated.h"

USTRUCT(BlueprintType)
struct FMontageData : public FTableRowBase 
{
	GENERATED_BODY()

public :
	UPROPERTY(EditAnywhere)
		EStateType Type; 
	UPROPERTY(EditAnywhere)
		class UAnimMontage* AnimMontage; 
	UPROPERTY(EditAnywhere)
		float PlayRatio = 1.0f; 
	UPROPERTY(EditAnywhere)
		FName StartSection; 
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALCPPPROJ2_API UCMontagesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCMontagesComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "DataTable")
		UDataTable* DataTable; 

private:
	FMontageData* Datas[(int32)EStateType::Max]; 

private :
	void PlayAnimMontage(EStateType InState); 

public :
	void PlayRoll(); 
	void PlayBackstep(); 
	void PlayHitted(); 
	void PlayDead(); 
};
