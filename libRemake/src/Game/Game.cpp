#include <Gothic/Game/zConsole.h>
#include <Utility/String.h>
#include <Gothic/Script/zParser.h>
#include <aw/utility/string/trim.h>
#include <aw/utility/string/split.h>
#include <aw/utility/string/case.h>
#include <aw/utility/to_string.h>


std::string handle_set_var(std::vector<string_view> const& args)
{
	using aw::to_string;


	if (args.size() < 1)
	{
		return "Specify variable name";
	}
	if (args.size() < 2)
	{
		return "Specify variable value";
	}

	auto varname = std::string(args[0]);
	auto value   = std::string(args[1]);

	std::string result;

	auto sym = zparser.GetSymbol(varname);
	if (!sym)
	{
		result += "Symbol ";
		result += varname;
		result += " not found";
	}
	else if (sym->type == zPAR_TYPE_VOID)
	{
		result += varname;
		result += " is of type void, can't set value";
	}
	else if (sym->type == zPAR_TYPE_FUNC)
	{
		result += varname;
		result += " is a func, can't set value";
	}
	else if (sym->type == zPAR_TYPE_CLASS)
	{
		result += varname;
		result += " is a class, can't set value";
	}
	else if (sym->type == zPAR_TYPE_PROTOTYPE)
	{
		result += varname;
		result += " is a prototype, can't set value";
	}
	else if (sym->type == zPAR_TYPE_INSTANCE)
	{
		result += varname;
		result += " is an instance, can't set value";
	}
	else if (sym->type == zPAR_TYPE_INT)
	{
		int i = 0;
		try {
			i = std::stoi(value);
		} catch(...) {}

		sym->SetValue(i);

		result += varname;
		result += " set to ";
		result += to_string(i);
	}
	else if (sym->type == zPAR_TYPE_FLOAT)
	{
		float f = 0;
		try {
			f = std::stof(value);
		} catch(...) {}

		sym->SetValue(f);

		result += varname;
		result += " set to ";
		result += to_string(f);
	}
	else if (sym->type == zPAR_TYPE_STRING)
	{
		sym->SetValue(value);

		result += varname;
		result += " set to ";
		result += value;
	}

	return result;
}

std::string handle_get_var(std::vector<string_view> const& args)
{
	using aw::to_string;


	if (args.size() < 1)
	{
		return "Specify variable name";
	}
	/*if (args.size() < 2)
	  {
	  msg = "Specify variable value";
	  return true;
	  }*/

	auto varname = std::string(args[0]);

	std::string result;

	auto sym = zparser.GetSymbol(varname);
	if (!sym)
	{
		result += "Symbol ";
		result += varname;
		result += " not found";
	}
	else if (sym->type == zPAR_TYPE_VOID)
	{
		result += varname;
		result += ": void";
	}
	else if (sym->type == zPAR_TYPE_INT)
	{
		result += varname;
		result += ": int";
		if (sym->ele > 1)
		{
			result += '[';
			result += to_string(sym->ele);
			result += ']';
		}
		else
		{
			result += " = ";
			result += to_string(sym->GetValue<int>());
		}
	}
	else if (sym->type == zPAR_TYPE_FLOAT)
	{
		result += varname;
		result += ": float";
		if (sym->ele > 1)
		{
			result += '[';
			result += to_string(sym->ele);
			result += ']';
		}
		else
		{
			result += " = ";
			result += to_string(sym->GetValue<float>());
		}
	}
	else if (sym->type == zPAR_TYPE_STRING)
	{
		result += varname;
		result += ": string";
		if (sym->ele > 1)
		{
			result += '[';
			result += to_string(sym->ele);
			result += ']';
		}
		else
		{
			result += " = ";
			result += to_string(sym->GetValue<std::string>());
		}
	}
	else if (sym->type == zPAR_TYPE_FUNC)
	{
		result += varname;
		result += ": func";
	}
	else if (sym->type == zPAR_TYPE_CLASS)
	{
		result += varname;
		result += ": class";
	}
	else if (sym->type == zPAR_TYPE_PROTOTYPE)
	{
		result += varname;
		result += ": prototype";
	}
	else if (sym->type == zPAR_TYPE_INSTANCE)
	{
		result += varname;
		result += ": instance";
	}

	return result;
}

int Hedede_ConsoleEvalFunc(zSTRING const& input, zSTRING& msg)
{
	string_view space = " ";

	std::string str = std::string(input);
	aw::string::tolower(str);

	string_view cmd = str;
	cmd = aw::string::trim(cmd, space);

	string_view prefix = "get var";
	if (has_prefix(cmd, prefix))
	{
		cmd.remove_prefix(prefix.size());

		auto args = aw::string::split_by(cmd, space);

		msg = handle_get_var(args);
		return true;
	}

	prefix = "set var";
	if (has_prefix(cmd, prefix))
	{
		cmd.remove_prefix(prefix.size());

		auto args = aw::string::split_by(cmd, space);

		msg = handle_set_var(args);
		return true;
	}

	return false;
}

