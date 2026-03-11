#include "NPC.h"
#include "AIController.h"
#include "../SpellBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

ANPC::ANPC()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentState = ENPCState::Idle;
    Speed = 0.0f;
    bIsCombat = false;
    bIsAttacking = false;
}

void ANPC::BeginPlay()
{
    Super::BeginPlay();

    PlayerRef = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;

    if (PlayerRef)
    {
        SetNPCState(ENPCState::Follow);
    }
}

void ANPC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Speed = GetVelocity().Size();

    switch (CurrentState)
    {
    case ENPCState::Idle:
        if (PlayerRef)
            SetNPCState(ENPCState::Follow);
        break;

    case ENPCState::Follow:
        UpdateFollow(DeltaTime);
        break;

    case ENPCState::Combat:
        UpdateCombat(DeltaTime);
        break;

    default:
        break;
    }
}

void ANPC::UpdateFollow(float DeltaTime)
{
    if (!PlayerRef) return;

    AActor* NearestEnemy = FindNearestEnemy();
    if (NearestEnemy)
    {
        TargetEnemy = NearestEnemy;
        bIsCombat = true;
        SetNPCState(ENPCState::Combat);
        return;
    }

    AAIController* AIC = Cast<AAIController>(GetController());
    if (!AIC) return;

    float DistToPlayer = FVector::Dist(GetActorLocation(), PlayerRef->GetActorLocation());

    if (DistToPlayer > FollowDistance)
    {
        AIC->MoveToActor(PlayerRef, FollowDistance);
    }
    else
    {
        AIC->StopMovement();
    }
}

void ANPC::UpdateCombat(float DeltaTime)
{
    if (!TargetEnemy || !IsValid(TargetEnemy))
    {
        ClearTargetEnemy();
        SetNPCState(ENPCState::Follow);
        return;
    }

    AAIController* AIC = Cast<AAIController>(GetController());
    if (!AIC) return;

    AIC->SetFocus(TargetEnemy, EAIFocusPriority::Gameplay);

    float DistToEnemy = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());

    if (DistToEnemy <= AttackRange)
    {
        AIC->StopMovement();
        TryAttack();
    }
    else
    {
        AIC->MoveToActor(TargetEnemy, AttackRange * 0.8f);
    }

    if (DistToEnemy > DetectionRadius * 1.5f)
    {
        ClearTargetEnemy();
        SetNPCState(ENPCState::Follow);
    }
}

AActor* ANPC::FindNearestEnemy()
{
    TArray<AActor*> OverlappedActors;
    TArray<AActor*> ActorsToIgnore = { this, PlayerRef };

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        GetActorLocation(),
        DetectionRadius,
        TArray<TEnumAsByte<EObjectTypeQuery>>(),
        nullptr,
        ActorsToIgnore,
        OverlappedActors
    );

    AActor* Nearest = nullptr;
    float MinDist = FLT_MAX;

    for (AActor* Actor : OverlappedActors)
    {
        if (!Actor->ActorHasTag(EnemyTag)) continue;

        float Dist = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
        if (Dist < MinDist)
        {
            MinDist = Dist;
            Nearest = Actor;
        }
    }

    return Nearest;
}

void ANPC::TryAttack()
{
    if (!bCanAttack || !TargetEnemy) return;

    bCanAttack = false;
    bIsAttacking = true;

    if (SpellClass)
    {
        FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 50.0f;
        FRotator SpawnRotation = (TargetEnemy->GetActorLocation() - SpawnLocation).Rotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();

        ASpellBase* Spell = GetWorld()->SpawnActor<ASpellBase>(SpellClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (Spell)
        {
            Spell->SpellOwner = this;
            Spell->BaseDamage = BaseDamage;
        }
    }

    GetWorldTimerManager().SetTimer(
        AttackCooldownTimer,
        this,
        &ANPC::ResetAttackCooldown,
        AttackCooldown,
        false
    );
}

void ANPC::ResetAttackCooldown()
{
    bCanAttack = true;
    bIsAttacking = false;
}

void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANPC::SetNPCState(ENPCState NewState)
{
    CurrentState = NewState;
}

void ANPC::ClearTargetEnemy()
{
    TargetEnemy = nullptr;
    bIsCombat = false;
    bIsAttacking = false;

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        AIC->ClearFocus(EAIFocusPriority::Gameplay);
    }

    CurrentState = ENPCState::Idle;
}

float ANPC::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                        class AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (DamageCauser != nullptr)
    {
        TargetEnemy = DamageCauser;
        bIsCombat = true;
    }

    SetNPCState(ENPCState::Combat);
    return ActualDamage;
}