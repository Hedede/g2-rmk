class oCCSProps : public zCCSProps {
	Z_OBJECT(oCCSProps);
private:
	virtual ~oCCSProps();
	virtual void AcceptRole(zCCSRole *,zCVob *);
	virtual void CheckRole(zCCSRole *,zCVob *);
	virtual void CheckRoleResult();
};
