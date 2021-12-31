#include "CEnemy.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/Animinstance.h"
#include "Component/CStatusComponent.h"	// << : 
#include "Component/CMontagesComponent.h"
#include "Component/CActionComponent.h"	
#include "Widgets/CUserWidget_Name.h"
#include "Widgets/CUserWidget_Health.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Actions/CActionData.h"

ACEnemy::ACEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent<UWidgetComponent>(this, &NameWidget,
		"NameWidget", GetMesh()); 
	CHelpers::CreateComponent<UWidgetComponent>(this, &HealthWidget,
		"HealthWidget", GetMesh());



	CHelpers::CreateActorComponent<UCStatusComponent>(this, &Status, "Status");
	CHelpers::CreateActorComponent<UCStateComponent>(this, &State, "State");
	CHelpers::CreateActorComponent<UCMontagesComponent>(this, &Montages, "Montages");
	CHelpers::CreateActorComponent<UCActionComponent>(this, &Action, "Action");


	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	USkeletalMesh* mesh;
	CHelpers::GetAsset<USkeletalMesh>(&mesh,
		"SkeletalMesh'/Game/Character/Mesh/SK_Mannequin.SK_Mannequin'");
	GetMesh()->SetSkeletalMesh(mesh);

	TSubclassOf<UAnimInstance> animInstance;
	CHelpers::GetClass<UAnimInstance>(&animInstance, "AnimBlueprint'/Game/Enemies/ABP_CEnemy.ABP_CEnemy_C'");
	GetMesh()->SetAnimInstanceClass(animInstance);

	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);

	TSubclassOf<UCUserWidget_Name> nameClass; 
	CHelpers::GetClass<UCUserWidget_Name>(&nameClass,
		"WidgetBlueprint'/Game/Widgets/WB_Name.WB_Name_C'"); 
	NameWidget->SetWidgetClass(nameClass); 
	NameWidget->SetRelativeLocation(FVector(0, 0, 240)); 
	NameWidget->SetDrawSize(FVector2D(240, 30)); 
	NameWidget->SetWidgetSpace(EWidgetSpace::Screen); 

	TSubclassOf<UCUserWidget_Health> healthClass;
	CHelpers::GetClass<UCUserWidget_Health>(&healthClass,
		"WidgetBlueprint'/Game/Widgets/WB_Health.WB_Health_C'");
	HealthWidget->SetWidgetClass(healthClass);
	HealthWidget->SetRelativeLocation(FVector(0, 0, 190));
	HealthWidget->SetDrawSize(FVector2D(120, 20));
	HealthWidget->SetWidgetSpace(EWidgetSpace::Screen);


}

void ACEnemy::BeginPlay()
{
	UMaterialInstanceConstant* body;
	UMaterialInstanceConstant* logo;

	CHelpers::GetAssetDynamic<UMaterialInstanceConstant>(&body,
		"MaterialInstanceConstant'/Game/Materials/M_UE4Man_Body_Inst.M_UE4Man_Body_Inst'");
	CHelpers::GetAssetDynamic<UMaterialInstanceConstant>(&logo,
		"MaterialInstanceConstant'/Game/Materials/M_UE4Man_ChestLogo_Inst.M_UE4Man_ChestLogo_Inst'");

	BodyMaterial = UMaterialInstanceDynamic::Create(body, this);
	LogoMaterial = UMaterialInstanceDynamic::Create(logo, this);

	GetMesh()->SetMaterial(0, BodyMaterial);
	GetMesh()->SetMaterial(1, LogoMaterial);

	Super::BeginPlay();

	State->OnStateTypeChanged.AddDynamic(this, &ACEnemy::OnStateTypeChanged); 

	NameWidget->InitWidget(); 
	Cast<UCUserWidget_Name>(NameWidget->GetUserWidgetObject())->SetNameText(GetActorLabel()); 

	HealthWidget->InitWidget(); 
	Cast<UCUserWidget_Health>(HealthWidget->GetUserWidgetObject())->Update(Status->GetHealth(),
		Status->GetMaxHealth()); 

	Action->SetUnarmedMode(); 
}

void ACEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACEnemy::ChangeColor(FLinearColor InColor)
{
	BodyMaterial->SetVectorParameterValue("BodyColor", InColor);
	LogoMaterial->SetVectorParameterValue("BodyColor", InColor);
}

float ACEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser); 
	//CLog::Log(Damage); 

	DamageInstigator = EventInstigator; 
	DamageValue = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	State->SetHittedMode(); 

	return Status->GetHealth(); 
}

void ACEnemy::OnStateTypeChanged(EStateType InPrevType, EStateType InNewType)
{
	switch (InNewType)
	{
	case EStateType::Hitted: Hitted(); break; 
	case EStateType::Dead: Dead(); break; 
	}
}

void ACEnemy::RestoreColor()
{
	FLinearColor color = Action->GetCurrent()->GetEquipmentColor(); 
	ChangeColor(color); 
}

void ACEnemy::Hitted()
{
	Status->SubHealth(DamageValue); 
	Cast<UCUserWidget_Health>(HealthWidget->GetUserWidgetObject())->Update(
		Status->GetHealth(),
		Status->GetMaxHealth()
	); 
	DamageValue = 0.0f; 

	Status->SetStop(); 

	if (Status->GetHealth() <= 0.0f)
	{
		State->SetDeadMode(); 
		return; 
	}

	Montages->PlayHitted(); 

	// >> : 
	FVector start = GetActorLocation(); 
	FVector target = DamageInstigator->GetPawn()->GetActorLocation(); 

	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(start, target)); 
	DamageInstigator = NULL; 

	FVector direction = target - start; 
	direction.Normalize(); 
	LaunchCharacter(-direction * LaunchAmount, true, false); 
	// << : 

	ChangeColor(FLinearColor(1, 0, 0, 1)); 
	UKismetSystemLibrary::K2_SetTimer(this, "RestoreColor", 0.1f, false); 
}

void ACEnemy::Dead()
{
	CheckFalse(State->IsDeadMode()); 

	Montages->PlayDead(); 
}

void ACEnemy::Begin_Dead()
{
	Action->OffAllCollision(); 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

}

void ACEnemy::End_Dead()
{
	Destroy(); 
}
