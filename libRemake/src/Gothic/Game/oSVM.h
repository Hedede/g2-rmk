#ifndef GOTHIC_SVM_H
#define GOTHIC_SVM_H
struct oCSVM;
struct oCSVMManager {
	oCSVMManager()
	{
		Thiscall<void(oCSVMManager*)> ctor{0x779B70};
		ctor(this);
	}

	int svmCount;
	oCSVM *modules;
};


#endif//GOTHIC_SVM_H
