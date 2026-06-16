#include "SaveGame/VNOSupabaseSubsystem.h"
#include "Core/VNOGameInstance.h"
#include "Core/VNOEnvSettings.h"
#include "Player/VNOPlayerState.h"
#include "Player/VNOPlayerCharacter.h"
// @codebuff Supabase save/load: quest progress serialization restored on load
#include "Quest/VNOQuestSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void UVNOSupabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (UVNOGameInstance* GI = Cast<UVNOGameInstance>(GetGameInstance()))
    {
        if (UVNOEnvSettings* Env = GI->GetEnv())
        {
            SupabaseUrl = Env->Get(TEXT("SUPABASE_URL"), TEXT(""));
            AnonKey = Env->Get(TEXT("SUPABASE_ANON_KEY"), TEXT(""));
            bDevSkipAuth = Env->GetBool(TEXT("VNO_DEV_SKIP_AUTH"), false);
            bConfigured = SupabaseUrl.Contains(TEXT("supabase.co")) && !AnonKey.IsEmpty();
        }
    }
    if (bConfigured)
    {
        UE_LOG(LogTemp, Log, TEXT("[Supabase] Configured — cloud save active"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[Supabase] Not configured — local only"));
    }
}

void UVNOSupabaseSubsystem::CloudSaveAll()
{
    if (!bConfigured) return;
    UWorld* World = GetWorld();
    if (!World) return;
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return;
    AVNOPlayerState* PS = PC->GetPlayerState<AVNOPlayerState>();
    AVNOPlayerCharacter* Ch = Cast<AVNOPlayerCharacter>(PC->GetPawn());
    if (PS && Ch && PS->IsCharacterReady())
    {
        PS->VndBalance = Ch->GetVndBalance();
        SaveCharacterProfile(PS, Ch);
    }
}

void UVNOSupabaseSubsystem::SaveOnQuit()
{
    CloudSaveAll();
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Da luu game truoc khi thoat!"));
    // Give HTTP request time to complete (async)
    FPlatformProcess::Sleep(0.5f);
}

void UVNOSupabaseSubsystem::SignIn(const FString& Email, const FString& Password)
{
    if (!bConfigured) { OnAuthCompleted.Broadcast(false, TEXT("Supabase chua cau hinh")); return; }

    TSharedRef<FJsonObject> Body = MakeShared<FJsonObject>();
    Body->SetStringField(TEXT("email"), Email);
    Body->SetStringField(TEXT("password"), Password);
    FString BodyStr;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyStr);
    FJsonSerializer::Serialize(Body, Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
    Req->SetURL(SupabaseUrl + TEXT("/auth/v1/token?grant_type=password"));
    Req->SetVerb(TEXT("POST"));
    Req->SetHeader(TEXT("apikey"), AnonKey);
    Req->SetHeader(TEXT("Authorization"), FString(TEXT("Bearer ")) + AnonKey);
    Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Req->SetContentAsString(BodyStr);
    Req->OnProcessRequestComplete().BindUObject(this, &UVNOSupabaseSubsystem::OnAuthResponse, false);
    Req->ProcessRequest();
}

void UVNOSupabaseSubsystem::SignUp(const FString& Email, const FString& Password)
{
    if (!bConfigured) { OnAuthCompleted.Broadcast(false, TEXT("Supabase chua cau hinh")); return; }

    TSharedRef<FJsonObject> Body = MakeShared<FJsonObject>();
    Body->SetStringField(TEXT("email"), Email);
    Body->SetStringField(TEXT("password"), Password);
    FString BodyStr;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyStr);
    FJsonSerializer::Serialize(Body, Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
    Req->SetURL(SupabaseUrl + TEXT("/auth/v1/signup"));
    Req->SetVerb(TEXT("POST"));
    Req->SetHeader(TEXT("apikey"), AnonKey);
    Req->SetHeader(TEXT("Authorization"), FString(TEXT("Bearer ")) + AnonKey);
    Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Req->SetContentAsString(BodyStr);
    Req->OnProcessRequestComplete().BindUObject(this, &UVNOSupabaseSubsystem::OnAuthResponse, true);
    Req->ProcessRequest();
}

void UVNOSupabaseSubsystem::OnAuthResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess, bool bSignUp)
{
    if (!bSuccess || !Response.IsValid() || Response->GetResponseCode() < 200 || Response->GetResponseCode() >= 300)
    {
        FString Msg = Response.IsValid() ? Response->GetContentAsString() : TEXT("Network error");
        OnAuthCompleted.Broadcast(false, Msg);
        return;
    }

    TSharedPtr<FJsonObject> Root;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        OnAuthCompleted.Broadcast(false, TEXT("JSON parse error"));
        return;
    }

    FString Token = Root->GetStringField(TEXT("access_token"));
    if (Token.IsEmpty())
    {
        OnAuthCompleted.Broadcast(false, bSignUp ? TEXT("Kiem tra email xac nhan") : TEXT("Sai email/mat khau"));
        return;
    }

    FString UserId;
    const TSharedPtr<FJsonObject>* UserObj;
    if (Root->TryGetObjectField(TEXT("user"), UserObj))
        UserId = (*UserObj)->GetStringField(TEXT("id"));

    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            if (AVNOPlayerState* PS = PC->GetPlayerState<AVNOPlayerState>())
            {
                PS->AccessToken = Token;
                PS->UserId = UserId;
            }
        }
    }
    OnAuthCompleted.Broadcast(true, UserId);
}

