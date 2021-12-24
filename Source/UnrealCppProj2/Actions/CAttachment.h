#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CAttachment.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttachmentBeginOverlap,
												class ACharacter*, InAttacker,
												class AActor*, InAttackCauser,
												class ACharacter*, InOtherCharacter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttachmentEndOverlap,
												class ACharacter*, InAttacker,
												class AActor*, InAttackCauser,
												class ACharacter*, InOtherCharacter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttachmentCollision);

UCLASS()
class UNREALCPPPROJ2_API ACAttachment : public AActor
{
	GENERATED_BODY()
	
public:	
	ACAttachment();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnEquip();

	UFUNCTION(BlueprintImplementableEvent)
		void OnUnequip();

protected:
	UPROPERTY(BlueprintReadOnly)
		class ACharacter* OwnerCharacter;

	UPROPERTY(BlueprintReadOnly)
		class UCStateComponent* State;

	UPROPERTY(BlueprintReadOnly)
		class UCStatusComponent* Status;

protected:
	UFUNCTION(BlueprintCallable)
		void AttachTo(FName InSocketName);

	UFUNCTION(BlueprintCallable)
		void AttachToCollision(class UShapeComponent* InComponent, FName InSocketName);

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
		class USceneComponent* Scene;

public:
	UPROPERTY(BlueprintAssignable)
		FAttachmentBeginOverlap OnAttachmentBeginOverlap;

	UPROPERTY(BlueprintAssignable)
		FAttachmentEndOverlap OnAttachmentEndOverlap;

	UPROPERTY(BlueprintAssignable)
		FAttachmentCollision OnAttachmentCollision;

	UPROPERTY(BlueprintAssignable)
		FAttachmentCollision OffAttachmentCollision;

private:
	TArray<class UShapeComponent*> ShapeComponents;

	UFUNCTION()
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
										AActor* OtherActor, 
										UPrimitiveComponent* OtherComp,
										int32 OtherBodyIndex,
										bool bFromSweep,
										const FHitResult & SweepResult);
	UFUNCTION()
		void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, 
									AActor* OtherActor, 
									UPrimitiveComponent* OtherComp,
									int32 OtherBodyIndex);

public:
	//충돌체 활성/비활성
	void OnCollision();
	void OffCollision();
};
