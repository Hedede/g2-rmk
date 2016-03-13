zCPar_Stack::zCPar_Stack(size_t Size)
{
	char* mem = new char[Size];
	stacksize = Size;
	stack = mem;
	stackptr = mem;
	stacklast = mem;
}

zCPar_Stack::~zCPar_Stack()
{
	delete[] stack;
	result = 0;
	stack = 0;
	stackptr = 0;
	stacklast = 0;
}

void zCPar_Stack::CheckOverflow(zCPar_Stack *this, int count)
{
	size_t tmp = &stacklast[count - stack];
	if (tmp > stacksize) {
		size = 2 * stacksize;
		newmem = (char *) new char[size];
		memcpy(newmem, stack, stacksize);

		oldmem = stack;
		stack = newmem;
		stackptr += newmem - oldmem;
		stacklast = &stacklast[newmem - oldmem];
		stacksize = size;

		delete[] oldmem;
	}
}

void zCPar_Stack::SetPopPos(int pos)
{
	stackptr = &stack[pos];
}

void zCPar_Stack::Set(int index, int value)
{
	  *(int*)stack[index] = value;
}


int zCPar_Stack::PopByte()
{
	int result;

	result = (*(char *)stackptr & 0xFF);
	stackptr += 1;

	return result;
}

int zCPar_Stack::PopWord()
{
	int result;

	result = (*(short *)stackptr & 0xFFFF);
	stackptr += 1;

	return result;
}

int zCPar_Stack::PopInt()
{
	int result;

	result = *(int *)stackptr;
	stackptr += 4;

	return result;
}

// public
void zCPar_Stack::PopString(zSTRING& outstr)
{
	char length = *stackptr;
	// originally stacklast, but I think that is bullshit
	char* str = stackptr + 1;
	
	outstr = zSTRING(str, length);
	stackptr += length;
}

void zCPar_Stack::PushByte(int8_t byte)
{
	CheckOverflow(1);
	*stacklast++ = byte;
}

void zCPar_Stack::PushWord(int16_t word)
{
	CheckOverflow(2);
	*(int16_t*)stacklast = word;
	stacklast += 2;
}

void zCPar_Stack::PushByte(int32_t Int)
{
	CheckOverflow(4);
	*(int32_t*)stacklast++ = Int;
	stacklast += 4;
}

void zCPar_Stack::Save(zFILE *file)
{
	int size = stacklast - stack;
	file->Write(&size, 4);
	file->Write(stack, size);
}

void zCPar_Stack::PushString(zSTRING& str)
{
	CheckOverflow(str->data.len + 2);

	char length = str->data.len & 0xFF;

	CheckOverflow(1);

	*stacklast++ = length + 1;
	memcpy(stacklast, str.data.ptr, length + 1);

	stacklast += length + 1;
}



void zCPar_Stack::Load(zFILE *file)
{
  int Size;
  file->Read(&Size, 4);

  delete[] stack;
  stack = new int[Size];
  file->Read_a(stack, Size);
  v3->stackptr = stack
  v3->stacksize = Size;
  v3->stacklast = &stack[Size];
}


zCPar_DataStack::zCPar_DataStack()
{
	sptr = 0;
	std::fill_n(&stack, 0x800u, 0);
}

int zCPar_DataStack::Pop()
{
	int result;

	if ( sptr <= 0 )
		return 0;

	return stack[--sptr];
}

float zCPar_DataStack::PopFloat()
{
	if ( sptr <= 0 )
	{
		zSTRING msg = "C: zCPar_DataStack :: PopFloat(): stack underflow - return 0.0f";

		zERROR::Report(zerr, zERR_TYPE_WARNING, 0,
		               &msg, 0, 0, 1541,
			       "P:\dev\g2addon\release\ZenGin\_ulf\zParser_Symbol.cpp",
			       0);
		return 0.0;
	}
	return *(float *)&stack[--sptr];;
}


void zCPar_DataStack::Push(int value)
{
	if ( sptr >= 2048 ) {
		zSTRING msg = "C: zCPar_DataStack :: Push(): stack overflow!!!";

		zERROR::Report(zerr, zERRT_FATAL, 0,
		               &msg, -1, 0, 1513,
			       zMACRO_FILE,
			       0);
		return;
	}

	stack[sptr++] = value;
}

void zCPar_DataStack::Clear()
{
	sptr = 0;
}

