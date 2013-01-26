#pragma once

#include "krt.h"

class KMachine : public KVM
{
private:
	TypeDef *allTypes;
	ModuleDef *allModules;
	ClassDef *allClasses;
	DelegateDef *allDelegates;
	FieldDef *allFields;
	MethodDef *allMethods;
	ParamDef *allParams;
	ParamDef *allDelegateParams;
	TypeDef *allLocals;

public:
	KMachine();
	~KMachine();

	//===========================================
	// Implement KVM
	//===========================================

	// conversions
	void convBool();
	void convInt();
	void convFloat();
	void convString();

	void box();
	void unbox(const ktoken_t toType);
	void cast(const ktoken_t toType);

	// gets data from storage units
	kbool_t getBool_loc(const ktoken_t token);
	kbool_t getBool_arg(const ktoken_t token);
	kbool_t getBool_argind(const ktoken_t token);
	kbool_t getBool_fld(const ktoken_t token);
	kbool_t getBool_sfld(const ktoken_t token);
	kbool_t getBool_elem(const kint_t index);
	
	kint_t getInt_loc(const ktoken_t token);
	kint_t getInt_argind(const ktoken_t token);
	kint_t getInt_arg(const ktoken_t token);
	kint_t getInt_fld(const ktoken_t token);
	kint_t getInt_sfld(const ktoken_t token);
	kint_t getInt_elem(const kint_t index);

	const kfloat_t & getFloat_loc(const ktoken_t token);
	const kfloat_t & getFloat_arg(const ktoken_t token);
	const kfloat_t & getFloat_argind(const ktoken_t token);
	const kfloat_t & getFloat_fld(const ktoken_t token);
	const kfloat_t & getFloat_sfld(const ktoken_t token);
	const kfloat_t & getFloat_elem(const kint_t index);

	kcstring_t getString_loc(const ktoken_t token);
	kcstring_t getString_arg(const ktoken_t token);
	kcstring_t getString_argind(const ktoken_t token);
	kcstring_t getString_fld(const ktoken_t token);
	kcstring_t getString_sfld(const ktoken_t token);

	// changes data in storage units
	void setBool_loc(const ktoken_t token, const kbool_t value);
	void setBool_arg(const ktoken_t token, const kbool_t value);
	void setBool_argind(const ktoken_t token, const kbool_t value);
	void setBool_fld(const ktoken_t token, const kbool_t value);
	void setBool_sfld(const ktoken_t token, const kbool_t value);
	void setBool_elem(const kint_t index, const kbool_t value);

	void setInt_loc(const ktoken_t token, const kint_t value);
	void setInt_arg(const ktoken_t token, const kint_t value);
	void setInt_argind(const ktoken_t token, const kint_t value);
	void setInt_fld(const ktoken_t token, const kint_t value);
	void setInt_sfld(const ktoken_t token, const kint_t value);
	void setInt_elem(const kint_t index, const kint_t value);

	void setFloat_loc(const ktoken_t token, const kfloat_t &value);
	void setFloat_arg(const ktoken_t token, const kfloat_t &value);
	void setFloat_argind(const ktoken_t token, const kfloat_t &value);
	void setFloat_fld(const ktoken_t token, const kfloat_t &value);
	void setFloat_sfld(const ktoken_t token, const kfloat_t &value);
	void setFloat_elem(const kint_t index, const kfloat_t &value);

	void setString_loc(const ktoken_t token, kcstring_t value);
	void setString_arg(const ktoken_t token, kcstring_t value);
	void setString_argind(const ktoken_t token, kcstring_t value);
	void setString_fld(const ktoken_t token, kcstring_t value);
	void setString_sfld(const ktoken_t token, kcstring_t value);
	void setString_elem(const kint_t index, kcstring_t value);

	// gets string or array length
	kint_t getLength_loc(const ktoken_t token);
	kint_t getLength_arg(const ktoken_t token);
	kint_t getLength_argind(const ktoken_t token);
	kint_t getLength_fld(const ktoken_t token);
	kint_t getLength_sfld(const ktoken_t token);
	kint_t getLength_elem(const kint_t index);

	// loads data onto the stack
	void loadBool(const kbool_t value);
	void loadInt(const kint_t value);
	void loadFloat(const kfloat_t &value);
	void loadString(kcstring_t value);
	
	void load_loc(const ktoken_t token);
	void load_arg(const ktoken_t token);
	void load_fld(const ktoken_t token);
	void load_sfld(const ktoken_t token);
	void load_elem(const kint_t index);

	void loadLength_loc(const ktoken_t token);
	void loadLength_arg(const ktoken_t token);
	void loadLength_fld(const ktoken_t token);
	void loadLength_sfld(const ktoken_t token);
	void loadLength_elem(const kint_t index);

	void loadAddress_loc(const ktoken_t token);
	void loadAddress_arg(const ktoken_t token);
	void loadAddress_fld(const ktoken_t token);
	void loadAddress_sfld(const ktoken_t token);
	void loadAddress_elem(const kint_t index);

	void loadIndirect();

	void loadNewObject(const ktoken_t classToken);
	void loadNewDelegate(const ktoken_t delegateTypeToken);
	void loadNewArray(const ktoken_t arrTypeToken);

	void loadNull();
	void loadThis();

	// stores data on the stack into storage units
	void store_loc(const ktoken_t token);
	void store_arg(const ktoken_t token);
	void store_fld(const ktoken_t token);
	void store_sfld(const ktoken_t token);
	void store_elem(const kint_t index);

	void storeIndirect();

	// checks for null object
	kbool_t isNull();
	kbool_t isNull_loc(const ktoken_t token);
	kbool_t isNull_arg(const ktoken_t token);
	kbool_t isNull_fld(const ktoken_t token);
	kbool_t isNull_sfld(const ktoken_t token);
	kbool_t isNull_elem(const kint_t index);

	// compares two references on the stack
	kbool_t referenceEquals();

	// invokes a method
	void invokeMethod(const ktoken_t token);
	void invokeObject(); //for delegate instance
	
	// allocates and initializes local variable
	void allocLocals(const ktoken_t *localTypes);

	//===========================================
	// KMachine methods
	//===========================================

	KObject * allocObjects(uint32_t length);
	void freeObjects(const KObject *objects);
	void referObjects(const KObject *objects);
};
