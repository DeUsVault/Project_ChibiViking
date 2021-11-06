// Copyright 2020-2021 Maksym Maisak. All Rights Reserved.

#include "Services/HTNService_BlueprintBase.h"
#include "BlueprintNodeHelpers.h"
#include "AIController.h"
#include "WorldStateProxy.h"
#include "HTNBlueprintLibrary.h"

UHTNService_BlueprintBase::UHTNService_BlueprintBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	bShowPropertyDetails(true)
{
#define IS_IMPLEMENTED(FunctionName) \
	(BlueprintNodeHelpers::HasBlueprintFunction(GET_FUNCTION_NAME_CHECKED(UHTNService_BlueprintBase, FunctionName), *this, *StaticClass()))
	bImplementsOnExecutionStart = IS_IMPLEMENTED(ReceiveExecutionStart);
	bImplementsTick = IS_IMPLEMENTED(ReceiveTick);
	bImplementsOnExecutionFinish = IS_IMPLEMENTED(ReceiveExecutionFinish);
	bImplementsOnPlanExecutionStarted = IS_IMPLEMENTED(ReceiveOnPlanExecutionStarted);
	bImplementsOnPlanExecutionFinished = IS_IMPLEMENTED(ReceiveOnPlanExecutionFinished);
#undef IS_IMPLEMENTED

	bNotifyExecutionStart = bImplementsOnExecutionStart;
	bNotifyTick = bImplementsTick;
	bNotifyExecutionFinish = bImplementsOnExecutionFinish;
	bNotifyOnPlanExecutionStarted = bImplementsOnPlanExecutionStarted;
	bNotifyOnPlanExecutionFinished = bImplementsOnPlanExecutionFinished;

	// All blueprint-based nodes must create instances
	bCreateNodeInstance = true;
	// Since tasks created from blueprints won't set this to true on their own.
	bOwnsGameplayTasks = true;
	
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		BlueprintNodeHelpers::CollectPropertyData(this, StaticClass(), PropertyData);
	}
}

void UHTNService_BlueprintBase::InitializeFromAsset(UHTN& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (Asset.BlackboardAsset)
	{
		BlueprintNodeHelpers::ResolveBlackboardSelectors(*this, *StaticClass(), *Asset.BlackboardAsset);
	}
}

FString UHTNService_BlueprintBase::GetStaticDescription() const
{
	FString Description = Super::GetStaticDescription();

	if (bShowPropertyDetails)
	{
		if (UHTNService_BlueprintBase* const CDO = GetClass()->GetDefaultObject<UHTNService_BlueprintBase>())
		{
			const FString PropertyDescription = BlueprintNodeHelpers::CollectPropertyDescription(this, StaticClass(), CDO->PropertyData);
			if (PropertyDescription.Len())
			{
				Description += TEXT(":\n\n");
				Description += PropertyDescription;
			}
		}
	}

	return Description;
}

void UHTNService_BlueprintBase::OnExecutionStart(UHTNComponent& OwnerComp, uint8* NodeMemory)
{
	if (!bImplementsOnExecutionStart)
	{
		return;
	}

	check(GetOwnerComponent() == &OwnerComp);
	check(UHTNNodeLibrary::GetOwnersWorldState(this) == OwnerComp.GetBlackboardProxy());
	check(UHTNNodeLibrary::GetOwnersWorldState(this)->IsBlackboard());
	check(UHTNNodeLibrary::GetOwnersWorldState(this)->IsEditable());
	ReceiveExecutionStart(OwnerComp.GetOwner(), OwnerComp.GetAIOwner(), OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr);
}

void UHTNService_BlueprintBase::TickNode(UHTNComponent& OwnerComp, uint8* NodeMemory, float DeltaTime)
{
	if (!bImplementsTick)
	{
		return;
	}

	check(GetOwnerComponent() == &OwnerComp);
	check(UHTNNodeLibrary::GetOwnersWorldState(this) == OwnerComp.GetBlackboardProxy());
	check(UHTNNodeLibrary::GetOwnersWorldState(this)->IsBlackboard());
	check(UHTNNodeLibrary::GetOwnersWorldState(this)->IsEditable());
	ReceiveTick(OwnerComp.GetOwner(), OwnerComp.GetAIOwner(), OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr, DeltaTime);
}

void UHTNService_BlueprintBase::OnExecutionFinish(UHTNComponent& OwnerComp, uint8* NodeMemory, EHTNNodeResult Result)
{
	BlueprintNodeHelpers::AbortLatentActions(OwnerComp, *this);
	
	if (!bImplementsOnExecutionFinish)
	{
		return;
	}
	
	check(GetOwnerComponent() == &OwnerComp);
	check(UHTNNodeLibrary::GetOwnersWorldState(this) == OwnerComp.GetBlackboardProxy());
	check(UHTNNodeLibrary::GetOwnersWorldState(this)->IsBlackboard());
	check(UHTNNodeLibrary::GetOwnersWorldState(this)->IsEditable());
	ReceiveExecutionFinish(OwnerComp.GetOwner(), OwnerComp.GetAIOwner(), OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr, Result);
}

void UHTNService_BlueprintBase::OnPlanExecutionStarted(UHTNComponent& OwnerComp, uint8* NodeMemory)
{
	if (!bImplementsOnPlanExecutionStarted)
	{
		return;
	}

	FGuardValue_Bitfield(bForceUsingPlanningWorldState, true);
	check(GetOwnerComponent() == &OwnerComp);
	check(UHTNNodeLibrary::GetOwnersWorldState(this) == OwnerComp.GetPlanningWorldStateProxy());
	check(!UHTNNodeLibrary::GetOwnersWorldState(this)->IsBlackboard());
	check(!UHTNNodeLibrary::GetOwnersWorldState(this)->IsEditable());
	ReceiveOnPlanExecutionStarted(OwnerComp.GetOwner(), OwnerComp.GetAIOwner(), OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr);
}

void UHTNService_BlueprintBase::OnPlanExecutionFinished(UHTNComponent& OwnerComp, uint8* NodeMemory, EHTNPlanExecutionFinishedResult Result)
{
	if (!bImplementsOnPlanExecutionFinished)
	{
		return;
	}

	FGuardValue_Bitfield(bForceUsingPlanningWorldState, true);
	check(GetOwnerComponent() == &OwnerComp);
	check(UHTNNodeLibrary::GetOwnersWorldState(this) == OwnerComp.GetPlanningWorldStateProxy());
	check(!UHTNNodeLibrary::GetOwnersWorldState(this)->IsBlackboard());
	check(!UHTNNodeLibrary::GetOwnersWorldState(this)->IsEditable());
	ReceiveOnPlanExecutionFinished(
		OwnerComp.GetOwner(), OwnerComp.GetAIOwner(), OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr,
		Result
	);
}