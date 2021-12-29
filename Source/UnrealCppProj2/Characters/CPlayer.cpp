#include "CPlayer.h"
#include "Global.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Component/COptionComponent.h"
#include "Component/CStatusComponent.h"
#include "Component/CMontagesComponent.h"
#include "Component/CActionComponent.h"
#include "Component/CTargetComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"

ACPlayer::ACPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent<USpringArmComponent>(this, &SpringArm, "SpringArm ", GetMesh());
	CHelpers::CreateComponent<UCameraComponent>(this, &Camera, "Camera", SpringArm);
	CHelpers::CreateActorComponent<UCOptionComponent>(this, &Option, "Option");
	CHelpers::CreateActorComponent<UCStatusComponent>(this, &Status, "Status");
	CHelpers::CreateActorComponent<UCStateComponent>(this, &State, "State");
	CHelpers::CreateActorComponent<UCMontagesComponent>(this, &Montages, "Montages");
	CHelpers::CreateActorComponent<UCActionComponent>(this, &Action, "Action");
	CHelpers::CreateActorComponent<UCTargetComponent>(this, &Target, "Target");

	bUseControllerRotationYaw = false;

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotationExact(FRotator(0, -90, 0));

	USkeletalMesh* mesh;
	CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/Character/Mesh/SK_Mannequin.SK_Mannequin'");
	GetMesh()->SetSkeletalMesh(mesh);

	TSubclassOf<UAnimInstance> animInstance;
	CHelpers::GetClass<UAnimInstance>(&animInstance, "AnimBlueprint'/Game/Player/ABP_CPlayer.ABP_CPlayer_C'");
	GetMesh()->SetAnimInstanceClass(animInstance);

	SpringArm->TargetArmLength = 400.0;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;

	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ACPlayer::BeginPlay()
{
	Super::BeginPlay();

	State->OnStateTypeChanged.AddDynamic(this, &ACPlayer::OnStateTypeChanged);

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

	Action->SetUnarmedMode();
}

void ACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACPlayer::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPlayer::OnMoveRight);
	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACPlayer::OnHorizontalLook);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACPlayer::OnVerticalLook);

	PlayerInputComponent->BindAction("Avoid", EInputEvent::IE_Pressed, this, &ACPlayer::OnAvoid);

	PlayerInputComponent->BindAction("Fist", EInputEvent::IE_Pressed, this, &ACPlayer::OnFist);
	PlayerInputComponent->BindAction("OneHand", EInputEvent::IE_Pressed, this, &ACPlayer::OnOneHand);
	PlayerInputComponent->BindAction("TwoHand", EInputEvent::IE_Pressed, this, &ACPlayer::OnTwoHand);
	PlayerInputComponent->BindAction("Warp", EInputEvent::IE_Pressed, this, &ACPlayer::OnWarp);
	PlayerInputComponent->BindAction("Target", EInputEvent::IE_Pressed, this, &ACPlayer::OnTarget);

	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Pressed, this, &ACPlayer::OnDoAction);


}

void ACPlayer::OnStateTypeChanged(EStateType InPrevType, EStateType InNewType)
{
	switch (InNewType)
	{
	case EStateType::Roll:		Begin_Roll();		break;
	case EStateType::Backstep:	Begin_Backstep();	 break;
	}
}

void ACPlayer::OnMoveForward(float InAxis)
{
	CheckFalse(Status->CanMove());

	FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
	FVector direction = FQuat(rotator).GetForwardVector();
	AddMovementInput(direction, InAxis);
}

void ACPlayer::OnMoveRight(float InAxis)
{
	CheckFalse(Status->CanMove());

	FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
	FVector direction = FQuat(rotator).GetRightVector();
	AddMovementInput(direction, InAxis);
}

void ACPlayer::OnHorizontalLook(float InAxis)
{
	float rate = Option->GetHorizontalLookRate();
	AddControllerYawInput(InAxis * rate * GetWorld()->GetDeltaSeconds());
}

void ACPlayer::OnVerticalLook(float InAxis)
{
	float rate = Option->GetVerticalLookRate();
	AddControllerPitchInput(InAxis * rate * GetWorld()->GetDeltaSeconds());
}

void ACPlayer::OnAvoid()
{
	CheckFalse(Status->CanMove());
	CheckFalse(State->IsIdleMode());

	if (InputComponent->GetAxisValue("MoveForward") < 0.0f)
	{
		State->SetBackstepMode();
		return;
	}

	State->SetRollMode();
}

void ACPlayer::Begin_Roll()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	FVector start = GetActorLocation();
	FVector dest = start + GetVelocity().GetSafeNormal2D();
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(start, dest));

	Montages->PlayRoll();
}

void ACPlayer::Begin_Backstep()
{
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	

	Montages->PlayBackStep();
}

void ACPlayer::End_Roll()
{
	if (Action->IsUnarmedMode() == false)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	State->SetIdleMode();
}

void ACPlayer::End_Backstep()
{
	if (Action->IsUnarmedMode())
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	State->SetIdleMode();
}

void ACPlayer::OnFist()
{
	CheckFalse(State->IsIdleMode());
	Action->SetFistMode();
}

void ACPlayer::OnOneHand()
{
	CheckFalse(State->IsIdleMode());
	Action->SetOneHandMode();
}

void ACPlayer::OnTwoHand()
{
	CheckFalse(State->IsIdleMode());
	Action->SetTwoHandMode();
}

void ACPlayer::OnWarp()
{
	CheckFalse(State->IsIdleMode());
	Action->SetWarpMode();
}

void ACPlayer::OnTarget()
{
	Target->ToggleTarget();
}

void ACPlayer::OnDoAction()
{
	Action->DoAction();
}

void ACPlayer::ChangeColor(FLinearColor InColor)
{
	BodyMaterial->SetVectorParameterValue("BodyColor", InColor);
	LogoMaterial->SetVectorParameterValue("BodyColor", InColor);//Logo에도 Body로 되어있어서 BodyColor씀
}

