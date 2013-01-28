/*
 *  Undo.h
 *  3D BoA Editor
 *
 *  Created by Niemand on 7/10/10.
 *
 */

#ifndef UNDO_H
#define UNDO_H

#include <algorithm>
#include <deque>
#include <string>
#include "global.h"

namespace Undo{
	//Welcome to the land of a zillion little objects

	//================
	//Fundamental Types
	//================
	
	class UndoStep{
	public:
		enum GroupType{NORMAL,BEGIN_GROUP,END_GROUP,SAVE_DELIMITER,DESCRIPTION_ONLY};
		
		virtual ~UndoStep(){}
		virtual void apply() const=0;
		virtual void invert()=0;
		virtual const std::string& description() const=0;
		virtual GroupType getGroupType() const;
	};
	
	class UndoGroupDelimiter : public UndoStep{
	public:
		UndoGroupDelimiter(GroupType type);
		UndoGroupDelimiter(GroupType type, const std::string& desc);
		virtual void apply() const;
		virtual void invert();
		virtual const std::string& description() const;
		virtual GroupType getGroupType() const;
	private:
		GroupType myType;
		const std::string myDesc;
	};
	
	//================
	//Intermediate Types
	//These abstract types implement logic shared by common categories of undo steps. 
	//They are intended to be subclassed to form concrete types which know about other objects to which to apply changes. 
	//================
	
	template<typename T>
	class BasicUndoStep : public UndoStep{
	public:
		BasicUndoStep(const T& n, const T& o):newVal(n),oldVal(o){}
		virtual void invert(){
			std::swap(oldVal,newVal);
		}
	protected:
		T newVal;
		T oldVal;
		
		void applyTo(T& t) const{
			t=oldVal;
		}
	};
	
	template<typename T>
	class CreationStep : public UndoStep{
	public:
		typedef T objectType;
		CreationStep<T>(unsigned short i, const objectType& o, bool c):
		index(i),obj(o),create(c){}
		
		virtual void invert(){
			create=!create;
		}
		virtual const std::string& description() const{
			if(create)
				return(createDescription);
			return(removeDescription);
		}
	protected:
		unsigned short index;
		objectType obj;
		bool create;
	public:
		static const std::string createDescription;
		static const std::string removeDescription;
	};
	
	template<int size>
	class StringChangeStep : public UndoStep{
	public:
		StringChangeStep<size>(const std::string& n, const std::string& o):oldStr(&oldBuf[0]),newStr(&newBuf[0]){
			strncpy(newStr,n.c_str(),size);
			strncpy(oldStr,o.c_str(),size);
		}
		StringChangeStep<size>(const char* n, const char* o):oldStr(&oldBuf[0]),newStr(&newBuf[0]){
			strncpy(newStr,n,size);
			strncpy(oldStr,o,size);
		}
		virtual void apply() const=0;
		virtual void invert(){
			std::swap(oldStr,newStr);
		}
	protected:
		//Use fixed character arrays to avoid extra malloc'ing
		char oldBuf[size];
		char newBuf[size];
		//Use pointers to refer to the fixed arrays, since they can easily be switched
		char* oldStr;
		char* newStr;
		
		void applyToBuffer(char* buf) const{
			strncpy(buf,oldStr,size);
		}
	};
	
	class RectChangeStep : public BasicUndoStep<short>{
	public:
		enum Side{TOP,BOTTOM,LEFT,RIGHT};
		RectChangeStep(short newValue, short oldValue, Side s);
	protected:
		Side side;
		
		void applyToRect(Rect& r) const;
	};
	
	//================
	//Concrete Types
	//================
	
	class FloorChange : public BasicUndoStep<unsigned char>{
	public:
		FloorChange(short changeX, short changeY, unsigned char newFloor, unsigned char oldFloor);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		short x;
		short y;
		
		static const std::string descriptionStr;
	};
	
	class TerrainChange : public BasicUndoStep<short>{
	public:
		TerrainChange(short changeX, short changeY, short newTerrain, short oldTerrain);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		short x;
		short y;
		
		static const std::string descriptionStr;
	};
	
	class HeightChange : public BasicUndoStep<unsigned char>{
	public:
		HeightChange(short changeX, short changeY, short newHeight, short oldHeight);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		short x;
		short y;
		
		static const std::string descriptionStr;
	};
	
	struct specialEncounter{
		Rect outline;
		short state;
		specialEncounter(){}
		specialEncounter(const Rect& r, short s):
		outline(r),state(s){}
	};
	
	class CreateSpecialEncounter : public CreationStep<specialEncounter>{
	public:
		CreateSpecialEncounter(unsigned short i, const Rect& r, short s, bool c);
		virtual void apply() const;
	};
	
	class SpecialEncounterStateChange : public BasicUndoStep<unsigned char>{
	public:
		SpecialEncounterStateChange(unsigned char n, unsigned char o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		
		static const std::string descriptionStr;
	};
	
