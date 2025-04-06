//Sub_Imp Plugin
#pragma once

static TWeakPtr<SWindow> CurrentSubImpWindow;
static void OpenSubImpWindow(TSharedPtr<SWindow> ParentWindow);
static void CloseSubImpWindow();

#define TIMESPAN_DELIMITER TEXT(":")
#define SUBTITLE_TIME_DELIMITER TEXT("-->")
#define OPEN_FILE_DIALOG_TITLE FString("Choose subtitles file")
#define OPEN_FILE_DIALOG_DEFAULT_FILE FString("")
#define OPEN_FILE_DIALOG_FILE_TYPES FString("Subtitles|*.srt")
#define OPEN_FILE_DIALOG_FLAGS 0

enum ESubImpLineType
{
	EmptyLine		= 0,
	SubtitleIndex	= 1,
	SubtitleTime	= 2,
	SubtitleText	= 3
};

class SUBIMP_API SSubImpWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSubImpWindow)
	{}
	SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
	
	SLATE_END_ARGS()
	
	SSubImpWindow();

	void Construct(const FArguments& InArgs);

protected:
	FString LoadedFileString;

	FReply OpenSRTFilePickerWindow();
	FReply DoTheSubImp();
	void OnSoundWaveSelected(const FAssetData& InSoundWave);

	bool GetIsGenerateButtonEnabled() const;
	FText GetLoadedFileStringAsText() const;
	FString GetSelectedSoundWavePath() const;

private:
	TSharedPtr<SWindow> ParentWindow;
	TArray<FSubtitleCue> GeneratedSubtitleInfo;
	
	USoundWave* SelectedSoundWave;
	FString SelectedSoundWavePath;

	float EndSubTagTimeout = 3.0f;
	FText EndSubTag = FText::FromString("NO_SUB");

	void ResetSubImp();

	float GetTotalSecondsFromTimespanString(const FString& TimespanString) const;
	
};
