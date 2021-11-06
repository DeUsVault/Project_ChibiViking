// Copyright 2020-2021 Maksym Maisak. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "VisualLogger/VisualLogger.h"

#include "BlackboardWorldstate.h"
#include "HTNTypes.h"

#include "WorldStateProxy.generated.h"

// A blueprint-exposed proxy for interacting with FBlackboardWorldState.
// If no worldstate is set, falls back onto a provided blackboard component.
UCLASS(DefaultToInstanced)
class HTN_API UWorldStateProxy : public UObject
{
	GENERATED_BODY()
	
public:
	UWorldStateProxy(const FObjectInitializer& Initializer);

	bool IsEditable() const;

	bool IsBlackboard() const;
	bool IsWorldState() const;

	UBlackboardComponent* GetBlackboard() const;
	TSharedPtr<FBlackboardWorldState> GetWorldState() const;

	template<class TDataClass>
	typename TDataClass::FDataType GetValue(const FName& KeyName) const;

	template<class TDataClass>
	typename TDataClass::FDataType GetValue(FBlackboard::FKey KeyID) const;

	template<class TDataClass>
	bool SetValue(const FName& KeyName, typename TDataClass::FDataType Value);

	template<class TDataClass>
	bool SetValue(FBlackboard::FKey KeyID, typename TDataClass::FDataType Value);

	bool CopyValueFrom(const FBlackboardWorldState& SourceWorldState, FBlackboard::FKey KeyID);
	
	bool TestBasicOperation(const UBlackboardKeyType& Key, FBlackboard::FKey KeyID, EBasicKeyOperation::Type Type) const;
	bool TestArithmeticOperation(const UBlackboardKeyType& Key, FBlackboard::FKey KeyID, EArithmeticKeyOperation::Type Type, int32 IntValue, float FloatValue) const;
	bool TestTextOperation(const UBlackboardKeyType& Key, FBlackboard::FKey KeyID, ETextKeyOperation::Type Type, const FString& StringValue) const;
	
	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	bool GetLocation(const FBlackboardKeySelector& KeySelector, FVector& OutLocation, AActor*& OutActor) const;

	UFUNCTION(BlueprintPure, Category = "AI|HTN")
	FVector GetSelfLocation() const;

	// A helper function that gets either a vector or the location of an actor, depending on the type of the KeySelector
	FVector GetLocation(const FBlackboardKeySelector& KeySelector, class AActor** OutActor = nullptr) const;
	
	UFUNCTION(BlueprintPure, Category="AI|HTN")
	UObject* GetValueAsObject(const FName& KeyName) const;

	UFUNCTION(BlueprintPure, Category="AI|HTN")
	AActor* GetValueAsActor(const FName& KeyName) const;

	UFUNCTION(BlueprintPure, Category="AI|HTN")
	UClass* GetValueAsClass(const FName& KeyName) const;

	UFUNCTION(BlueprintPure, Category="AI|HTN")
	uint8 GetValueAsEnum(const FName& KeyName) const;

	UFUNCTION(BlueprintPure, Category="AI|HTN")
	int32 GetValueAsInt(const FName& KeyName) const;

	UFUNCTION(BlueprintPure, Category="AI|HTN")
	float GetValueAsFloat(const FName& KeyName) const;

	UFUNCTION(BlueprintPure, Category="AI|HTN")
	bool GetValueAsBool(const FName& KeyName) const;

	UFUNCTION(BlueprintPure, Category="AI|HTN")
	FString GetValueAsString(const FName& KeyName) const;

	UFUNCTION(BlueprintPure, Category="AI|HTN")
	FName GetValueAsName(const FName& KeyName) const;

	UFUNCTION(BlueprintPure, Category="AI|HTN")
	FVector GetValueAsVector(const FName& KeyName) const;

	UFUNCTION(BlueprintPure, Category ="AI|HTN")
	FRotator GetValueAsRotator(const FName& KeyName) const;

	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void SetValueAsObject(const FName& KeyName, UObject* Value);

	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void SetValueAsClass(const FName& KeyName, UClass* Value);

	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void SetValueAsEnum(const FName& KeyName, uint8 Value);

	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void SetValueAsInt(const FName& KeyName, int32 Value);

	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void SetValueAsFloat(const FName& KeyName, float Value);

	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void SetValueAsBool(const FName& KeyName, bool Value);

	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void SetValueAsString(const FName& KeyName, FString Value);

	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void SetValueAsName(const FName& KeyName, FName Value);

	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void SetValueAsVector(const FName& KeyName, FVector Value);

	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void SetValueAsRotator(const FName& KeyName, FRotator Value);

