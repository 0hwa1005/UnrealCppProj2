#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CTargetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALCPPPROJ2_API UCTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCTargetComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	class ACharacter* OwnerCharacter;
	class ACharacter* Target;

private:
	void StartTargeting();
	void EndTargeting();
	void SetTraceTargets();
	void SetTarget();

public:
	void ToggleTarget();

private:
	UPROPERTY(EditAnywhere)
		float TraceRadius = 1000.0f;
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EDrawDebugTrace::Type>Debug;

private:
	TArray<class ACharacter*> TraceTargets;

		
};
