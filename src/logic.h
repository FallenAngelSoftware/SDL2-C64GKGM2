#ifndef LOGIC
#define LOGIC

class Logic
{
public:

	Logic(void);
	virtual ~Logic(void);

    int CommandScrollNumberMoved;
    int CommandScrollSpeed;

    int CommandDisplayStartIndex;
    int CommandDisplayEndIndex;

    int CommandSelectedByMouseOld;
    int CommandSelectedByMouse;

    void RunCodeEditor(void);

};

#endif