	UFUNCTION(BlueprintCallable, Category = "AI|HTN", Meta = (Tooltip = "If the vector value has been set (and not cleared), this function returns true (indicating that the value should be valid).  If it's not set, the vector value is invalid and this function will return false.  (Also returns false if the key specified does not hold a vector.)"))
	bool IsVectorValueSet(const FName& KeyName) const;

	// return false if call failed (most probably no such entry in BB)
	UFUNCTION(BlueprintCallable, Category = "AI|HTN")
	bool GetLocationFromEntry(const FName& KeyName, FVector& ResultLocation) const;

	// return false if call failed (most probably no such entry in BB)
	UFUNCTION(BlueprintCallable, Category = "AI|HTN")
	bool GetRotationFromEntry(const FName& KeyName, FRotator& ResultRotation) const;

	// Resets indicated value to "not set" value, based on values type
	UFUNCTION(BlueprintCallable, Category="AI|HTN")
	void ClearValue(const FName& KeyName);

	FString DescribeKeyValue(const FName& KeyName, EBlackboardDescription::Type Type) const;
	FString DescribeKeyValue(FBlackboard::FKey KeyID, EBlackboardDescription::Type Mode) const;

private:
	FName GetKeyName(FBlackboard::FKey KeyID) const;
	FBlackboard::FKey GetKeyID(const FName& KeyName) const;
	
	friend class UHTNComponent;
	friend struct FGuardWorldStateProxy;
	
	UPROPERTY()
	UBrainComponent* Owner;

	TSharedPtr<FBlackboardWorldState> WorldState;

	UPROPERTY()
	bool bIsEditable;
};

struct FGuardWorldStateProxy : FNoncopyable
{
	FGuardWorldStateProxy(UWorldStateProxy& Proxy);
	FGuardWorldStateProxy(UWorldStateProxy& Proxy, TSharedPtr<class FBlackboardWorldState> WorldState, bool bIsEditable = true);
	~FGuardWorldStateProxy();
	
private:
	UWorldStateProxy& Proxy;
	TSharedPtr<FBlackboardWorldState> PrevWorldState;
	bool bPrevIsEditable;
};

FORCEINLINE bool UWorldStateProxy::IsEditable() const { return bIsEditable; }
FORCEINLINE bool UWorldStateProxy::IsBlackboard() const { return Owner->GetBlackboardComponent() && !WorldState.IsValid(); }
FORCEINLINE bool UWorldStateProxy::IsWorldState() const { return WorldState.IsValid(); }
FORCEINLINE UBlackboardComponent* UWorldStateProxy::GetBlackboard() const { check(Owner); return Owner->GetBlackboardComponent(); }
FORCEINLINE TSharedPtr<FBlackboardWorldState> UWorldStateProxy::GetWorldState() const { return WorldState; }

template <class TDataClass>
FORCEINLINE typename TDataClass::FDataType UWorldStateProxy::GetValue(const FName& KeyName) const
{
	return GetValue<TDataClass>(GetKeyID(KeyName));
}

template <class TDataClass>
typename TDataClass::FDataType UWorldStateProxy::GetValue(FBlackboard::FKey KeyID) const
{
	if (WorldState.IsValid())
	{
		return WorldState->GetValue<TDataClass>(KeyID);
	}

	if (ensure(Owner))
	{
		if (UBlackboardComponent* const BlackboardComponent = Owner->GetBlackboardComponent())
		{
			return BlackboardComponent->GetValue<TDataClass>(KeyID);
		}
	}

	return TDataClass::InvalidValue;
}

template <class TDataClass>
FORCEINLINE bool UWorldStateProxy::SetValue(const FName& KeyName, typename TDataClass::FDataType Value)
{
	return SetValue<TDataClass>(GetKeyID(KeyName), Value);
}

template <class TDataClass>
bool UWorldStateProxy::SetValue(FBlackboard::FKey KeyID, typename TDataClass::FDataType Value)
{
	if (!bIsEditable)
	{
		UE_VLOG_UELOG(Owner, LogHTN, Error, 
			TEXT("Trying to set value on a read-only Worldstate! Key: %s. Worldstates are read-only during plan recheck."), 
			*GetKeyName(KeyID).ToString()
		);
		return false;
	}
	
	if (WorldState.IsValid())
	{
		return WorldState->SetValue<TDataClass>(KeyID, Value);
	}

	if (ensureMsgf(Owner, TEXT("Trying to set value on a worldstate proxy that has no Owner component assigned.")))
	{
		if (UBlackboardComponent* const BlackboardComponent = Owner->GetBlackboardComponent())
		{
			return BlackboardComponent->SetValue<TDataClass>(KeyID, Value);
		}
	}

	return false;
}

FORCEINLINE FVector UWorldStateProxy::GetSelfLocation() const
{
	return GetValue<UBlackboardKeyType_Vector>(FBlackboard::KeySelfLocation);
}