void Game_InitConsole()
{
	auto game_con = Value<int>(0xAB0894);
	auto oCGame_ConsoleEvalFunc = reinterpret_cast<zCConsole::EvalFunc*>(0x6CC0A0);

	zcon.AddEvalFunc(Hedede_ConsoleEvalFunc);
	zcon.Register("get var", "get a symbol value [name]");
	zcon.Register("set var", "set a symbol value [name value]");

	zcon.AddEvalFunc(oCGame_ConsoleEvalFunc);
	zcon.Register("insert", "insert a script-instance [name]");
	zcon.Register("goto waypoint", "beam player to waypoint [name]");
	zcon.Register("toggle desktop", "desktop on/off");
	zcon.Register("toggle debug", "debug-Messages on/off");
	zcon.Register("toggle spy", "spy-Messages on/off");
	zcon.Register("toggle text", "text window on/off");
	zcon.Register("toggle waynet", "waynet on/off");
	zcon.Register("toggle frame", "frame-info on/off");
	zcon.Register("toggle screen", "fullscreen / window");
	zcon.Register("toggle aniinfo", "animation-info on/off");
	zcon.Register("toggle camera", "change camera mode on/off");
	zcon.Register("toggle turns", "turn player on/off");
	zcon.Register("toggle time", "show worldtime on/off");
	zcon.Register("toggle wayboxes", "wayboxes on/off");
	zcon.Register("toggle freepoints", "freepoints on/off");
	zcon.Register("toggle musiccontrol", "game music logic on/off");
	zcon.Register("edit combat", "fight console on/off");
	zcon.Register("edit abilities", "edit player-instance");
	zcon.Register("edit species", "edit species values");
	zcon.Register("edit focus", "edit focus object");
	zcon.Register("edit ai", "edit ai parameters");
	zcon.Register("set throw", "set throw range");
	zcon.Register("camera mode", "set camera mode [name]");
	zcon.Register("camera autoswitch", "toggle camera autoswitch");
	zcon.Register("cheat full", "heal player");
	zcon.Register("cheat god", "godmode on/off");
	zcon.Register("save position", "save player position");
	zcon.Register("save zen", "save level [name.ZEN]");
	zcon.Register("load position", "restore player position");
	zcon.Register("lc1", "Include LevelChanger to net/netlevel1.zen.");
	zcon.Register("lc2", "Include LevelChanger to net/netlevel2.zen.");
	zcon.Register("play ani", "play animation [name]");
	zcon.Register("play sound", "player sound instance [name]");
	zcon.Register("kill", "kill focus npc");
	zcon.Register("set time", "set worldtime [hh mm]");
	zcon.Register("aigoto", "player goes to waypoint [name]");
	zcon.Register("autocomplement", "autocomplement on/off");
	zcon.Register("witness attack", "call reaction attack");
	zcon.Register("witness aim", "call reaction aim");
	zcon.Register("witness steal", "call reaction steal");
	zcon.Register("parserd on", "show parser debug info");
	zcon.Register("parserd off", "hide parser debug info");
	zcon.Register("parserd merge", "merge 'SRC' oder 'D'-File");
	zcon.Register("parserd pcode", "print pcode to spy.");
	zcon.Register("set fbbox", "show boundingbox of focus object");
	zcon.Register("apply overlaymds", "enable overlay-MDS [mds-name]");
	zcon.Register("remove overlaymds", "remove overlay-MDS [mds-name]");
	zcon.Register("apply randomani", "apply a random-ani [ani-name, random-ani-name]");
	zcon.Register("apply anifreq", "set random-ani frequency [ani-name, frequency]");
	zcon.Register("edit particlefx", "edit particle-effect-instance [name]");
	zcon.Register("start particlefx", "start particle-effect-instance [name]");
	zcon.Register("create particlefx", "create new particle-effect-instance [name]");
	zcon.Register("delete particlefx", "delete last used particle-effect-instance");
	zcon.Register("play faceani", "play a face-ani [name]");
	zcon.Register("save game", "save game in slot 0");
	zcon.Register("load game", "load game from Slot 0");
	zcon.Register("set modelfatness", "set model fatness [value]");
	zcon.Register("set debugfocus", "set debug focus object");
	zcon.Register("clear debugfocus", "clear debug focus object");
	zcon.Register("toggle debugfocus", "toggle debug focus object");
	zcon.Register("set alldebuginstances", "set all debug instances");
	zcon.Register("clear alldebuginstances", "clear all debug instances");
	zcon.Register("set debugchannel", "set debug channel [ch]");
	zcon.Register("clear debugchannel", "clear debug channel [ch]");
	zcon.Register("toggle debugchannel", "toggle debug channel [ch]");
	zcon.Register("set alldebugchannels", "set all debug channels");
	zcon.Register("clear alldebugchannels", "clear all debug channels");
	zcon.Register("toggle alldebugchannels", "toggle all debug channels");
	zcon.Register("set permattitude", "set permanent attitude [attitude]");
	zcon.Register("set tempattitude", "set temporary attitude [attitude]");
	zcon.Register("set trueguild", "set true guild [guild]");
	zcon.Register("set guild", "set guild [guild]");
	zcon.Register("goto vob", "beam player to vob [name] ([num])");
	zcon.Register("print vob", "print info for vob(s) [name]");
	zcon.Register("version", "print version info");
	
	game_con = 1;
}
