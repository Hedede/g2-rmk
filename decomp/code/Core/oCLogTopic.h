struct oCLogEntry {
	~oCLogEntry() = default;

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

struct oCLogTopic {
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
	static oCLogManager& GetLogManager()
	{
		static oCLogManager logMan;
		return logMan;
	}

	oCLogManager() = default;
	oCLogManager(oCLogManager const&) {}
	oCLogManager& operator=(oCLogManager const&) {}
	~oCLogManager() { Clear(); }

	void Archive(zCArchiver& arc);
	void Unarchive(zCArchiver& arc);

	void Clear() { topics.DeleteListDatas(); }

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
