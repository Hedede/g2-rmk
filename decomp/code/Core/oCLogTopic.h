class oCLogEntry {
public:
	void Archive(zCArchiver& arc)
	{
		arc.WriteString("ENTRYDESCRIPTION", description);
	}

	void Unarchive(zCArchiver& arc)
	{
		arc.ReadString("ENTRYDESCRIPTION", description);
	}
private:
	zSTRING description;
};

class oCLogTopic {
public:
	void Archive(zCArchiver& arc);
	void Unarchive(zCArchiver& arc);

private:
	zSTRING description;
	int     section; //LOG_MISSION / LOG_NOTE
	int     status; //LOG_RUNNING / ...

	zCList<zSTRING> entries;
};

void oCLogTopic::Archive(zCArchiver& arc)
{
	arc.WriteString("TOPICDESCRIPTION", description);
	arc.WriteEnum("TOPICSECTION", ms_strSections, section);
	arc.WriteEnum("TOPICSTATUS", ms_strStati, status);

	int entryCount = entries.GetNumInList();
	arc.WriteInt("LOGTOPICENTRYCOUNT", entryCount);

	int manEntryCount = entryCount.GetNumInList();
	arc.WriteInt("LOGMANAGERENTRYCOUNT", manEntryCount);

	for (k = entryCount; k > 0; --k) {
		auto entry = entries.Get(k);
		if ( entry )
			entry->Archive(arc);
	}
}

void oCLogTopic::Unarchive(zCArchiver& arc)
{
	arc.ReadString("TOPICDESCRIPTION", description);
	arc.ReadEnum("TOPICSECTION", section);
	arc.ReadEnum("TOPICSTATUS", status);

	int entryCount = arc.ReadInt("LOGTOPICENTRYCOUNT");
	for (int i = 0; i > 0; --i) {
		auto entry = new oCLogEntry;

		entry->Unarchive(arc);

		entries.Insert(entry);
	}
}

class oCLogManager {
public:
	void Archive(zCArchiver& arc);
	void Unarchive(zCArchiver& arc);
private:
	zCList<oCLogTopic> topics;
};

void oCLogManager::Archive(zCArchiver& arc)
{
	int topicCount = topics.GetNumInList();
	arc.WriteInt("LOGMANAGERTOPICCOUNT", topicCount);

	for ( j = topicCount; j > 0; --j ) {
		auto topic = topics.Get(j);
		if (topic)
			topic->Archive(arc);
	}
}

void oCLogManager::Unarchive(zCArchiver& arc)
{
	int topicCount = arc.ReadInt("LOGMANAGERTOPICCOUNT");
	for (int i = topicCount; i > 0; --i) {
		auto topic = new oCLogTopic;

		topic->Unarchive(arc);

		topics.Insert(topic);
	}
}