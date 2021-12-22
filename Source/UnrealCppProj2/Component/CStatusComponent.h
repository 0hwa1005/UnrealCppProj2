#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CStatusComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALCPPPROJ2_API UCStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCStatusComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Health")//
		float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Speed")
		float WalkSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Speed")
		float RunSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Speed")
		float SprintSpeed = 600.0f;

public:
	FORCEINLINE float GetWalkSpeed() { return WalkSpeed; }
	FORCEINLINE float GetRunSpeed() { return RunSpeed; }
	FORCEINLINE float GetSprintSpeed() { return SprintSpeed; }
	FORCEINLINE bool CanMove() { return bCanMove; }

	FORCEINLINE float GetMaxHealth() { return MaxHealth; }
	FORCEINLINE float GetHealth() { return Health; }

private:
	bool bCanMove = true;
	float Health;

public:
	void SetMove();
	void SetStop();

};
