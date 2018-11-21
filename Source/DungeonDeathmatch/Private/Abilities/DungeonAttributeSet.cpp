// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonAttributeSet.h"
#include "DungeonDeathmatch.h"
#include "DungeonAbilitySystemComponent.h"
#include "Characters/DungeonCharacter.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UDungeonAttributeSet::UDungeonAttributeSet()
	: Health(100.0f)
	, MaxHealth(100.0f)
	, HealthRegen(1.0f)
	, Mana(100.0f)
	, MaxMana(100.0f)
	, ManaRegen(5.0f)
	, Stamina(100.0f)
	, MaxStamina(100.0f)
	, StaminaRegen(10.0f)
	, AttackPower(10.0f)
	, DefensePower(10.0f)
	, MoveSpeed(100.0f)
	, Damage(5.0f)
{
}

void UDungeonAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDungeonAttributeSet, Health);
	DOREPLIFETIME(UDungeonAttributeSet, MaxHealth);
	DOREPLIFETIME(UDungeonAttributeSet, HealthRegen);
	DOREPLIFETIME(UDungeonAttributeSet, Mana);
	DOREPLIFETIME(UDungeonAttributeSet, MaxMana);
	DOREPLIFETIME(UDungeonAttributeSet, ManaRegen);
	DOREPLIFETIME(UDungeonAttributeSet, Stamina);
	DOREPLIFETIME(UDungeonAttributeSet, MaxStamina);
	DOREPLIFETIME(UDungeonAttributeSet, StaminaRegen);
	DOREPLIFETIME(UDungeonAttributeSet, AttackPower);
	DOREPLIFETIME(UDungeonAttributeSet, DefensePower);
	DOREPLIFETIME(UDungeonAttributeSet, MoveSpeed);
}

void UDungeonAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
}

void UDungeonAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	// Compute the delta between old and new, if it is available
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ADungeonCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ADungeonCharacter>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Get the Source actor
		AActor* SourceActor = nullptr;
		AController* SourceController = nullptr;
		ADungeonCharacter* SourceCharacter = nullptr;
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if (SourceController == nullptr && SourceActor != nullptr)
			{
				if (APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			// Use the controller to find the source pawn
			if (SourceController)
			{
				SourceCharacter = Cast<ADungeonCharacter>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter = Cast<ADungeonCharacter>(SourceActor);
			}

			// Set the causer actor based on context if it's set
			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}

		// Try to extract a hit result
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0)
		{
			// Apply the health change and then clamp it
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.0f, GetMaxHealth()));

			if (TargetCharacter)
			{
				// This is proper damage
				//TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor);

				// Call for all health changes
				//TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle other health changes such as from healing or direct modifiers
		// First clamp it
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		if (TargetCharacter)
		{
			// Call for all health changes
			//TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Clamp mana
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));

		if (TargetCharacter)
		{
			// Call for all mana changes
			//TargetCharacter->HandleManaChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		// Clamp stamina
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));

		if (TargetCharacter)
		{
			// Call for all stamina changes
			TargetCharacter->HandleStaminaChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		if (TargetCharacter)
		{
			// Call for all movespeed changes
			TargetCharacter->HandleMoveSpeedChanged(DeltaValue, SourceTags);
		}
	}
}

void UDungeonAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UDungeonAttributeSet::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, Health);
}

void UDungeonAttributeSet::OnRep_MaxHealth()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, MaxHealth);
}

void UDungeonAttributeSet::OnRep_HealthRegen()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, HealthRegen);
}

void UDungeonAttributeSet::OnRep_Mana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, Mana);
}

void UDungeonAttributeSet::OnRep_MaxMana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, MaxMana);
}

void UDungeonAttributeSet::OnRep_ManaRegen()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, ManaRegen);
}

void UDungeonAttributeSet::OnRep_Stamina()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, Stamina);
}

void UDungeonAttributeSet::OnRep_MaxStamina()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, MaxStamina);
}

void UDungeonAttributeSet::OnRep_StaminaRegen()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, StaminaRegen);
}

void UDungeonAttributeSet::OnRep_AttackPower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, AttackPower);
}

void UDungeonAttributeSet::OnRep_DefensePower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, DefensePower);
}

void UDungeonAttributeSet::OnRep_MoveSpeed()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDungeonAttributeSet, MoveSpeed);
}
