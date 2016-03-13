int zCParser::ParseFile(zSTRING& fileName)
{
	if (files.size() > 0) {
		do {
			tmp = files[ebx].GetName();

			if (fileName == tmo) {
				zSTRING msg = "U:PAR: Ignoring " + fileName +
				              " ( File already parsed )";
				zERROR::Report(zerr, zERR_TYPE_WARNING, 0,
				               msg, 0, 0, 784, zMACRO_FILE, 0);

				return 0;
			}
		} while (++ebx < files.size);

mov	cl, [esp+0B8h+var_A1]
mov	[esp+0B8h+var_98], cl
push	0
lea	ecx, [esp+0BCh+var_98]



fname = fileName;
labelpos = 0


// = new int[48];
auto* var_A0 = new zCPar_TreeNode();

tree = var_A0;
treenode = var_A0;

var_A0->(+14) = 
[eax+14h], 5Ah

eax = new zCPar_TreeNode();

mov	[esp+0B8h+var_A0], edi
ebx = 0

loc_78F839:
mov	[esp+0B8h+Memory], edi
lea	edx, [esp+0B8h+Memory]
lea	edi, [esi+4]
push	edx
mov	ecx, edi
files.InsertEnd(edx);

mov	eax, array.size() - 1;
mov	ecx, [esp+0B8h+Memory]
mov	*[ecx], eax

mov	eax, [esi+2098h]
mov	edx, [esp+0B8h+Memory]
mov	*[edx+24h] = eax
mov	ecx, [esp+0B8h+Memory] ; this

lea	eax, [esp+0B8h+var_44]
push	eax		; struct zDATE *

lea	ebx, [esi+10h]
push	ebx		; struct zCPar_SymbolTable *
push	ebp		; struct zSTRING *
mov	[esp+0C4h+var_74], ebx
Memory->Load(fileName, symtab, zDATE);

if (eax == -1) {
push	ebp
lea	ecx, [esp+0BCh+var_88]
msg =  "File	" + fileName +  " not found.";
zCParser::Error(msg, 0)
mov	ebp, offset ??_7zSTRING@@6B@ ; const zSTRING::`vftable'
push	1
lea	ecx, [esp+0BCh+var_6C]
mov	byte ptr [esp+0BCh+var_4], bl
mov	[esp+0BCh+var_70], ebp
call	?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEX_N@Z ; std::basic_string<char,std::char_traits<char>,std::allocator<char>>::_Tidy(bool)
push	1
lea	ecx, [esp+0BCh+var_84]
mov	byte ptr [esp+0BCh+var_4], 3
mov	[esp+0BCh+var_88], ebp
call	?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEX_N@Z ; std::basic_string<char,std::char_traits<char>,std::allocator<char>>::_Tidy(bool)
mov	eax, [edi+8]
test	eax, eax
jz	short loc_78F92C
xor	edx, edx
test	eax, eax
jle	short loc_78F92C
mov	ebx, [edi]
mov	ecx, [esp+0B8h+Memory]
mov	esi, ebx

loc_78F910:
cmp	[esi], ecx
jz	short loc_78F91E
inc	edx
add	esi, 4
cmp	edx, eax
jl	short loc_78F910
jmp	short loc_78F930

loc_78F91E:
dec	eax
cmp	edx, eax
mov	[edi+8], eax
jz	short loc_78F930
mov	eax, [ebx+eax*4]
mov	[ebx+edx*4], eax

loc_78F92C:		; this
mov	ecx, [esp+0B8h+Memory]

loc_78F930:
test	ecx, ecx
mov	esi, ecx
jz	short loc_78F944
call	??1zCPar_File@@QAE@XZ ;	zCPar_File::~zCPar_File(void)
push	esi		; Memory
call	??3@YAXPAX@Z	; operator delete(void *)
add	esp, 4

loc_78F944:
push	1
lea	ecx, [esp+0BCh+var_98]
mov	[esp+0BCh+var_4], 0FFFFFFFFh
mov	[esp+0BCh+var_9C], ebp
call	?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEX_N@Z ; std::basic_string<char,std::char_traits<char>,std::allocator<char>>::_Tidy(bool)
return -1;
}

loc_78F966:
test	eax, eax
jnz	loc_78FD1F
push	offset ??_C@_0M@HBJB@?5?3?5Parse?4?4?4?$AA@ ;
msg = fileName + "	: Parse...";
lea	ecx, [esp+0BCh+var_88]
push	ebp
push	ecx
mov	dword ptr [esi+2094h], 1
call	??H@YA?AVzSTRING@@ABV0@PBD@Z ; operator+(zSTRING const &,char const *)
add	esp, 0Ch
mov	edi, eax
push	offset ??_C@_07LPJH@U?3PAR?3?5?$AA@ ; 
lea	ecx, [esp+0BCh+var_70]
mov	byte ptr [esp+0BCh+var_4], 8
call	??0zSTRING@@QAE@PBD@Z ;	zSTRING::zSTRING(char const *)
cmp	dword ptr [esi], 0
mov	byte ptr [esp+0B8h+var_4], 9
if (msgfunc)
	msgfunc(msg);
jz	short loc_78F9C4
sub	esp, 14h
mov	ecx, esp
mov	[esp+0CCh+var_A0], esp
push	edi
call	??0zSTRING@@QAE@ABV0@@Z	; zSTRING::zSTRING(zSTRING const &)
call	dword ptr [esi]
add	esp, 14h

loc_78F9C4:
push	edi
lea	edx, [esp+0BCh+var_70]
push	edx
lea	eax, [esp+0C0h+var_58]
push	eax
msg = "U:PAR: " + msg;
call	??H@YA?AVzSTRING@@ABV0@0@Z ; operator+(zSTRING const &,zSTRING const &)
add	esp, 0Ch
push	eax
mov	ecx, offset ?zerr@@3VzERROR@@A ; zERROR	zerr
mov	byte ptr [esp+0BCh+var_4], 0Ah
 zERROR::Message(msg);

var_A0 = symtab.GetNumInList();
var_5c = symtab.GetLastSymbol();
mov	eax, [esp+0B8h+Memory]

linec = 1;
line_start = 0;
mov	ecx, [eax+18h]
pc = ecx
pc_start ecx
mov	ecx, [eax+1Ch]
mov	edx, [eax+18h]
lea	eax, [ecx+edx-1]
pc_stop = eax
mov	bl, 0Bh

loc_78FA67:
ReadWord(tmp);

if (tmp == "CONST")
DeclareVar(1);
else if (tmp == "VAR")
DeclareVar(0);
else if (tmp == "FUNC")
DeclareFunc(void);
else if (tmp == "CLASS")
DeclareClass(void);
else if (tmp == "INSTANCE")
DeclareInstance(void);
else if (tmp == "PROTOTYPE")
DeclarePrototype(void);
else {
	if (tmp == "")
		break;

msg = ParErr_SyntaxError + tmp;

Error(msg, 0)
}



loc_78FC18:
lea	edx, [esp+0B8h+var_A1]
push	edx
push	offset ??_C@_01FAJB@?$DL?$AA@ ;	";"
lea	ecx, [esp+0C0h+var_84]
mov	[esp+0B8h+var_88], ebp
lea	eax, [esp+0B8h+var_88]
push	eax		; struct zSTRING *

mov	ecx, esi	; this
mov	byte ptr [esp+0BCh+var_4], 0Ch
Match(";");

push	1
lea	ecx, [esp+0BCh+var_84]
mov	byte ptr [esp+0BCh+var_4], 3
mov	[esp+0BCh+var_88], ebp
while (pc < pc_stop);

loc_78FC6C:
mov	edx, [esp+0B8h+Memory]
mov	eax, [edx+18h]
lea	edi, [edx+18h]
push	eax		; Memory
call	??_V@YAXPAX@Z	; operator delete[](void *)

mov	dword ptr [edi], 0
mov	eax, [esi+21A0h]
mov	ecx, [esp+0BCh+Memory]
mov	[ecx+20h], eax
mov	edx, [esp+0BCh+Memory]
mov	edi, [edx+28h]
mov	ebx, [esi+218Ch]
lea	ebp, [edx+28h]
add	esp, 4
test	edi, edi
jz	short loc_78FCB6
mov	ecx, edi	; this
call	??1zCPar_TreeNode@@QAE@XZ ; zCPar_TreeNode::~zCPar_TreeNode(void)
push	edi		; Memory
call	??3@YAXPAX@Z	; operator delete(void *)
add	esp, 4

loc_78FCB6:
mov	[ebp+0], ebx
mov	eax, [esi+208Ch]
test	eax, eax
if (treesave) {
mov	ecx, [esp+0B8h+var_5C] ; this
	if (ecx) {
lea	eax, [esp+0B8h+var_44]
push	eax		; struct zDATE *
call	?GetNext@zCPar_Symbol@@QAEPAV1@XZ ; zCPar_Symbol::GetNext(void)
mov	ecx, [esp+0BCh+var_74] ; this
push	eax		; struct zCPar_Symbol *
call	?GetNumInList@zCPar_SymbolTable@@QAEHXZ	; zCPar_SymbolTable::GetNumInList(void)
mov	edx, [esp+0C0h+var_A0]
mov	ecx, [esp+0C0h+Memory] ; this
sub	eax, edx
push	eax		; int
call	?SaveTree@zCPar_File@@QAEXHPAVzCPar_Symbol@@AAVzDATE@@@Z ; zCPar_File::SaveTree(int,zCPar_Symbol *,zDATE &)
	} else {
mov	esi, [esp+0B8h+var_74]
lea	ecx, [esp+0B8h+var_44]
push	ecx		; struct zDATE *
mov	ecx, esi	; this
call	?GetFirstSymbol@zCPar_SymbolTable@@QAEPAVzCPar_Symbol@@XZ ; zCPar_SymbolTable::GetFirstSymbol(void)
push	eax		; struct zCPar_Symbol *
mov	ecx, esi	; this
call	?GetNumInList@zCPar_SymbolTable@@QAEHXZ	; zCPar_SymbolTable::GetNumInList(void)
mov	ecx, [esp+0C0h+Memory] ; this
push	eax		; int
call	?SaveTree@zCPar_File@@QAEXHPAVzCPar_Symbol@@AAVzDATE@@@Z ; zCPar_File::SaveTree(int,zCPar_Symbol *,zDATE &)
	}
}
return 0;

loc_78FD1F:		
msg = fileName + " : Load..."

if (msgfunc)
	msgfunc(msg);

msg = "U:PAR: " + msg;

zERROR::Message(msg);

//feoc
return;
}
