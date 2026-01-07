// Copyright Schuyler Zheng. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseCharacterAttributeSet.generated.h"

/* Base Variable Helper Functions */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)\

/**
 * 
 */
UCLASS()
class MINER_API UBaseCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	UBaseCharacterAttributeSet();
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, MaxStamina);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseAttack, Category = "Attributes")
	FGameplayAttributeData BaseAttack;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, BaseAttack);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth)	{	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseCharacterAttributeSet, Health, OldHealth);	}
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseCharacterAttributeSet, MaxHealth, OldMaxHealth); }
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina) { GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseCharacterAttributeSet, Stamina, OldStamina); }
	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) { GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseCharacterAttributeSet, MaxStamina, OldMaxStamina); }
	UFUNCTION()
	void OnRep_BaseAttack(const FGameplayAttributeData& OldBaseAttack) { GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseCharacterAttributeSet, BaseAttack, OldBaseAttack); }

protected:
	/**
	* Override values
	*/

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;	// Called before an attribute is changed
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;	// Called after a gameplay effect is executed
};