void UVNOSupabaseSubsystem::SaveCharacterProfile(AVNOPlayerState* PlayerState, AVNOPlayerCharacter* Character)
{
    if (!PlayerState || !Character || PlayerState->AccessToken.IsEmpty())
    {
        OnSaveCompleted.Broadcast(false, TEXT("Chua dang nhap"));
        return;
    }

    PlayerState->VndBalance = Character->GetVndBalance();

    FVector Loc = Character->GetActorLocation();
    TSharedRef<FJsonObject> Body = MakeShared<FJsonObject>();
    Body->SetStringField(TEXT("user_id"), PlayerState->UserId);
    Body->SetStringField(TEXT("display_name"), PlayerState->DisplayName);
    Body->SetStringField(TEXT("zone_id"), TEXT("HOI_AN"));

    TSharedRef<FJsonObject> Pos = MakeShared<FJsonObject>();
    Pos->SetNumberField(TEXT("x"), Loc.X);
    Pos->SetNumberField(TEXT("y"), Loc.Y);
    Pos->SetNumberField(TEXT("z"), Loc.Z);
    Body->SetObjectField(TEXT("position_json"), Pos);

    TSharedRef<FJsonObject> App = MakeShared<FJsonObject>();
    App->SetStringField(TEXT("gender"), PlayerState->Gender);
    App->SetStringField(TEXT("skin_tone"), PlayerState->SkinTone);
    Body->SetObjectField(TEXT("appearance_json"), App);

    TSharedRef<FJsonObject> Stats = MakeShared<FJsonObject>();
    Stats->SetNumberField(TEXT("explorer_lv"), PlayerState->ExplorerLevel);
    Body->SetObjectField(TEXT("stats_json"), Stats);
    Body->SetNumberField(TEXT("wallet_vnd"), static_cast<double>(PlayerState->VndBalance));

    // Quest progress
    if (UWorld* W = GetWorld())
    {
        if (UVNOQuestSubsystem* Q = W->GetSubsystem<UVNOQuestSubsystem>())
        {
            TArray<TSharedPtr<FJsonValue>> QuestArr;
            for (const FVNOQuestRuntime& R : Q->GetQuests())
            {
                TSharedPtr<FJsonObject> QJ = MakeShared<FJsonObject>();
                QJ->SetStringField(TEXT("id"), R.QuestId);
                QJ->SetBoolField(TEXT("active"), R.bActive);
                QJ->SetNumberField(TEXT("steps_done"), R.StepsDone);
                QuestArr.Add(MakeShared<FJsonValueObject>(QJ));
            }
            Body->SetArrayField(TEXT("quests_json"), QuestArr);
        }
    }

    FString BodyStr;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyStr);
    FJsonSerializer::Serialize(Body, Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
    Req->SetURL(SupabaseUrl + TEXT("/rest/v1/characters"));
    Req->SetVerb(TEXT("POST"));
    Req->SetHeader(TEXT("apikey"), AnonKey);
    Req->SetHeader(TEXT("Authorization"), FString(TEXT("Bearer ")) + PlayerState->AccessToken);
    Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Req->SetHeader(TEXT("Prefer"), TEXT("resolution=merge-duplicates"));
    Req->SetContentAsString(BodyStr);
    Req->OnProcessRequestComplete().BindUObject(this, &UVNOSupabaseSubsystem::OnSaveResponse);
    Req->ProcessRequest();
}

