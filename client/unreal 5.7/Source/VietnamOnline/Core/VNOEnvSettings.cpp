#include "Core/VNOEnvSettings.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool UVNOEnvSettings::LoadEnvFile()
{
    Vars_.Empty();
    FString Path = FPaths::Combine(FPaths::ProjectDir(), TEXT("../../.env"));
    FString Content;
    if (!FFileHelper::LoadFileToString(Content, *Path))
    {
        UE_LOG(LogTemp, Warning, TEXT("[VNO] .env not found at %s"), *Path);
        return false;
    }

    TArray<FString> Lines;
    Content.ParseIntoArrayLines(Lines);
    for (const FString& Line : Lines)
    {
        FString Trimmed = Line.TrimStartAndEnd();
        if (Trimmed.IsEmpty() || Trimmed.StartsWith(TEXT("#"))) continue;
        FString Key, Val;
        if (!Trimmed.Split(TEXT("="), &Key, &Val)) continue;
        Key = Key.TrimStartAndEnd();
        Val = Val.TrimStartAndEnd();
        if (!Key.IsEmpty()) Vars_.Add(Key, Val);
    }
    UE_LOG(LogTemp, Log, TEXT("[VNO] Loaded .env (%d keys) from %s"), Vars_.Num(), *Path);
    return true;
}

FString UVNOEnvSettings::Get(const FString& Key, const FString& DefaultValue) const
{
    if (const FString* Found = Vars_.Find(Key)) return *Found;
    return DefaultValue;
}

bool UVNOEnvSettings::GetBool(const FString& Key, bool bDefault) const
{
    FString V = Get(Key, TEXT(""));
    if (V.IsEmpty()) return bDefault;
    V = V.ToLower();
    return V == TEXT("1") || V == TEXT("true") || V == TEXT("yes") || V == TEXT("on");
}

float UVNOEnvSettings::GetFloat(const FString& Key, float DefaultValue) const
{
    FString V = Get(Key, TEXT(""));
    if (V.IsEmpty()) return DefaultValue;
    return FCString::Atof(*V);
}
