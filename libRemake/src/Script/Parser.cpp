#include <Gothic/Script/zParser.h>
#include <Logging/Log.h>

#include <Hook/size_checker.h>
#include <aw/utility/to_string.h>

namespace {
	size_checker<zCParser, 0x21C4>  sc_zparsize;
	size_checker<zCPar_Symbol, 0x3C>  sc_zsymsize;
}

std::string zCPar_Symbol::DebugPrintValue(int index) const
{
	switch (type) {
		case zPAR_TYPE_FLOAT:
			return aw::to_string(GetValue<float>(index));

		case zPAR_TYPE_INT:
			return aw::to_string(GetValue<int>(index));

		case zPAR_TYPE_STRING:
		{
			std::string result{ '"' };
			result += aw::to_string(GetValue<std::string>(index));
			result += '"';
			return result;
		}
	}
	return {};
}

std::string zCPar_Symbol::DebugPrint() const
{
	std::string result;

	if (ele > 1) result += "( ";

	result += DebugPrintValue(0);

	for (int i = 1; i < ele; ++i)
	{
		result += ", ";
		result += DebugPrintValue(i);
	}

	if (ele > 1) result += " )";

	return result;
}


void zCParser::DefineExternal(std::string const& name, int (*func)(), zPAR_TYPE ret, std::vector<zPAR_TYPE> args)
{
	g2::Log("Parser", "Adding External ", name);

	auto funcsym = new zCPar_Symbol;
	funcsym->SetName(name);
	funcsym->SetType(zPAR_TYPE_FUNC);
	funcsym->SetFlag(zPAR_FLAG_EXTERNAL|zPAR_FLAG_CONST);
	if ( ret != zPAR_TYPE_VOID )
		funcsym->SetFlag(zPAR_FLAG_RETURN);
	funcsym->SetOffset(ret);

	g2::Log("Parser", "Adding Symbol ", funcsym->GetName());
	if ( !symtab.Insert(funcsym) ) {
		if ( !compiled ) {
			//TODO: Error();
			g2::Warning("Parser", "Redefined External ", name);
			return;
		}

		auto name = funcsym->GetName();
		delete funcsym;
		funcsym = GetSymbol(name);
	}

	funcsym->SetStackPos((int)func);
	if (compiled)
		return;

	for (const auto arg : args) {
		auto argsym = new zCPar_Symbol();
		
		auto id = funcsym->ele++;
		argsym->name = funcsym->name + ".PAR" + id;
		argsym->SetType(arg);
		argsym->SetParent(nullptr);

		symtab.Insert(argsym);
	}
}


