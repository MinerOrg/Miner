// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseCharacterAttributeSet.generated.h"

/**
* Define how to get , set, and initialize attributes. (nvm didn't need it)
*/

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
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, Health);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, MaxHealth);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, Stamina);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, MaxStamina);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData BaseAttack;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, BaseAttack);

protected:
	/**
	* Override values
	*/

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;	// Called before an attribute is changed
	
	/**
	* Helper functions
	*/

	void ClampValues(const FGameplayAttribute& Attribute, float& NewValue);
};
