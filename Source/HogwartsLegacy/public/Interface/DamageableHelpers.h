// DamageableHelpers.h
#pragma once

#include "CoreMinimal.h"
#include "Damageable.h" // UDamageable, FDamageInfo, IDamageable

namespace DamageableHelpers
{
	// 공통 체크: Damageable인가?
	inline bool IsDamageable(const UObject* Obj)
	{
		return IsValid(Obj) && Obj->GetClass()->ImplementsInterface(UDamageable::StaticClass());
	}

	// 데미지 적용 
	inline bool ApplyDamage(AActor* Target, AActor* Causer, float Amount)
	{
		if (!IsDamageable(Target)) return false;

		FDamageInfo Info
		{
			.DamageCauser = Causer,
			.DamageAmount = Amount
		};

		// 인터페이스는 무조건 Execute_로 호출
		return IDamageable::Execute_ReceiveDamage(Target, Info);
	}

	// 이미 FDamageInfo가 있을 때
	inline bool ApplyDamage(AActor* Target, const FDamageInfo& Info)
	{
		if (!IsDamageable(Target)) return false;
		return IDamageable::Execute_ReceiveDamage(Target, Info);
	}

	// 조회 계열도 같이 감싸두면 호출부가 더 깨끗해짐
	inline float GetHP(const UObject* Target, float DefaultValue = 0.f)
	{
		if (!IsDamageable(Target)) return DefaultValue;
		return IDamageable::Execute_GetHP(const_cast<UObject*>(Target));
	}

	inline bool IsDead(const UObject* Target, bool DefaultValue = false)
	{
		if (!IsDamageable(Target)) return DefaultValue;
		return IDamageable::Execute_IsDead(const_cast<UObject*>(Target));
	}
}