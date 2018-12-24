#ifndef LOGICCODE
#define LOGICCODE

class LogicCode
{
public:

	LogicCode(void);
	virtual ~LogicCode(void);

    int ScrollNumberMoved;
    int ScrollSpeed;

    int CommandDisplayStartIndex;
    int CommandDisplayEndIndex;

    int CommandSelectedByMouseOld;
    int CommandSelectedByMouse;

    #define NumberOfCodes     65000
    struct Code
    {
        int CodeCommandLineNumber;
        int CodeCommandIndex;
        bool CodeCommandLineActive;

    } Codes[NumberOfCodes];

    struct CacheCode
    {
        int CodeCommandLineNumber;
        int CodeCommandIndex;
        bool CodeCommandLineActive;

    } CacheCodes[NumberOfCodes];

    int CurrentCodeLine;

    int CodeLastLine;

    int CodeDisplayStartIndex;
    int CodeDisplayEndIndex;

    int CodeSelectedByMouseOld;
    int CodeSelectedByMouse;

    int CodeSelectedForEdit;
    int CodeSelectorSelected;

    int CodeSelectedForLineNumberEdit;

    int CommandBoxMaxY;
    int CodeBoxMaxY;

    int CodeBoxOffsetY;

    int EditLineSelectedByMouse;
    int EditFirstLine;
    int EditLastLine;
    int EditLocation;

    void CalculateCodeLastLine(void);

    bool ThereIsCodeAfterThisLine(int line);
    void ShowHideCodeSelectLineNumberBoxes(void);

    #define DialogNothing               0
    #define DialogLineNumberSelect      1
    #define DialogClearCode             2
    #define DialogFindLineNumber        3
    #define DialogEdit                  4
    int DialogToShow;
    int DialogToShowOld;

    int LineNumberArray[3];

    bool ThereAreLineNumbers(void);

    void ClearAllCodeSelectors(void);

    void SetupClickableButtons(void);
    void CheckForClearButton(void);
    void CheckForInsertButton(void);
    void CheckForDeleteButton(void);
    void CheckForFindButton(void);

    void CheckForEditButton(void);
    void CheckForEditQuitButton(void);
    void CheckForEditDelButton(void);
    void CheckForEditMoveButton(void);
    void CheckForEditCopyButton(void);

    void CheckForScrollArrowButtons(void);
    void CheckForCodeSelectButtons(void);
    void CheckForCodeLineSelectButtons(void);

    void RunCodeEditor(void);

};

#endif

