#include "CActionComponent.h"
#include "Global.h"
#include "Actions/CActionData.h"
#include "Actions/CDoAction.h"
#include "GameFramework/Character.h"
#include "Actions/CEquipment.h"
#include "Actions/CAttachment.h"

UCActionComponent::UCActionComponent()
{
}

void UCActionComponent::BeginPlay()
{
	Super::BeginPlay();


	ACharacter* character = Cast<ACharacter>(GetOwner()); 
	for (int32 i = 0; i < (int32)EActionType::Max; i++)
	{
		if (!!Datas[i])
			Datas[i]->BeginPlay(character); 
	}

}

void UCActionComponent::ChangeType(EActionType InNewType)
{
	EActionType prevType = Type; 
	Type = InNewType; 
	if (OnActionTypeChanged.IsBound())
		OnActionTypeChanged.Broadcast(prevType, InNewType);

}

void UCActionComponent::SetMode(EActionType InType)
{
	if (Type == InType)
	{
		SetUnarmedMode(); 
		return; 
	}
	else if (IsUnarmedMode() == false)
	{
		// : 무기 해제
		ACEquipment* equipment = Datas[(int32)Type]->GetEquipment();
		CheckNull(equipment);

		equipment->Unequip();
	}

	// : 무기 장착 
	if (!!Datas[(int32)InType])
	{
		ACEquipment* equipment = Datas[(int32)InType]->GetEquipment();
		CheckNull(equipment);
		equipment->Equip();
	}

	ChangeType(InType); 
}

void UCActionComponent::SetUnarmedMode()
{
	if (!!Datas[(int32)Type])
	{
		ACEquipment* equipment = Datas[(int32)Type]->GetEquipment(); 
		CheckNull(equipment); 

		equipment->Unequip(); 
	}

	ChangeType(EActionType::Unarmed); 
}

void UCActionComponent::SetFistMode()
{
	SetMode(EActionType::Fist);
}

void UCActionComponent::SetOneHandMode()
{
	SetMode(EActionType::OneHand); 
}

void UCActionComponent::SetTwoHandMode()
{
	SetMode(EActionType::TwoHand);
}

void UCActionComponent::SetWarpMode()
{
	SetMode(EActionType::Warp);
}

void UCActionComponent::SetFireStormMode()
{
	SetMode(EActionType::FireStorm); 
}

void UCActionComponent::SetIceBallMode()
{
	SetMode(EActionType::IceBall); 
}

void UCActionComponent::DoAction()
{
	CheckTrue(IsUnarmedMode()); 

	if (!!Datas[(int32)Type])
	{
		ACDoAction* action = Datas[(int32)Type]->GetDoAction(); 

		if (!!action)
			action->DoAction(); 
	}
}

void UCActionComponent::OffAllCollision()
{
	for (UCActionData* data : Datas)
	{
		if (!!data == false)
			continue; 

		if (!!data->GetAttachment() == false)
			continue; 

		data->GetAttachment()->OffCollision(); 
	}
}

void UCActionComponent::DoAim(bool InAim)
{
	CheckTrue(IsUnarmedMode()); 
	if (!!Datas[(int32)Type])
	{
		ACDoAction* action = Datas[(int32)Type]->GetDoAction(); 
		if (!!action)
			InAim ? action->OnAim() : action->OffAim(); 
	}
}

