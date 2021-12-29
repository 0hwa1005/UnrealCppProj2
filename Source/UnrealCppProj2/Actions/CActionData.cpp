#include "CActionData.h"
#include "Global.h"
#include "CAttachment.h"
#include "CEquipment.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "CDoAction.h"

void UCActionData::BeginPlay(ACharacter * InOwnerCharacter)
{
	FTransform transform;

	if (!!AttachmentClass)
	{
		Attachment = InOwnerCharacter->GetWorld()->SpawnActorDeferred<ACAttachment>(AttachmentClass, transform, InOwnerCharacter);
		//Attachment->SetActorLabel(InOwnerCharacter->GetActorLabel() + "_Attachment");//GetName도가능 숫자 붙어서 나옴
		Attachment->SetActorLabel(GetLabelName(InOwnerCharacter, "Attachment"));
		UGameplayStatics::FinishSpawningActor(Attachment, transform);
	}
	
	if (!!EquipmentClass)
	{
		Equipment = InOwnerCharacter->GetWorld()->SpawnActorDeferred<ACEquipment>
			(EquipmentClass, transform, InOwnerCharacter);
		Equipment->AttachToComponent(InOwnerCharacter->GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
		//Equipment->SetActorLabel(InOwnerCharacter->GetActorLabel() + "_Equipment");
		Equipment->SetActorLabel(GetLabelName(InOwnerCharacter, "Equipment"));
		Equipment->SetData(EquipmentData);
		Equipment->SetColor(EquipmentColor);
		UGameplayStatics::FinishSpawningActor(Equipment, transform);

		if (!!AttachmentClass)
		{
			Equipment->OnEquipmentDelegate.AddDynamic(Attachment, &ACAttachment::OnEquip);
			Equipment->OnUnequipmentDelegate.AddDynamic(Attachment, &ACAttachment::OnUnequip);
		}
	}

	if (!!DoActionClass)
	{
		// : do action
		DoAction = InOwnerCharacter->GetWorld()->SpawnActorDeferred<ACDoAction>
			(DoActionClass, transform, InOwnerCharacter);
		DoAction->AttachToComponent(InOwnerCharacter->GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
		//DoAction->SetActorLabel(InOwnerCharacter->GetActorLabel() + "_DoAction");
		DoAction->SetActorLabel(GetLabelName(InOwnerCharacter, "DoAction"));
		DoAction->SetDatas(DoActionDatas);
		UGameplayStatics::FinishSpawningActor(DoAction, transform);

		if (!!Equipment)
		{
			DoAction->SetEquipped(Equipment->GetEquipped());
		}

		if (!!Attachment)
		{
			Attachment->OnAttachmentBeginOverlap.AddDynamic(DoAction, &ACDoAction::OnAttachmentBeginOverlap);
			Attachment->OnAttachmentEndOverlap.AddDynamic(DoAction, &ACDoAction::OnAttachmentEndOverlap);

			Attachment->OnAttachmentCollision.AddDynamic(DoAction, &ACDoAction::OnAttachmentCollision);
			Attachment->OffAttachmentCollision.AddDynamic(DoAction, &ACDoAction::OffAttachmentCollision);
		}
	}
	
}

FString UCActionData::GetLabelName(ACharacter * InOwnerCharacter, FString InName)
{
	FString str;
	str.Append(InOwnerCharacter->GetActorLabel());
	str.Append("_");
	str.Append(InName);
	str.Append("_");
	str.Append(GetName().Replace(L"DA_", L""));

	return str;
}
