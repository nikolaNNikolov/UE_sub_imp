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

#define OPEN_FILE_HINT_STRING FString("Choose subtitles file...")

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
	
	bool GetIsGenerateButtonEnabled() const;
	bool IsLoadedPathStringValid() const;
	
	FText GetLoadedFileStringAsText() const;
	FText GetLoadedFileTooltipText() const;

	void OnSoundWaveSelected(const FAssetData& InSoundWave);
	FString GetSelectedSoundWavePath() const;

	void OnStringTableSelected(const FAssetData& InStringTable);
	FString GetSelectedStringTablePath() const;

	void OnStringTableCheckboxStateChanged(ECheckBoxState bIsChecked);
	ECheckBoxState GetStringTableCheckBoxState() const;
	EVisibility GetStringTableSettingsVisibility() const;

	void OnApplyEndSubTagStatChanged(ECheckBoxState bIsChecked);
	ECheckBoxState GetApplyEndSubTagCheckBoxState() const;
	EVisibility GetEndSubTagSettingsVisibility() const;
	
	void EndSubTagTextBlockCommitted(const FText& Text, ETextCommit::Type CommitType);
	FText GetEndSubTag() const;
	
	void EndSubTagTimeoutNumericCommitted(float Value, ETextCommit::Type CommitType);
	float GetEndSubTagTimeout() const;

private:
	TSharedPtr<SWindow> ParentWindow;
	TArray<FSubtitleCue> GeneratedSubtitleInfo;
	
	USoundWave* SelectedSoundWave;
	FString SelectedSoundWavePath;

	bool bUseStringTableForSubtitles;
	UStringTable* SelectedStringTable;
	FString SelectedStringTablePath;

	bool bApplyEndSubTag = false;
	float EndSubTagTimeout = 3.0f;
	FText EndSubTag = FText::FromString("NO_SUB");

	void ResetSubImp();
	void GenerateSubtitleCueArrayFromReadFile();

	float GetTotalSecondsFromTimespanString(const FString& TimespanString) const;
	
};
