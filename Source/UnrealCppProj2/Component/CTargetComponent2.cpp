#include "CTargetComponent2.h"
#include "GameFramework/Character.h"
#include "Global.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Component/CStateComponent.h"


UCTargetComponent2::UCTargetComponent2()
{
	PrimaryComponentTick.bCanEverTick = true;

	CHelpers::GetAsset<UParticleSystem>(&Particle, "ParticleSystem'/Game/Effects/P_Enrage_Base.P_Enrage_Base'");
}

void UCTargetComponent2::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
}


void UCTargetComponent2::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckNull(Target);

	UCStateComponent* state = CHelpers::GetComponent<UCStateComponent>(Target);

	bool b = false;
	b |= state->IsDeadMode();
	b |= Target->GetDistanceTo(OwnerCharacter) >= TraceRadius;

	if (b == true)
	{
		EndTargeting();
		return;
	}


	FVector start = OwnerCharacter->GetActorLocation();
	FVector target = Target->GetActorLocation();

	FRotator rotator = UKismetMathLibrary::FindLookAtRotation(start, target);

	FRotator current = OwnerCharacter->GetControlRotation();
	rotator = UKismetMathLibrary::RInterpTo(current, rotator, DeltaTime, InteropSpeed);
	OwnerCharacter->GetController()->SetControlRotation(rotator);
}

void UCTargetComponent2::StartTargeting()
{
	SetTraceTargets();
	SetTarget();
}

void UCTargetComponent2::EndTargeting()
{
	Target = NULL;
	TraceTargets.Empty();
	if (!!Attached)
		Attached->DestroyComponent();
}

void UCTargetComponent2::SetTraceTargets()
{

	FVector start = OwnerCharacter->GetActorLocation();
	FVector end = FVector(start.X, start.Y, start.Z + 1);

	TArray<AActor*> ignoreActors;
	ignoreActors.Add(OwnerCharacter);

	TArray<FHitResult> hitResults;
	UKismetSystemLibrary::SphereTraceMultiByProfile(GetWorld(), start, end,
		TraceRadius, "Pawn", false, ignoreActors, Debug, hitResults, true,
		FLinearColor::Green, FLinearColor::Red, 1.0f);

	for (const FHitResult& result : hitResults)
	{
		if (result.GetActor()->GetClass() == OwnerCharacter->GetClass())
			continue;

		ACharacter* character = Cast<ACharacter>(result.GetActor());
		if (!!character)
			TraceTargets.AddUnique(character);
	}


	//for (const ACharacter* character : TraceTargets)
	//{
	//	CLog::Print(character->GetName()); 
	//	CLog::Log(character->GetName()); 
	//}
}

void UCTargetComponent2::SetTarget()
{
	float angle = -2.0f;
	ACharacter* target = NULL;

	for (ACharacter* character : TraceTargets)
	{
		FVector direction = FQuat(OwnerCharacter->GetControlRotation()).GetForwardVector();
		FVector offset = character->GetActorLocation() - OwnerCharacter->GetActorLocation();
		offset = offset.GetSafeNormal();

		float temp = direction | offset; // : dot product 
		if (temp < angle)
			continue;

		angle = temp;
		target = character;
	}

	//CLog::Print(target->GetName());
	//CLog::Log(target->GetActorLabel());
	ChangeCursor(target);
}

void UCTargetComponent2::ChangeCursor(ACharacter* InTarget)
{
	if (!!InTarget)
	{
		if (!!Attached)
			Attached->DestroyComponent();

		Attached = UGameplayStatics::SpawnEmitterAttached(Particle, InTarget->GetMesh(), "Spine_Target");
		Target = InTarget;
		return;
	}

	EndTargeting();
}

void UCTargetComponent2::ChangeTarget(bool InRight)
{
	CheckNull(Target);
	TMap<float, ACharacter*> map;
	for (ACharacter* character : TraceTargets)
	{
		if (Target == character)
			continue;

		FVector targetLocation = character->GetActorLocation();
		FVector ownerLocation = OwnerCharacter->GetActorLocation();
		FVector ownerToTarget = targetLocation - ownerLocation;

		FQuat quat = FQuat(OwnerCharacter->GetControlRotation());
		FVector forward = quat.GetForwardVector();
		FVector up = quat.GetUpVector();

		FVector cross = forward ^ ownerToTarget; // ^ cross product
		float dot = cross | up;

		map.Add(dot, character); //
	}

	float minimum = FLT_MAX;
	ACharacter* target = NULL;

	TArray<float> keys;
	map.GetKeys(keys);

	for (float key : keys)
	{
		if (InRight == true)
		{
			if (key < 0.0f)
				continue;
		}
		else
		{
			if (key > 0.0f)
				continue;
		}

		if (FMath::Abs(key) > minimum)
			continue;

		minimum = FMath::Abs(key);
		target = *map.Find(key);
	}

	ChangeCursor(target);

}

void UCTargetComponent2::ToggleTarget()
{
	if (!!Target)
	{
		EndTargeting();
		return;
	}

	StartTargeting();
}

void UCTargetComponent2::ChangeTargetLeft()
{
	ChangeTarget(false);
}

void UCTargetComponent2::ChangeTargetRight()
{
	ChangeTarget(true);
}

