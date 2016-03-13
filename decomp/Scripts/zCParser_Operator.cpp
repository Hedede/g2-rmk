void zCParser::GetOperatorString(zPAR_TOK opcode, zSTRING& out)
{
  switch ( opcode )
  {
    case zPAR_OP_PLUS:
	    out = " +";
      break;
    case zPAR_OP_MINUS:
      out = " -";
      break;
    case zPAR_OP_MUL:
      out = " *";
      break;
    case zPAR_OP_DIV:
      out = " /";
      break;
    case zPAR_OP_MOD:
      out = " %";
      break;
    case zPAR_OP_OR:
      out = " |";
      break;
    case zPAR_OP_AND:
      out = " &";
      break;
    case zPAR_OP_LOWER:
      out = " <";
      break;
    case zPAR_OP_HIGHER:
      out = " >";
      break;
    case zPAR_OP_LOG_OR:
      out = " ||";
      break;
    case zPAR_OP_LOG_AND:
      out = " &&";
      break;
    case zPAR_OP_SHIFTL:
      out = " <<";
      break;
    case zPAR_OP_SHIFTR:
      out = " >>";
      break;
    case zPAR_OP_LOWER_EQ:
      out = " <=";
      break;
    case zPAR_OP_EQUAL:
      out = " ==";
      break;
    case zPAR_OP_NOTEQUAL:
      out = " !=";
      break;
    case zPAR_OP_HIGHER_EQ:
      out = " >=";
      break;
    case zPAR_OP_IS:
      out = " =";
      break;
    case zPAR_OP_ISPLUS:
      out = " +=";
      break;
    case zPAR_OP_ISMINUS:
      out = " -=";
      break;
    case zPAR_OP_ISMUL:
      out = " *=";
      break;
    case zPAR_OP_ISDIV:
      out = " /=";
      break;
    case zPAR_OP_UN_PLUS:
      out = " +";
      break;
    case zPAR_OP_UN_MINUS:
      out = " -";
      break;
    case zPAR_OP_UN_NOT:
      out = " !";
      break;
    case zPAR_OP_UN_NEG:
      out = " ~";
      break;
    default:
      out = "??";
      break;
  }
}
