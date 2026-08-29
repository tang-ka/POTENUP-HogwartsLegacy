# Hogwarts Legacy - Guardian Combat

> 호그와트 레거시의 가디언 전투 구간을 재현한 3인 팀 프로젝트

<!-- TODO: 가디언 전투 + 파괴 연출 대표 GIF 1개 -->

<table>
  <tr><td><b>기간</b></td><td>2026.02 ~ 2026.03 (1개월)</td></tr>
  <tr><td><b>인원</b></td><td>3인</td></tr>
  <tr><td><b>엔진</b></td><td>Unreal Engine 5.7 / C++</td></tr>
  <tr><td><b>담당</b></td><td>Guardian(보스) AI · 스폰/전투 중재 · 런타임 메시 파괴</td></tr>
</table>

---

## 목차
 
- [코드 가이드](#코드-가이드)
  - [1. 런타임 스켈레탈 메시 파괴 (Shatter)](#1-런타임-스켈레탈-메시-파괴-shatter)
  - [2. Guardian FSM](#2-guardian-fsm)
  - [3. 다수 적 전투 중재 (GuardianManager)](#3-다수-적-전투-중재-guardianmanager)
- [그 외 담당](#그-외-담당)
- [팀 담당 범위](#팀-담당-범위)
- [실행](#실행)

---

## 코드 가이드

### 1. 런타임 스켈레탈 메시 파괴 (Shatter)

가디언 사망 시 스켈레탈 메시를 본 그룹 단위로 분해해 물리 파편으로 전환한다.
사전 제작 파편 에셋 없이, 사망 시점의 **포즈를 그대로 반영한** 파편을 런타임에 생성한다.

처리 순서:

1. 스켈레톤의 모든 본을 부모 방향으로 거슬러 올라가며 `ShatterBoneGroup` 중 하나로 분류
2. 스킨 웨이트를 기준으로 전체 버텍스를 그룹에 배정
3. 현재 포즈의 CPU 스키닝 결과(Position / TangentX / TangentZ / UV)를 **1회만** 수집해 `FSharedMeshSnapshot`으로 전체 청크가 읽기 전용 공유
4. 삼각형을 1회 순회해 그룹별 `FChunkDescriptor` 생성
5. 각 청크가 **워커 스레드**에서 인덱스 리매핑 · 절단면 캡 메시 생성을 수행하고, 결과만 게임 스레드에서 `ProceduralMeshComponent`에 적용
6. 청크마다 다른 대기 프레임을 부여해 메시 적용과 물리 활성화를 프레임 단위로 분산

| 역할 | 파일 |
|---|---|
| 본/버텍스 분류, 스냅샷 수집, 청크 생성 | [`GuardianShatterComponent.cpp`](https://github.com/tang-ka/POTENUP-HogwartsLegacy/blob/335567677025c39e1508140fbbb464576cc13f15/Source/HogwartsLegacy/private/GuardianShatterComponent.cpp) |
| 비동기 메시 빌드, 캡 생성, 지연 물리 활성화 | [`GuardianChunk.cpp`](https://github.com/tang-ka/POTENUP-HogwartsLegacy/blob/335567677025c39e1508140fbbb464576cc13f15/Source/HogwartsLegacy/private/GuardianChunk.cpp) |
| 공유 스냅샷 / 청크 기술자 자료구조 | [`GuardianChunk.h`](https://github.com/tang-ka/POTENUP-HogwartsLegacy/blob/335567677025c39e1508140fbbb464576cc13f15/Source/HogwartsLegacy/public/GuardianChunk.h) |

---

### 2. Guardian FSM

State 패턴 기반 상태 기계. 9개 상태(`Sleep` / `Idle` / `Strafe` / `ChargeAttack` / `BashAttack` / `Stumble` / `Damaged` / `Stun` / `Dead`)를
`UGuardianStateBase` 파생 `UObject`로 분리해, 상태별 로직이 `AGuardian`에 누적되지 않도록 했다.

- 상태 인스턴스를 `TMap<EEnemyState, UGuardianStateBase*>`에 **미리 생성해 캐싱** — 전이마다 할당하지 않음
- 외부는 `RequestAttack()` / `RequestDamaged()` / `RequestDead()` 등 요청 함수만 호출하고, 실제 전이 승인 여부는 FSM이 판단
- 상태 변경 · 피격 · 사망 · 공격 종료를 Multicast Delegate로 방송해 UI·매니저가 폴링 없이 반응
- `bIsChanging` 플래그로 전이 도중 재진입 차단

| 역할 | 파일 |
|---|---|
| 상태 전이 · 전이 요청 처리 | [`EnemyFSMComponent.cpp`](https://github.com/tang-ka/POTENUP-HogwartsLegacy/blob/335567677025c39e1508140fbbb464576cc13f15/Source/HogwartsLegacy/private/Enemy/EnemyFSMComponent.cpp) |
| 상태 추상 클래스 | [`GuardianStateBase.h`](https://github.com/tang-ka/POTENUP-HogwartsLegacy/blob/335567677025c39e1508140fbbb464576cc13f15/Source/HogwartsLegacy/public/Enemy/GuardianState/GuardianStateBase.h) |
| 개별 상태 구현 (8종) | [`GuardianState/`](https://github.com/tang-ka/POTENUP-HogwartsLegacy/tree/335567677025c39e1508140fbbb464576cc13f15/Source/HogwartsLegacy/private/Enemy/GuardianState) |
| 돌진 · 강타 · 넉백 실행부 | [`Guardian.cpp`](https://github.com/tang-ka/POTENUP-HogwartsLegacy/blob/335567677025c39e1508140fbbb464576cc13f15/Source/HogwartsLegacy/private/Enemy/Guardian.cpp) |

---

### 3. 다수 적 전투 중재 (GuardianManager)

`UWorldSubsystem`으로 구현한 전역 중재자. 개체가 각자 판단하면 여러 마리가 동시에 달려드는 문제가 생기므로,
**공격 권한을 매니저가 독점하고 한 번에 한 개체에게만 지시**하는 구조로 설계했다.

- 일정 주기마다 생존 개체를 순회하며 거리·시야각 가중합으로 점수를 매겨 공격자 1명 선정
- 공격 종료/사망 시 콜백으로 공격권 반환
- 최대 동시 생존 수를 유지하도록 주기적으로 검사 → 스폰 큐에 적재 → 간격을 두고 순차 스폰
- 스폰 지점은 원형 배치로 런타임 생성하고, 플레이어 시야·충돌을 검사해 안전한 지점만 선택

| 역할 | 파일 |
|---|---|
| 공격자 선정 · 스폰 큐 · 페이즈 관리 | [`GuardianManager.cpp`](https://github.com/tang-ka/POTENUP-HogwartsLegacy/blob/335567677025c39e1508140fbbb464576cc13f15/Source/HogwartsLegacy/private/Enemy/GuardianManager.cpp) |
| 스폰 지점 안전성 판정 | [`EnemySpawnPoint.cpp`](https://github.com/tang-ka/POTENUP-HogwartsLegacy/blob/335567677025c39e1508140fbbb464576cc13f15/Source/HogwartsLegacy/private/Enemy/EnemySpawnPoint.cpp) |

---

## 그 외 담당

- `IDamageable` 인터페이스 — 피격 처리를 인터페이스로 통일해 플레이어/적 구현을 분리 ([`Damageable.h`](https://github.com/tang-ka/POTENUP-HogwartsLegacy/blob/335567677025c39e1508140fbbb464576cc13f15/Source/HogwartsLegacy/public/Interface/Damageable.h))
- `GuardianAnim` — 이동 방향 기반 스트레이프 블렌드, 상태 연동 애님 인스턴스
- `GuardianCombatGameMode` — 전투 레벨 진행 및 게임 오버/클리어 처리

## 팀 담당 범위

| 파트 | 담당 |
|---|---|
| Guardian AI · FSM · 전투 중재 · 메시 파괴 | 🟢 본인 |
| 플레이어 캐릭터 · 주문(Lumos / Protego / Stupefy) · 콤보 | ⚪ 팀원 |
| UI · HUD · 미니맵 | ⚪ 팀원 |

## 실행

Unreal Engine 5.7 필요. `HogwartsLegacy.uproject` 우클릭 → Generate Visual Studio project files → 빌드 후 실행.