	class SpecialEncounterRectChange : public RectChangeStep{
	public:
		SpecialEncounterRectChange(short n, short o, RectChangeStep::Side s, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		
		static const std::string descriptionStr;
	};
	
	class CreateField : public CreationStep<preset_field_type>{
	public:
		CreateField(unsigned short i, const preset_field_type& f, bool c);
		virtual void apply() const;
	};
	
	class CreateCreature : public CreationStep<creature_start_type>{
	public:
		CreateCreature(unsigned short i, const creature_start_type& r, bool c);
		virtual void apply() const;
	};
	
	class CreatureLocationChangeStep : public BasicUndoStep<location>{
	public:
		CreatureLocationChangeStep(location n, location o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned short index;
		
		static const std::string descriptionStr;
	};
	
	class MiscCreaturePropertyChange : public BasicUndoStep<short>{
	public:
		enum Property{TYPE, FACING, ATTITUDE, PERSONALITY, EXTRA_ITEM_1, EXTRA_CHANCE_1, EXTRA_ITEM_2, EXTRA_CHANCE_2, 
		              HIDDEN_CLASS, ID, TIME_FLAG, TIME, EVENT, UNIQUE, ACT_AT_DISTANCE};
		MiscCreaturePropertyChange(short n, short o, Property p, unsigned short i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		Property prop;
		unsigned short index;
		
		static const std::string descriptionStr;
	};
	
	class CreatureMemoryCellChange : public BasicUndoStep<short>{
	public:
		CreatureMemoryCellChange(short n, short o, unsigned char c, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		unsigned char cell;
		
		static const std::string descriptionStr;
	};
	
	class CreatureScriptChangeStep : public StringChangeStep<SCRIPT_NAME_LEN>{
	public:
		CreatureScriptChangeStep(char* n, char* o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		
		static const std::string descriptionStr;
	};
	
	class CreateItem : public CreationStep<item_type>{
	public:
		CreateItem(unsigned short i, const item_type& t, bool c);
		virtual void apply() const;
	};
	
	class ItemTypeChange : public BasicUndoStep<short>{
	public:
		ItemTypeChange(short n, short o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		
		static const std::string descriptionStr;
	};
	
	class ItemLocationChangeStep : public BasicUndoStep<location>{
	public:
		ItemLocationChangeStep(location n, location o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned short index;
		
		static const std::string descriptionStr;
	};
	
	class ItemShiftChangeStep : public BasicUndoStep<location>{
	public:
		ItemShiftChangeStep(location n, location o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned short index;
		
		static const std::string descriptionStr;
	};
	
	class MiscItemPropertyChange : public BasicUndoStep<unsigned char>{
	public:
		enum Property{CHARGES, PROPERTY_MASK};
		MiscItemPropertyChange(unsigned char n, unsigned char o, Property p, unsigned short i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		Property prop;
		unsigned short index;
		
		static const std::string descriptionStr;
	};
	
	class CreateTerrainScript : public CreationStep<in_town_on_ter_script_type>{
	public:
		CreateTerrainScript(unsigned short i, const in_town_on_ter_script_type& s, bool c);
		virtual void apply() const;
	};
	
	class TerrainScriptLocationChangeStep : public BasicUndoStep<location>{
	public:
		TerrainScriptLocationChangeStep(location n, location o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned short index;
		
		static const std::string descriptionStr;
	};
	
	class TerrainScriptNameChangeStep : public StringChangeStep<SCRIPT_NAME_LEN>{
	public:
		TerrainScriptNameChangeStep(char* n, char* o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		
		static const std::string descriptionStr;
	};
	
	class TerrainScriptMemoryCellChange : public BasicUndoStep<short>{
	public:
		TerrainScriptMemoryCellChange(short n, short o, unsigned char c, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		unsigned char cell;
		
		static const std::string descriptionStr;
	};
	
	struct descriptionArea{
		Rect outline;
		char text[30];
		descriptionArea(){}
		descriptionArea(const Rect& r, const char* t):
		outline(r){
			strncpy(text, t, 30);
		}
		void assignTo(Rect& r, char* t) const{
			r=outline;
			strncpy(t,text,30);
		}
	};
	
	class CreateDescriptionArea : public CreationStep<descriptionArea>{
	public:
		CreateDescriptionArea(unsigned short i, const Rect& r, const char* t, bool c);
		virtual void apply() const;
	};
	
	class DescriptionAreaRectChange : public RectChangeStep{
	public:
		DescriptionAreaRectChange(short n, short o, RectChangeStep::Side s, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		
		static const std::string descriptionStr;
	};
	
	class DescriptionAreaTextChange : public StringChangeStep<30>{
	public:
		DescriptionAreaTextChange(char* n, char* o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		
		static const std::string descriptionStr;
	};
	
