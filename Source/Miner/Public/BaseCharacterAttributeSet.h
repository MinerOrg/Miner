// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseCharacterAttributeSet.generated.h"

/**
* Define how to get , set, and initialize attributes. (nvm didn't need it)
*/

/* Define Health variable functions */
//#define ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, Health)\
//	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBaseCharacterAttributeSet, Health)\
//	GAMEPLAYATTRIBUTE_VALUE_GETTER(Health)\
//	GAMEPLAYATTRIBUTE_VALUE_SETTER(Health)\
//	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health)\
//
///* Define Health variable functions */
//#define ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, Health)\
//	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBaseCharacterAttributeSet, Health)\
//	GAMEPLAYATTRIBUTE_VALUE_GETTER(Health)\
//	GAMEPLAYATTRIBUTE_VALUE_SETTER(Health)\
//	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health)\
//
///* Define Health variable functions */
//#define ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, Health)\
//	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBaseCharacterAttributeSet, Health)\
//	GAMEPLAYATTRIBUTE_VALUE_GETTER(Health)\
//	GAMEPLAYATTRIBUTE_VALUE_SETTER(Health)\
//	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health)\

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData BaseAttack;
public: 
	/**
	* Helper functions to access attributes
	*/

	/* Health Variable Functions */
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBaseCharacterAttributeSet, Health);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(Health);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(Health);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health);

	/* MaxHealth Variable Functions */
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBaseCharacterAttributeSet, MaxHealth);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxHealth);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxHealth);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxHealth);

	/* Stamina Variable Functions */
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBaseCharacterAttributeSet, Stamina);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(Stamina);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(Stamina);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Stamina);

	/* MaxStamina Variable Functions */
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBaseCharacterAttributeSet, MaxStamina);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxStamina);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxStamina);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxStamina);

	/* BaseAttack Variable Functions */
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBaseCharacterAttributeSet, BaseAttack);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(BaseAttack);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(BaseAttack);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(BaseAttack);
};
