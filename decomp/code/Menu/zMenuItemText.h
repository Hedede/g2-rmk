//#################################################################
//
//  Vermutlich ziemlich nutzlos, ich dachte zunächst die Klasse
//  wäre wichtiger. Alles entscheidende spielt sich
//  zumindest was das Charaktermenü angeht in den gewöhnlichen
//  zCMenuItems ab. zCMenuItemText wird (nicht ausschließlich)
//  für Auswahlboxen benutzt (In den Einstellungen: [ja|nein]-Box)
//
//#################################################################

struct zCMenuItemText : zCMenuItem {
	zSTRING GetText(int index) const
	{
		if ( index < listLines.NumInList() )
			return listLines[index];
		return zSTRING();
	}

	enum zTMenuItemTextMode {
		MODE_SIMPLE,
		MODE_ENUM,
		MODE_MULTILINE
	};

private:
	int mode; //siehe enum

	zSTRING fullText;
	//Relevant für Options-Menüitems wo man zum Beispiel
	//zwischen "aus" und "an" wählen kann.
	int	numOptions;

	int topLine;
	int viewLines;
	int numLines;
	zBOOL unformated;
};
