#ifndef LOGIC
#define LOGIC

class Logic
{
public:

	Logic(void);
	virtual ~Logic(void);

    int ScrollNumberMoved;
    int ScrollSpeed;

    int CommandDisplayStartIndex;
    int CommandDisplayEndIndex;

    int CommandSelectedByMouseOld;
    int CommandSelectedByMouse;

    #define NumberOfCodes     65000
    struct Code
    {
        int CodeCommandIndex;

    } Codes[NumberOfCodes];

    int CurrentCodeLine;

    int CodeLastLine;

    int CodeDisplayStartIndex;
    int CodeDisplayEndIndex;

    int CodeSelectedByMouseOld;
    int CodeSelectedByMouse;

    int CodeSelectedForEdit;
    int CodeSelectorSelected;

    void CalculateCodeLastLine(void);

    void ShowHideCodeSelectBoxes(void);

    void RunCodeEditor(void);

};

#endif

