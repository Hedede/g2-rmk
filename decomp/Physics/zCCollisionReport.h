struct zCCollisionReport {
	virtual ~zCCollisionReport();

public:
	int unk[10];
	zCCollisionObject *col1;
	zCCollisionObject *col2;
};
