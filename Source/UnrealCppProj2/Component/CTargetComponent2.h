// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"	// << 
#include "CTargetComponent2.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALCPPPROJ2_API UCTargetComponent2 : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCTargetComponent2();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	class ACharacter* OwnerCharacter;
	class ACharacter* Target;
	class UParticleSystemComponent* Attached;

private:
	void StartTargeting();
	void EndTargeting();
	void SetTraceTargets(); //
	void SetTarget();
	void ChangeCursor(class ACharacter* InTarget);
	void ChangeTarget(bool InRight);

public:
	void ToggleTarget();
	void ChangeTargetLeft();
	void ChangeTargetRight();

private:
	UPROPERTY(EditAnywhere)
		float	TraceRadius = 1000.0f;

	UPROPERTY(EditAnywhere)
		TEnumAsByte<EDrawDebugTrace::Type> Debug;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* Particle;

	UPROPERTY(EditAnywhere)
		float	InteropSpeed = 2.5f;

private:
	TArray<class ACharacter*> TraceTargets;
};
