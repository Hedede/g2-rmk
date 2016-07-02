

void zCParser::DeclareVar(int isConst)
{
ReadVar:
	zPAR_TYPE type = ReadVarType();
	zCPar_Symbol* typesym;
	if ( type >= 7 ) {
		typesym = symtab.GetSymbol(type - 7);
		type = 7;
	}

	while ( 1 ) {
		int line = linec;
		int pos = line_start;
		zSTRING name;
		ReadWord(name);
		ReadWord(this->aword);

		int ele = 1;
		if (aword == "[") {
			ele = ReadInt();
			assert(ele <= 0x1000);

			Match("]");
		} else {
			pc    = prevword_index[prevword_nr];
			linec = prevline_index[prevword_nr];
			prevword_nr = (prevword_nr - 1) & 0xF;
		}

		zCPar_Symbol* newsym = new zCPar_Symbol(name)

		newsym->bitfield = (type << 12) & 0xF000 + size & 0xFFF; // todo
		newsym->SetParent(typesym);

		if ( isConst )
			newsym->SetFlag(1);

		if ( in_func ) {
			zSTRING name = in_func->name + "." + newsym->name;
			newsym->SetName(name);

			if ( !this->params )
				goto PushSym;

			assert(in_func->ele < 0x1000);
			in_func->bitfield.ele++;

			int type = newsym->bitfield.type;
			if (type == 1) {
				node = new zCPar_TreeNode(zPAR_TOK_VAR, name, 0);
				node->stack_index = 1;
				node->label_index = 0;

				node2 = CreateLeaf(zPAR_TOK_ASSIGNFLOAT, node);
				node2->SetNext(treenode->next);

				treenode->next = node;

				if ( (in_func->bitfield & 0xFFF) == 1 )
					this->treenode = node2;
			} else if ( type == 2 ) {
				auto* node = new zCPar_TreeNode(zPAR_TOK_VAR, name, 2);
				// for some reason stack_index=2
				// not label_index like in ctor
				
				node2 = CreateLeaf(zPAR_OP_IS, node);
				node2->SetNext(treenode->next);

				treenode->next = node;

				if ( (this->in_func->bitfield & 0xFFF) == 1 )
					this->treenode = node2;
			} else if ( type == 3 ) {
				auto* node = new zCPar_TreeNode(zPAR_TOK_VAR, name, 3);
				node2 = CreateLeaf(zPAR_TOK_ASSIGNSTR, node);
				node2->SetNext(treenode->next);

				treenode->next = node;

				if ( (in_func->bitfield & 0xFFF) == 1 )
					treenode = node2;
			} else if ( type == 7 ) {
				auto* node = new zCPar_TreeNode(zPAR_PUSHINST, name, 0);
				node->stack_index = 7;
				node->label_index = symindex;

				node2 = CreateLeaf(zPAR_TOK_ASSIGNINST, node);
				node2->SetNext(treenode->next);

				treenode->next = node;

				if ( (in_func->bitfield & 0xFFF) == 1 )
					treenode = node2;
			}
			else if (type == 5)
			{
				auto* node = new zCPar_TreeNode(zPAR_TOK_VAR, name, 0);
				node->stack_index = 5;
				node2 = CreateLeaf(zPAR_TOK_ASSIGNFUNC, node);
				node2->SetNext(treenode->next);

				treenode->next = node;
				if ( (this->in_func->bitfield & 0xFFF) == 1 )
					this->treenode = node2;
			} else {
				Error("Parameters of this type are not supported.", 0);
			}
		} else if ( in_class ) {
			newsym->SetName(in_class->name + "." + newsym->name);
			if ( this->params ) {
				in_class->ele++;
				newsym->SetParent(in_class);
				newsym->SetFlag(4);
			}
		}
PushSym:
		if ( !symtab.Insert(newsym) )
			Error(ParErr_Redefined + newsym->name, 0);

		if ( isConst ) {
			Match("=");
			if (size > 1)
				Match("{");
	
			for (int i = 0; i < size; ++i) {
				switch ( type ) {
				case 1:
					value = ReadFloat();
					newsym->SetValue(value, i);
					break;
				case 2:
					tok_ptr = GetNextToken();
					auto* node3 = Parse_Expression(&tok_ptr, -1);
					auto* node4 = EvalLeaf(node3, 0);
					newsym->SetValue(node4, i);

					pc = this->prevword_index[prevword_nr];
					linec = this->prevline_index[prevword_nr];
					prevword_nr = (prevword_nr - 1) & 0xF;
					break;
				case 3:
					ReadString(&str);
					newsym->SetValue(&str, i);
					break;
				case 5:
					ReadWord(this->aword);
					auto* sym = GetIndex(this->aword);
					if ( sym < 0 )
						Error(ParErr_UnknownVar + aword, 0);

					newsym->bitfield = newsym->bitfield & 0xFFFF2FFF | 0x2000;
					newsym->SetValue(sym, i);
					newsym->bitfield = newsym->bitfield & 0xFFFF5FFF | 0x2000;
					break;
				default:
					break;
				}
				if ( i != size )
					Match(",");
			}

			if ( i > 1 )
				Match("}");

			if ( type == 5 )
				newsym->bitfield = newsym->bitfield & 0xFFFF2FFF | 0x2000;
		}
		newsym->SetLineData(line,
		                    linec - line + 1,
		                    pos,
		                    pc - pc_start - pos + 3);

		newsym->SetFileNr(files.numInArray - 1);

		ReadWord(this->aword);
		if ( aword != "," )
			break;

		if ( in_func ) {
			Match("VAR");
			goto ReadVar;
		}
	}

	PrevWord();
}