	struct sign{
		location loc;
		char text[256];
		sign(){}
		sign(const location& l, const char* t):loc(l){
			strncpy(text,t,256);
		}
		void assignTo(location& l, char* t) const{
			l=loc;
			strncpy(t,text,256);
		}
	};
	
	class CreateSign : public CreationStep<sign>{
	public:
		CreateSign(unsigned short i, const location& l, const char* t, bool c);
		virtual void apply() const;
	};
	
	class SignTextChange : public StringChangeStep<256>{
	public:
		SignTextChange(char* n, char* o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		
		static const std::string descriptionStr;
	};
	
	struct townEntrance{
		short town;
		Rect outline;
		townEntrance(){}
		townEntrance(short t, const Rect& r):
		town(t),outline(r){}
		void assignTo(short& t, Rect& r) const{
			t=town;
			r=outline;
		}
	};
	
	class CreateTownEntrance : public CreationStep<townEntrance>{
	public:
		CreateTownEntrance(unsigned short i, const Rect& r, short t, bool c);
		virtual void apply() const;
	};
	
	class TownEntranceTownChange : public BasicUndoStep<short>{
	public:
		TownEntranceTownChange(short n, short o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		
		static const std::string descriptionStr;
	};
	
	class TownEntranceRectChange : public RectChangeStep{
	public:
		TownEntranceRectChange(short n, short o, RectChangeStep::Side s, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned int index;
		
		static const std::string descriptionStr;
	};
	
	struct waypoint{
		location loc;
		explicit waypoint(const location& l):loc(l){}
		void assignTo(location& l) const{
			l=loc;
		}
	};
	
	class CreateWaypoint : public CreationStep<waypoint>{
	public:
		CreateWaypoint(unsigned short i, const location& l, bool c);
		virtual void apply() const;
	};
	
	class WaypointLocationChangeStep : public BasicUndoStep<location>{
	public:
		WaypointLocationChangeStep(location n, location o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned short index;
		
		static const std::string descriptionStr;
	};
	
	class TownEntrancePointLocationChange : public BasicUndoStep<location>{
	public:
		TownEntrancePointLocationChange(location n, location o, unsigned int i);
		virtual void apply() const;
		virtual const std::string& description() const;
	private:
		unsigned short index;
		
		static const std::string descriptionStr;
	};
	
	class ScenStartLocationTownChange : public UndoStep{
	public:
		ScenStartLocationTownChange(location nl, location ol, short nt, short ot);
		virtual void apply() const;
		virtual void invert();
		virtual const std::string& description() const;
	private:
		class LocationChange : public BasicUndoStep<location>{
		public:
			LocationChange(const location& n, const location& o):BasicUndoStep<location>(n,o){}
			virtual void apply() const;
			virtual const std::string& description() const{ return(descriptionStr); }
		} lu;
		class TownChange : public BasicUndoStep<short>{
		public:
			TownChange(const short& n, const short& o):BasicUndoStep<short>(n,o){}
			virtual void apply() const;
			virtual const std::string& description() const{ return(descriptionStr); }
		} tu;
		static const std::string descriptionStr;
	};
	
	class ScenStartLocationOutdoorChange : public UndoStep{
	public:
		ScenStartLocationOutdoorChange(location nl, location ol, location ns, location os);
		virtual void apply() const;
		virtual void invert();
		virtual const std::string& description() const;
	private:
		class LocationChange : public BasicUndoStep<location>{
		public:
			LocationChange(const location& n, const location& o):BasicUndoStep<location>(n,o){}
			virtual void apply() const;
			virtual const std::string& description() const{ return(descriptionStr); }
		} lu;
		class SectionChange : public BasicUndoStep<location>{
		public:
			SectionChange(const location& n, const location& o):BasicUndoStep<location>(n,o){}
			virtual void apply() const;
			virtual const std::string& description() const{ return(descriptionStr); }
		} su;
		static const std::string descriptionStr;
	};
	
	//Other things for which we could consider adding undo support:
	//modifying town details
	//modifying town wandering monsters
	//modifying town boundaries
	//modifying outdoor details
	//modifying oudoor wandering monsters
	//modifying oudoor special monsters
	//modifying oudoor preset monsters
	//modifying wandering monster spawn points
	
	class UndoStack{
	public:
		UndoStack():nestLevel(0),levelLengths(1,0){}
		~UndoStack();
		bool empty() const;
		void clear();
		void appendChange(UndoStep* step);
		//Apply the last change (or group of changes) on the stack, 
		//invert it, and move it to the counterpart stack.
		//If, however, transfer is false, simply discard the change after applying it. 
		//This makes it easy to rollback changes which the user aborted 
		//partway through, and which therefore 'never actually happened'. 
		bool applyLast(UndoStack& counterpart, bool transfer=true);
		const std::string& currentDescription() const;
		
	private:
		std::deque<UndoStep*> steps;
		unsigned int nestLevel;
		std::deque<unsigned int> levelLengths;
		
		static const std::string emptyDescription;
	};
	
} //namespace Undo

#endif