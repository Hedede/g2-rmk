struct zCFreeBlock {
	zCFreeBlock* next;
	void* dunno;
};

class zCMemPoolBase {
	static void SetMemDebug(char* className, char* fileName, int line)
	{
		s_className = className;
		s_fileName = fileName;
		s_line = line;
	}

	static void DisablePools(zBOOL b)
	{
		s_disablePools = b;
	}

protected:
	static zBOOL s_disablePools = 1;
	static char* s_className = 0;
	static char* s_fileName = 0;
	static int s_line = 0;
}
