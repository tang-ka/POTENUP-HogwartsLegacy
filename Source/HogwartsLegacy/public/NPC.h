#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

UENUM(BlueprintType)
enum class ENPCState : uint8
{
    Idle    UMETA(DisplayName = "Idle"),
    Follow  UMETA(DisplayName = "Follow"),
    Combat  UMETA(DisplayName = "Combat"),
    Damaged UMETA(DisplayName = "Damaged"),
};

UCLASS()
class HOGWARTSLEGACY_API ANPC : public ACharacter
{
    GENERATED_BODY()

public:
    ANPC();

protected:
    virtual void BeginPlay() override;

public: 
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|State")
    ENPCState CurrentState = ENPCState::Idle;

    UFUNCTION(BlueprintCallable, Category = "AI|State")
    void SetNPCState(ENPCState NewState);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MovementSpeed = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BaseDamage = 20.0f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Support AI")
    class ACharacter* PlayerRef;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float FollowDistance = 80.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float DetectionRadius = 800.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    AActor* TargetEnemy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackRange = 250.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    bool bCanAttack = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackCooldown = 2.0f;
    
    UFUNCTION(BlueprintCallable, Category = "AI")
    void ClearTargetEnemy();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // --- 애니메이션용 변수 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    float Speed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsCombat;
};