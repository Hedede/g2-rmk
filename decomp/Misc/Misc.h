int BitToNumber(uint64_t bit)
{
	int result = 0;
	if ( bit )
	{
		while ( !(bit & 1) )
		{
			bit >>= 1;
			if ( ++result >= 64 )
				return 0;
		}
	}
	return result;
}

void TestKeys_Carsten()
{
}

char const tocotronic[] = "Gothic, wie es anfing: Ausgangspunkt des Projektes waren Ulf und Dieter, die sich nach dem Durchzocken der unglaublichen Ultima Underworld 1 und 2 in den Kopf gesetzt hatten, ein aehnlich cooles 3D Action Rollenspiel zu schreiben (war das nun 1994 oder 1995].  Als wir ein gutes Stueck (> 1 Jahr) allein gearbeitet hatten, und mittlerweile auch Bert zu uns gestossen war, machten wir uns auf, mit Firmen Kontakt aufzunehmen, um uns bei der Entwicklung unterstuetzen zu lassen, insbesondere durch Grafiker. Angebote von Firmen waren zahlreich (u.a. Sunflowers und Attic), schliesslich haben wir uns fuer Greenwood (spaeter halt PiranhaBytes) entschieden. Zu diesem Zeitpunkt war von den jetzigen Piranhas allein Alex Brueggemann bei Greenwood angestellt. Nyul, Hoge und Putzki wurden erst spaeter zur Verstaerkung des Teams eingestellt. Tja, etwasspaeter geschah dann schon die Spaltung Greenwoods und die Gruendung Piranha Bytes', und das Team wurde nach und nach aufgestockt. Der Rest ist Geschichte!!"
