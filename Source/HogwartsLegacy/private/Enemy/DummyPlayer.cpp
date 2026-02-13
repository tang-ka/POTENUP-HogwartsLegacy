// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DummyPlayer.h"

// Sets default values
ADummyPlayer::ADummyPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADummyPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADummyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
 
}

// Called to bind functionality to input
void ADummyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