void UVNOSupabaseSubsystem::OnSaveResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    const bool Ok = bSuccess && Response.IsValid() &&
        (Response->GetResponseCode() == 200 || Response->GetResponseCode() == 201);
    OnSaveCompleted.Broadcast(Ok, Ok ? TEXT("Da luu Supabase (FR-10)") : Response->GetContentAsString());
}

void UVNOSupabaseSubsystem::LoadCharacterProfile(AVNOPlayerState* PlayerState, AVNOPlayerCharacter* Character)
{
    if (!PlayerState || !Character || PlayerState->AccessToken.IsEmpty())
    {
        OnSaveCompleted.Broadcast(false, TEXT("Chua dang nhap"));
        return;
    }
    PendingLoadPlayerState = PlayerState;
    PendingLoadCharacter = Character;

    FString Url = FString::Printf(TEXT("%s/rest/v1/characters?user_id=eq.%s&select=*"),
        *SupabaseUrl, *PlayerState->UserId);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
    Req->SetURL(Url);
    Req->SetVerb(TEXT("GET"));
    Req->SetHeader(TEXT("apikey"), AnonKey);
    Req->SetHeader(TEXT("Authorization"), FString(TEXT("Bearer ")) + PlayerState->AccessToken);
    Req->OnProcessRequestComplete().BindUObject(this, &UVNOSupabaseSubsystem::OnLoadResponse);
    Req->ProcessRequest();
}

void UVNOSupabaseSubsystem::OnLoadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    AVNOPlayerState* PS = PendingLoadPlayerState.Get();
    AVNOPlayerCharacter* Ch = PendingLoadCharacter.Get();
    if (!PS || !Ch) return;

    if (!bSuccess || !Response.IsValid() || Response->GetResponseCode() != 200)
    {
        OnSaveCompleted.Broadcast(false, TEXT("Khong tai duoc profile"));
        return;
    }

    TArray<TSharedPtr<FJsonValue>> Arr;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    if (!FJsonSerializer::Deserialize(Reader, Arr) || Arr.Num() == 0)
    {
        OnSaveCompleted.Broadcast(true, TEXT("Chua co save cloud"));
        return;
    }

    TSharedPtr<FJsonObject> Row = Arr[0]->AsObject();
    PS->DisplayName = Row->GetStringField(TEXT("display_name"));
    PS->VndBalance = static_cast<int64>(Row->GetNumberField(TEXT("wallet_vnd")));
    PS->bCharacterCreated = true;
    Ch->SetVndBalance(PS->VndBalance);

    const TSharedPtr<FJsonObject>* PosObj;
    if (Row->TryGetObjectField(TEXT("position_json"), PosObj))
    {
        FVector Loc;
        Loc.X = static_cast<float>((*PosObj)->GetNumberField(TEXT("x")));
        Loc.Y = static_cast<float>((*PosObj)->GetNumberField(TEXT("y")));
        Loc.Z = static_cast<float>((*PosObj)->GetNumberField(TEXT("z")));
        Ch->SetActorLocation(Loc);
    }
    const TSharedPtr<FJsonObject>* AppObj;
    if (Row->TryGetObjectField(TEXT("appearance_json"), AppObj))
    {
        PS->Gender = (*AppObj)->GetStringField(TEXT("gender"));
        PS->SkinTone = (*AppObj)->GetStringField(TEXT("skin_tone"));
    }
    Ch->ApplyAppearanceFromState(PS);

    // Restore quest progress from cloud
    const TArray<TSharedPtr<FJsonValue>>* QuestArr;
    if (Row->TryGetArrayField(TEXT("quests_json"), QuestArr))
    {
        if (UWorld* W = GetWorld())
        {
            if (UVNOQuestSubsystem* Q = W->GetSubsystem<UVNOQuestSubsystem>())
            {
                for (const TSharedPtr<FJsonValue>& QV : *QuestArr)
                {
                    TSharedPtr<FJsonObject> QO = QV->AsObject();
                    if (!QO) continue;
                    FString QId = QO->GetStringField(TEXT("id"));
                    bool bActive = QO->GetBoolField(TEXT("active"));
                    int32 StepsDone = static_cast<int32>(QO->GetNumberField(TEXT("steps_done")));
                    Q->RestoreQuestProgress(QId, bActive, StepsDone);
                }
                UE_LOG(LogTemp, Log, TEXT("[Save] Restored %d quests from cloud"), QuestArr->Num());
            }
        }
    }

    OnSaveCompleted.Broadcast(true, TEXT("Tai cloud thanh cong"));
}
