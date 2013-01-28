/*
 *  Undo.cpp
 *  3D BoA Editor
 *
 *  Created by Niemand on 7/10/10.
 *
 */

#include "Undo.h"
#include <stdexcept>

extern scenario_data_type scenario;
extern Boolean editing_town;
extern town_record_type town;
extern big_tr_type t_d;
extern outdoor_record_type current_terrain;
extern SelectionType::SelectionType_e selected_object_type;
extern unsigned short selected_object_number;

namespace Undo{
	
	UndoStep::GroupType UndoStep::getGroupType() const{
		return(NORMAL);
	}
	
	//--------
	
	UndoGroupDelimiter::UndoGroupDelimiter(GroupType type):myType(type),myDesc(""){
		//TODO: maybe assert that type is not NORMAL?
	}
	
	UndoGroupDelimiter::UndoGroupDelimiter(GroupType type, const std::string& desc):
	myType(type),myDesc(desc){}
	
	void UndoGroupDelimiter::apply() const{
		//do nothing
	}
	
	void UndoGroupDelimiter::invert(){
		switch(myType){
			case NORMAL:
				//do nothing
				break;
			case BEGIN_GROUP:
				myType=END_GROUP;
				break;
			case END_GROUP:
				myType=BEGIN_GROUP;
				break;
			case SAVE_DELIMITER:
				//do nothing
				break;
			case DESCRIPTION_ONLY:
				//do nothing
				break;
		}
	}
	
	const std::string& UndoGroupDelimiter::description() const{
		return(myDesc);
	}
	
	UndoStep::GroupType UndoGroupDelimiter::getGroupType() const{
		return(myType);
	}
	
	//--------
	
	RectChangeStep::RectChangeStep(short newValue, short oldValue, Side s):
	BasicUndoStep<short>(newValue,oldValue),side(s){}
	
	void RectChangeStep::applyToRect(Rect& r) const{
		switch(side){
			case TOP:
				applyTo(r.top);
				break;
			case BOTTOM:
				applyTo(r.bottom);
				break;
			case LEFT:
				applyTo(r.left);
				break;
			case RIGHT:
				applyTo(r.right);
				break;
		}
	}
	
	//--------
	
	FloorChange::FloorChange(short changeX, short changeY, unsigned char newFloor, unsigned char oldFloor):
	BasicUndoStep<unsigned char>(newFloor, oldFloor),x(changeX),y(changeY){}
	
	void FloorChange::apply() const{
		if(editing_town)
			applyTo(t_d.floor[x][y]);
		else
			applyTo(current_terrain.floor[x][y]);
	}
	
	const std::string& FloorChange::description() const{
		return(descriptionStr);
	}
	
	const std::string FloorChange::descriptionStr("Draw Floor");
	
	//--------
	
	TerrainChange::TerrainChange(short changeX, short changeY, short newTerrain, short oldTerrain):
	BasicUndoStep<short>(newTerrain, oldTerrain),x(changeX),y(changeY){}
	
	void TerrainChange::apply() const{
		if(editing_town)
			applyTo(t_d.terrain[x][y]);
		else
			applyTo(current_terrain.terrain[x][y]);
	}
	
	const std::string& TerrainChange::description() const{
		return(descriptionStr);
	}
	
	const std::string TerrainChange::descriptionStr("Draw Terrain");
	
	//--------
	
	HeightChange::HeightChange(short changeX, short changeY, short newHeight, short oldHeight):
	BasicUndoStep<unsigned char>(newHeight,oldHeight),x(changeX),y(changeY){}
	
	void HeightChange::apply() const{
		if(editing_town)
			applyTo(t_d.height[x][y]);
		else
			applyTo(current_terrain.height[x][y]);
	}
	
	const std::string& HeightChange::description() const{
		return(descriptionStr);
	}
	
	const std::string HeightChange::descriptionStr("Change Height");
	
	//--------
	
	CreateSpecialEncounter::CreateSpecialEncounter(unsigned short i, const Rect& r, short s, bool c):
	CreationStep<specialEncounter>(i,specialEncounter(r,s),c){}
	
	void CreateSpecialEncounter::apply() const{
		if(editing_town){
			if(create){
				if(town.spec_id[index] == kNO_TOWN_SPECIALS)
					throw std::logic_error("Undo history corruption: Attempt to delete a town special encounter from an empty slot");
				town.spec_id[index] = kNO_TOWN_SPECIALS;
				SetRect(&town.special_rects[index],-1,-1,-1,-1);
			}
			else{
				if(town.spec_id[index] != kNO_TOWN_SPECIALS)
					throw std::logic_error("Undo history corruption: Attempt to un-delete a town special encounter into a non-empty slot");
				town.spec_id[index] = obj.state;
				town.special_rects[index] = obj.outline;
			}
		}
		else{ //outdoors
			if(create){
				if(current_terrain.spec_id[index] == kNO_OUT_SPECIALS)
					throw std::logic_error("Undo history corruption: Attempt to delete an outdoor special encounter from an empty slot");
				current_terrain.spec_id[index] = kNO_OUT_SPECIALS;
				SetRect(&current_terrain.special_rects[index],-1,-1,-1,-1);
			}
			else{
				if(current_terrain.spec_id[index] != kNO_OUT_SPECIALS)
					throw std::logic_error("Undo history corruption: Attempt to un-delete an outdoor special encounter into a non-empty slot");
				current_terrain.spec_id[index] = obj.state;
				current_terrain.special_rects[index] = obj.outline;
			}
		}
		if(create){
			if(selected_object_type==SelectionType::SpecialEncounter && selected_object_number==index)
				setSelection(SelectionType::None,0);
		}
		else
			setSelection(SelectionType::SpecialEncounter,index,true);
	}
	
	template <>
	const std::string CreationStep<specialEncounter>::createDescription("Place Special Encounter");
	template <>
	const std::string CreationStep<specialEncounter>::removeDescription("Delete Special Encounter");
	
	//--------
	
	SpecialEncounterStateChange::SpecialEncounterStateChange(unsigned char n, unsigned char o, unsigned int i):
	BasicUndoStep<unsigned char>(n,o),index(i){}
	
	void SpecialEncounterStateChange::apply() const{
		if(editing_town){
			if(town.spec_id[index] == kNO_TOWN_SPECIALS)
				throw std::logic_error("Undo history corruption: Attempt to modify a town special encounter in an empty slot");
			applyTo(town.spec_id[index]);
		}
		else{ //outdoors
			if(current_terrain.spec_id[index] == kNO_OUT_SPECIALS)
				throw std::logic_error("Undo history corruption: Attempt to modify an outdoor special encounter in an empty slot");
			//This cast is a bit questionable but seems to behave correctly
			applyTo((unsigned char&)current_terrain.spec_id[index]);
		}
		setSelection(SelectionType::SpecialEncounter,index,true);
	}
	
	const std::string& SpecialEncounterStateChange::description() const{
		return(descriptionStr);
	}
	
	const std::string SpecialEncounterStateChange::descriptionStr("Change Encounter State");
	
	//--------
	
	SpecialEncounterRectChange::SpecialEncounterRectChange(short n, short o, RectChangeStep::Side s, unsigned int i):
	RectChangeStep(n,o,s),index(i){}
	
	void SpecialEncounterRectChange::apply() const{
		if(editing_town){
			if(town.spec_id[index] == kNO_TOWN_SPECIALS)
				throw std::logic_error("Undo history corruption: Attempt to modify a town special encounter in an empty slot");
			applyToRect(town.special_rects[index]);
		}
		else{ //outdoors
			if(current_terrain.spec_id[index] == kNO_OUT_SPECIALS)
				throw std::logic_error("Undo history corruption: Attempt to modify an outdoor special encounter in an empty slot");
			applyToRect(current_terrain.special_rects[index]);
		}
		setSelection(SelectionType::SpecialEncounter,index,true);
	}
	
	const std::string& SpecialEncounterRectChange::description() const{
		return(descriptionStr);
	}
	
	const std::string SpecialEncounterRectChange::descriptionStr("Change Encounter Rectangle");
	
	//--------
	
	CreateField::CreateField(unsigned short i, const preset_field_type& f, bool c):
	CreationStep<preset_field_type>(i,f,c){}
	
	void CreateField::apply() const{
		if(create){
			if(town.preset_fields[index].field_type == -1)
				throw std::logic_error("Undo history corruption: Attempt to delete a preset field from an empty slot");
			town.preset_fields[index].clear_preset_field_type();
		}
		else{
			if(town.preset_fields[index].field_type != -1)
				throw std::logic_error("Undo history corruption: Attempt to un-delete a preset field into a non-empty slot");
			town.preset_fields[index]=obj;
		}
	}
	
	template<>
	const std::string CreationStep<preset_field_type>::createDescription("Place Field");
	template<>
	const std::string CreationStep<preset_field_type>::removeDescription("Remove Field");
	
	//--------
	
	CreateCreature::CreateCreature(unsigned short i, const creature_start_type& r, bool c):
	CreationStep<creature_start_type>(i,r,c){}
	
	void CreateCreature::apply() const{
		if(create){
			if(!town.creatures[index].exists())
				throw std::logic_error("Undo history corruption: Attempt to delete a creature from an empty slot");
			town.creatures[index].number = -1;
			if(selected_object_type==SelectionType::Creature && selected_object_number==index)
				setSelection(SelectionType::None,0);
		}
		else{
			if(town.creatures[index].exists())
				throw std::logic_error("Undo history corruption: Attempt to un-delete a creature from an non-empty slot");
			town.creatures[index]=obj;
			setSelection(SelectionType::Creature,index,true);
		}
	}
	
	template <>
	const std::string CreationStep<creature_start_type>::createDescription("Place Creature");
	template <>
	const std::string CreationStep<creature_start_type>::removeDescription("Delete Creature");
	
	//--------
	
	CreatureLocationChangeStep::CreatureLocationChangeStep(location n, location o, unsigned int i):
	BasicUndoStep<location>(n,o),index(i){}
	
	void CreatureLocationChangeStep::apply() const{
		if(!town.creatures[index].exists())
			throw std::logic_error("Undo history corruption: Attempt to change location of a creature in an empty slot");
		applyTo(town.creatures[index].start_loc);
	}
	
	const std::string& CreatureLocationChangeStep::description() const{
		return(descriptionStr);
	}
	
	const std::string CreatureLocationChangeStep::descriptionStr("Move Creature");
	
	//--------
	
	MiscCreaturePropertyChange::MiscCreaturePropertyChange(short n, short o, Property p, unsigned short i):
	BasicUndoStep<short>(n,o),prop(p),index(i){}
	
	void MiscCreaturePropertyChange::apply() const{
		if(!town.creatures[index].exists())
			throw std::logic_error("Undo history corruption: Attempt to modify property of a creature in an empty slot");
		switch(prop){
			case TYPE:
				applyTo(town.creatures[index].number);
				break;
			case FACING:
				applyTo(town.creatures[index].facing);
				break;
			case ATTITUDE:
				applyTo(town.creatures[index].start_attitude);
				break;
			case PERSONALITY:
				applyTo(town.creatures[index].personality);
				break;
			case EXTRA_ITEM_1:
				applyTo(town.creatures[index].extra_item);
				break;
			case EXTRA_CHANCE_1:
				applyTo(town.creatures[index].extra_item_chance_1);
				break;
			case EXTRA_ITEM_2:
				applyTo(town.creatures[index].extra_item_2);
				break;
			case EXTRA_CHANCE_2:
				applyTo(town.creatures[index].extra_item_chance_2);
				break;
			case HIDDEN_CLASS:
				applyTo(town.creatures[index].hidden_class);
				break;
			case ID:
				applyTo(town.creatures[index].character_id);
				break;
			case TIME_FLAG:
				applyTo(town.creatures[index].time_flag);
				break;
			case TIME:
				applyTo(town.creatures[index].creature_time);
				break;
			case EVENT:
				applyTo(town.creatures[index].attached_event);
				break;
			case UNIQUE:
				applyTo(town.creatures[index].unique_char);
				break;
			case ACT_AT_DISTANCE:
				applyTo(town.creatures[index].act_at_distance);
				break;
		}
		setSelection(SelectionType::Creature,index,true);
	}
	
	const std::string& MiscCreaturePropertyChange::description() const{
		return(descriptionStr);
	}
	
	const std::string MiscCreaturePropertyChange::descriptionStr("Change Creature Property");
	
	//--------
	
	CreatureMemoryCellChange::CreatureMemoryCellChange(short n, short o, unsigned char c, unsigned int i):
	BasicUndoStep<short>(n,o),index(i),cell(c){}
	
	void CreatureMemoryCellChange::apply() const{
		if(!town.creatures[index].exists())
			throw std::logic_error("Undo history corruption: Attempt to modify memory cell of a creature in an empty slot");
		if(cell>=10)
			throw std::logic_error("Undo history corruption: Attempt to modify out of range creature memory cell");
		applyTo(town.creatures[index].memory_cells[cell]);
		setSelection(SelectionType::Creature,index,true);
	}
	
	const std::string& CreatureMemoryCellChange::description() const{
		return(descriptionStr);
	}
	
	const std::string CreatureMemoryCellChange::descriptionStr("Creature Memory Cell");
	
	//--------
	
	CreatureScriptChangeStep::CreatureScriptChangeStep(char* n, char* o, unsigned int i):
	StringChangeStep<SCRIPT_NAME_LEN>(n,o),index(i){}
	
	void CreatureScriptChangeStep::apply() const{
		if(!town.creatures[index].exists())
			throw std::logic_error("Undo history corruption: Attempt to change script of a creature in an empty slot");
		applyToBuffer(town.creatures[index].char_script);
		setSelection(SelectionType::Creature,index,true);
	}
	
	const std::string& CreatureScriptChangeStep::description() const{
		return(descriptionStr);
	}
	
	const std::string CreatureScriptChangeStep::descriptionStr("Change Creature Script");
	
	//--------
	
	CreateItem::CreateItem(unsigned short i, const item_type& t, bool c):
	CreationStep<item_type>(i,t,c){}
	
	void CreateItem::apply() const{
		if(create){
			if(!town.preset_items[index].exists())
				throw std::logic_error("Undo history corruption: Attempt to delete an item from an empty slot");
			town.preset_items[index].which_item = -1;
			if(selected_object_type==SelectionType::Item && selected_object_number==index)
				setSelection(SelectionType::None,0);
		}
		else{
			if(town.preset_items[index].exists())
				throw std::logic_error("Undo history corruption: Attempt to un-delete an item from an non-empty slot");
			town.preset_items[index]=obj;
			setSelection(SelectionType::Item,index,true);
		}
	}
	
	template <>
	const std::string CreationStep<item_type>::createDescription("Place Item");
	template <>
	const std::string CreationStep<item_type>::removeDescription("Delete Item");
	
	//--------
	
	ItemTypeChange::ItemTypeChange(short n, short o, unsigned int i):
	BasicUndoStep<short>(n,o),index(i){}
	
	void ItemTypeChange::apply() const{
		if(!town.preset_items[index].exists())
			throw std::logic_error("Undo history corruption: Attempt to modify type of an item in an empty slot");
		applyTo(town.preset_items[index].which_item);
		setSelection(SelectionType::Item,index,true);
	}
	
	const std::string& ItemTypeChange::description() const{
		return(descriptionStr);
	}
	
	const std::string ItemTypeChange::descriptionStr("Change Item Type");
	
	//--------
	
	ItemLocationChangeStep::ItemLocationChangeStep(location n, location o, unsigned int i):
	BasicUndoStep<location>(n,o),index(i){}
	
	void ItemLocationChangeStep::apply() const{
		if(!town.preset_items[index].exists())
			throw std::logic_error("Undo history corruption: Attempt to modify position of an item in an empty slot");
		applyTo(town.preset_items[index].item_loc);
		setSelection(SelectionType::Item,index,true);
	}
	
	const std::string& ItemLocationChangeStep::description() const{
		return(descriptionStr);
	}
	
	const std::string ItemLocationChangeStep::descriptionStr("Move Item");
	
	//--------
	
	ItemShiftChangeStep::ItemShiftChangeStep(location n, location o, unsigned int i):
	BasicUndoStep<location>(n,o),index(i){}
	
	void ItemShiftChangeStep::apply() const{
		if(!town.preset_items[index].exists())
			throw std::logic_error("Undo history corruption: Attempt to modify drawing shift of an item in an empty slot");
		applyTo(town.preset_items[index].item_shift);
		setSelection(SelectionType::Item,index,true);
	}
	
	const std::string& ItemShiftChangeStep::description() const{
		return(descriptionStr);
	}
	
	const std::string ItemShiftChangeStep::descriptionStr("Change Item Drawing Shift");
	
	//--------
	
	MiscItemPropertyChange::MiscItemPropertyChange(unsigned char n, unsigned char o, Property p, unsigned short i):
	BasicUndoStep<unsigned char>(n,o),prop(p),index(i){}
	
	void MiscItemPropertyChange::apply() const{
		if(!town.preset_items[index].exists())
			throw std::logic_error("Undo history corruption: Attempt to modify property of an item in an empty slot");
		switch(prop){
			case CHARGES:
				applyTo(town.preset_items[index].charges);
				break;
			case PROPERTY_MASK:
				applyTo(town.preset_items[index].properties);
				break;
		}
		setSelection(SelectionType::Item,index,true);
	}
	
	const std::string& MiscItemPropertyChange::description() const{
		return(descriptionStr);
	}
	
	const std::string MiscItemPropertyChange::descriptionStr("Change Item Property");
	
	//--------
	
	CreateTerrainScript::CreateTerrainScript(unsigned short i, const in_town_on_ter_script_type& s, bool c):
	CreationStep<in_town_on_ter_script_type>(i,s,c){}
	
	void CreateTerrainScript::apply() const{
		if(create){
			if(!town.ter_scripts[index].exists)
				throw std::logic_error("Undo history corruption: Attempt to delete an item from an empty slot");
			town.ter_scripts[index].exists = false;
			if(selected_object_type==SelectionType::TerrainScript && selected_object_number==index)
				setSelection(SelectionType::None,0);
		}
		else{
			if(town.ter_scripts[index].exists)
				throw std::logic_error("Undo history corruption: Attempt to un-delete an item from an non-empty slot");
			town.ter_scripts[index]=obj;
			setSelection(SelectionType::TerrainScript,index,true);
		}
	}
	
	template <>
	const std::string CreationStep<in_town_on_ter_script_type>::createDescription("Place Terrain Script");
	template <>
	const std::string CreationStep<in_town_on_ter_script_type>::removeDescription("Delete Terrain Script");
	
	//--------
	
	TerrainScriptLocationChangeStep::TerrainScriptLocationChangeStep(location n, location o, unsigned int i):
	BasicUndoStep<location>(n,o),index(i){}
	
	void TerrainScriptLocationChangeStep::apply() const{
		applyTo(town.ter_scripts[index].loc);
		setSelection(SelectionType::TerrainScript,index,true);
	}
	
	const std::string& TerrainScriptLocationChangeStep::description() const{
		return(descriptionStr);
	}
	
	const std::string TerrainScriptLocationChangeStep::descriptionStr("Move Terrain Script");
	
	//--------
	
	TerrainScriptNameChangeStep::TerrainScriptNameChangeStep(char* n, char* o, unsigned int i):
	StringChangeStep<SCRIPT_NAME_LEN>(n,o),index(i){}
	
	void TerrainScriptNameChangeStep::apply() const{
		applyToBuffer(town.ter_scripts[index].script_name);
		setSelection(SelectionType::TerrainScript,index,true);
	}
	
	const std::string& TerrainScriptNameChangeStep::description() const{
		return(descriptionStr);
	}
	
	const std::string TerrainScriptNameChangeStep::descriptionStr("Change Terrain Script Name");
	
	//--------
	
	TerrainScriptMemoryCellChange::TerrainScriptMemoryCellChange(short n, short o, unsigned char c, unsigned int i):
	BasicUndoStep<short>(n,o),index(i),cell(c){}
	
	void TerrainScriptMemoryCellChange::apply() const{
		if(!town.ter_scripts[index].exists)
			throw std::logic_error("Undo history corruption: Attempt to modify memory cell of a terrain script in an empty slot");
		if(cell>=10)
			throw std::logic_error("Undo history corruption: Attempt to modify out of range terrain script memory cell");
		applyTo(town.ter_scripts[index].memory_cells[cell]);
		setSelection(SelectionType::TerrainScript,index,true);
	}
	
	const std::string& TerrainScriptMemoryCellChange::description() const{
		return(descriptionStr);
	}
	
	const std::string TerrainScriptMemoryCellChange::descriptionStr("Change Terrain Script Memory Cell");
	
	//--------
	
	CreateDescriptionArea::CreateDescriptionArea(unsigned short i, const Rect& r, const char* t, bool c):
	CreationStep<descriptionArea>(i,descriptionArea(r,t),c){}
	
	void CreateDescriptionArea::apply() const{
		if(editing_town){
			if(create){
				if(town.room_rect[index].right == 0)
					throw std::logic_error("Undo history corruption: Attempt to delete a town description area from an empty slot");
				SetRect(&town.room_rect[index],0,0,0,0);
			}
			else{
				if(town.room_rect[index].right != 0)
					throw std::logic_error("Undo history corruption: Attempt to un-delete a town description area into a non-empty slot");
				obj.assignTo(town.room_rect[index],town.info_rect_text[index]);
			}
		}
		else{ //outdoors
			if(create){
				if(current_terrain.info_rect[index].right == 0)
					throw std::logic_error("Undo history corruption: Attempt to delete an outdoor description area from an empty slot");
				SetRect(&current_terrain.info_rect[index],0,0,0,0);
			}
			else{
				if(current_terrain.info_rect[index].right != 0)
					throw std::logic_error("Undo history corruption: Attempt to un-delete an outdoor description area into a non-empty slot");
				obj.assignTo(current_terrain.info_rect[index],current_terrain.info_rect_text[index]);
			}
		}
		if(create){
			if(selected_object_type==SelectionType::AreaDescription && selected_object_number==index)
				setSelection(SelectionType::None,0);
		}
		else
			setSelection(SelectionType::AreaDescription,index,true);
	}
	
	template <>
	const std::string CreationStep<descriptionArea>::createDescription("Place Description Area");
	template <>
	const std::string CreationStep<descriptionArea>::removeDescription("Delete Description Area");
	
	//--------
	
	DescriptionAreaRectChange::DescriptionAreaRectChange(short n, short o, RectChangeStep::Side s, unsigned int i):
	RectChangeStep(n,o,s),index(i){}
	
	void DescriptionAreaRectChange::apply() const{
		if(editing_town){
			if(town.room_rect[index].right == 0)
				throw std::logic_error("Undo history corruption: Attempt to modify a town area description in an empty slot");
			applyToRect(town.room_rect[index]);
		}
		else{ //outdoors
			if(current_terrain.info_rect[index].right == 0)
				throw std::logic_error("Undo history corruption: Attempt to modify an outdoor area description in an empty slot");
			applyToRect(current_terrain.info_rect[index]);
		}
		setSelection(SelectionType::AreaDescription,index,true);
	}
	
	const std::string& DescriptionAreaRectChange::description() const{
		return(descriptionStr);
	}
	
	const std::string DescriptionAreaRectChange::descriptionStr("Change Area Description Rectangle");
	
	//--------
	
	DescriptionAreaTextChange::DescriptionAreaTextChange(char* n, char* o, unsigned int i):
	StringChangeStep<30>(n,o),index(i){}
	
	void DescriptionAreaTextChange::apply() const{
		if(editing_town){
			if(town.room_rect[index].right == 0)
				throw std::logic_error("Undo history corruption: Attempt to modify a town area description in an empty slot");
			applyToBuffer(town.info_rect_text[index]);
		}
		else{ //outdoors
			if(current_terrain.info_rect[index].right == 0)
				throw std::logic_error("Undo history corruption: Attempt to modify an outdoor area description in an empty slot");
			applyToBuffer(current_terrain.info_rect_text[index]);
		}
		setSelection(SelectionType::AreaDescription,index,true);
	}
	
	const std::string& DescriptionAreaTextChange::description() const{
		return(descriptionStr);
	}
	
	const std::string DescriptionAreaTextChange::descriptionStr("Change Area Description Text");
	
	//--------
	
	CreateSign::CreateSign(unsigned short i, const location& l, const char* t, bool c):
	CreationStep<sign>(i,sign(l,t),c){}
	
	void CreateSign::apply() const{
		if(create){
			if(editing_town){
				if(town.sign_locs[index].x == kNO_SIGN)
					throw std::logic_error("Undo history corruption: Attempt to delete a sign from an empty slot");
				town.sign_locs[index].x = town.sign_locs[index].y = kNO_SIGN;
			}
			else{ //outdoors
				if(current_terrain.sign_locs[index].x == kNO_SIGN)
					throw std::logic_error("Undo history corruption: Attempt to delete a sign from an empty slot");
				current_terrain.sign_locs[index].x = current_terrain.sign_locs[index].y = kNO_SIGN;
			}
			setSelection(SelectionType::None,0);
		}
		else{
			if(editing_town){
				if(town.sign_locs[index].x != kNO_SIGN)
					throw std::logic_error("Undo history corruption: Attempt to un-delete a sign from a non-empty slot");
				obj.assignTo(town.sign_locs[index],town.sign_text[index]);
				setSelection(SelectionType::Sign,index,true);
			}
			else{ //outdoors
				if(current_terrain.sign_locs[index].x != kNO_SIGN)
					throw std::logic_error("Undo history corruption: Attempt to un-delete a sign from a non-empty slot");
				obj.assignTo(current_terrain.sign_locs[index],current_terrain.sign_text[index]);
				setSelection(SelectionType::Sign,index,true);
			}
		}
	}
	
	template <>
	const std::string CreationStep<sign>::createDescription("Place Sign");
	template <>
	const std::string CreationStep<sign>::removeDescription("Erase Sign");
	
	//--------
	
	SignTextChange::SignTextChange(char* n, char* o, unsigned int i):
	StringChangeStep<256>(n,o),index(i){}
	
	void SignTextChange::apply() const{
		if(editing_town){
			if(town.sign_locs[index].x==kNO_SIGN)
				throw std::logic_error("Undo history corruption: Attempt to modify a sign in an empty slot");
			applyToBuffer(town.sign_text[index]);
			setSelection(SelectionType::Sign,index,true);
		}
		else{ //outdoors
			if(current_terrain.sign_locs[index].x==kNO_SIGN)
				throw std::logic_error("Undo history corruption: Attempt to modify a sign in an empty slot");
			applyToBuffer(current_terrain.sign_text[index]);
			setSelection(SelectionType::Sign,index,true);
		}
	}
	
	const std::string& SignTextChange::description() const{
		return(descriptionStr);
	}
	
	const std::string SignTextChange::descriptionStr("Change Sign Text");
	
	//--------
	
	CreateTownEntrance::CreateTownEntrance(unsigned short i, const Rect& r, short t, bool c):
	CreationStep<townEntrance>(i,townEntrance(t,r),c){}
	
	void CreateTownEntrance::apply() const{
		if(create){
			if(current_terrain.exit_dests[index]==kNO_OUT_TOWN_ENTRANCE)
				throw std::logic_error("Undo history corruption: Attempt to delete a town entrance from an empty slot");
			current_terrain.exit_dests[index]=kNO_OUT_TOWN_ENTRANCE;
		}
		else{
			if(current_terrain.exit_dests[index]!=kNO_OUT_TOWN_ENTRANCE)
				throw std::logic_error("Undo history corruption: Attempt to un-delete a town entrance from a non-empty slot");
			obj.assignTo(current_terrain.exit_dests[index],current_terrain.exit_rects[index]);
		}
	}
	
	template <>
	const std::string CreationStep<townEntrance>::createDescription("Place Town Entrance");
	template <>
	const std::string CreationStep<townEntrance>::removeDescription("Erase Town Entrance");
	
	//--------
	
	TownEntranceTownChange::TownEntranceTownChange(short n, short o, unsigned int i):
	BasicUndoStep<short>(n,o),index(i){}
	
	void TownEntranceTownChange::apply() const{
		if(current_terrain.exit_dests[index] == kNO_OUT_TOWN_ENTRANCE)
			throw std::logic_error("Undo history corruption: Attempt to modify a town entrance in an empty slot");
		applyTo(current_terrain.exit_dests[index]);
		setSelection(SelectionType::TownEntrance,index,true);
	}
	
	const std::string& TownEntranceTownChange::description() const{
		return(descriptionStr);
	}
	
	const std::string TownEntranceTownChange::descriptionStr("Change Town Entrance");
	
	//--------
	
	TownEntranceRectChange::TownEntranceRectChange(short n, short o, RectChangeStep::Side s, unsigned int i):
	RectChangeStep(n,o,s),index(i){}
	
	void TownEntranceRectChange::apply() const{
		if(current_terrain.exit_dests[index] == kNO_OUT_TOWN_ENTRANCE)
			throw std::logic_error("Undo history corruption: Attempt to modify a town entrance in an empty slot");
		applyToRect(current_terrain.exit_rects[index]);
		setSelection(SelectionType::TownEntrance,index,true);
	}
	
	const std::string& TownEntranceRectChange::description() const{
		return(descriptionStr);
	}
	
	const std::string TownEntranceRectChange::descriptionStr("Change Town Entrance Rectangle");
	
	//--------
	
	CreateWaypoint::CreateWaypoint(unsigned short i, const location& l, bool c):
	CreationStep<waypoint>(i,waypoint(l),c){}
	
	void CreateWaypoint::apply() const{
		if(create){
			if(town.waypoints[index].x < 0)
				throw std::logic_error("Undo history corruption: Attempt to delete a waypoint from an empty slot");
			town.waypoints[index].x = -1;
			setSelection(SelectionType::None,0);
		}
		else{
			if(town.waypoints[index].x >= 0)
				throw std::logic_error("Undo history corruption: Attempt to un-delete a waypoint from a non-empty slot");
			obj.assignTo(town.waypoints[index]);
			setSelection(SelectionType::Waypoint,index,true);
		}
	}
	
	template <>
	const std::string CreationStep<waypoint>::createDescription("Place Waypoint");
	template <>
	const std::string CreationStep<waypoint>::removeDescription("Erase Waypoint");
	
	//--------
	
	WaypointLocationChangeStep::WaypointLocationChangeStep(location n, location o, unsigned int i):
	BasicUndoStep<location>(n,o),index(i){}
	
	void WaypointLocationChangeStep::apply() const{
		applyTo(town.waypoints[index]);
		setSelection(SelectionType::Waypoint,index,true);
	}
	
	const std::string& WaypointLocationChangeStep::description() const{
		return(descriptionStr);
	}
	
	const std::string WaypointLocationChangeStep::descriptionStr("Move Waypoint");
	
	//--------
	
	TownEntrancePointLocationChange::TownEntrancePointLocationChange(location n, location o, unsigned int i):
	BasicUndoStep<location>(n,o),index(i){}
	
	void TownEntrancePointLocationChange::apply() const{
		applyTo(town.start_locs[index]);
		setSelection(SelectionType::None,0);
	}
	
	const std::string& TownEntrancePointLocationChange::description() const{
		return(descriptionStr);
	}
	
	const std::string TownEntrancePointLocationChange::descriptionStr("Move Town Entrance Point");
	
	//--------
	
	ScenStartLocationTownChange::ScenStartLocationTownChange(location nl, location ol, short nt, short ot):
	lu(nl,ol),tu(nt,ot){}
	
	void ScenStartLocationTownChange::apply() const{
		lu.apply();
		tu.apply();
	}
	
	void ScenStartLocationTownChange::LocationChange::apply() const{
		applyTo(scenario.what_start_loc_in_town);
	}
	
	void ScenStartLocationTownChange::TownChange::apply() const{
		applyTo(scenario.start_in_what_town);
	}
	
	void ScenStartLocationTownChange::invert(){
		lu.invert();
		tu.invert();
	}
	
	const std::string& ScenStartLocationTownChange::description() const{
		return(descriptionStr);
	}
	
	const std::string ScenStartLocationTownChange::descriptionStr("Move Scenario Start Point");
	
	//--------
	
	ScenStartLocationOutdoorChange::ScenStartLocationOutdoorChange(location nl, location ol, location ns, location os):
	lu(nl,ol),su(ns,os){}
	
	void ScenStartLocationOutdoorChange::apply() const{
		lu.apply();
		su.apply();
	}
	
	void ScenStartLocationOutdoorChange::LocationChange::apply() const{
		applyTo(scenario.start_where_in_outdoor_section);
	}
	
	void ScenStartLocationOutdoorChange::SectionChange::apply() const{
		applyTo(scenario.what_outdoor_section_start_in);
	}
	
	void ScenStartLocationOutdoorChange::invert(){
		lu.invert();
		su.invert();
	}
	
	const std::string& ScenStartLocationOutdoorChange::description() const{
		return(descriptionStr);
	}
	
	const std::string ScenStartLocationOutdoorChange::descriptionStr("Move Scenario Start Point");
	
	//--------
	
	
	//--------
	
	UndoStack::~UndoStack(){
		clear();
	}
	
	void UndoStack::clear(){
		while(!steps.empty()){
			delete steps.back();
			steps.pop_back();
		}
	}
	
	bool UndoStack::empty() const{
		return(steps.empty());
	}
	
	void UndoStack::appendChange(UndoStep* step){
		switch(step->getGroupType()){
			case UndoStep::NORMAL:
				//std::cout << "Added normal undo step (" << step->description() << ')' << std::endl;
				steps.push_back(step);
				levelLengths.back()++;
				break;
			case UndoStep::BEGIN_GROUP:
				//std::cout << "Began undo group" << std::endl;
				steps.push_back(step);
				levelLengths.back()++;
				levelLengths.push_back(0);
				nestLevel++;
				break;
			case UndoStep::END_GROUP:
				//std::cout << "Ending undo group" << std::endl;
				if(nestLevel<1)
					throw std::logic_error("Undo history corruption: trying to end an undo group when no groups are open");
				if(levelLengths.back()<=1){
					//std::cout << "\tEliminating trivial group" << std::endl;
					//if there was an item in the group, preserve it
					if(levelLengths.back()==1){
						//std::cout << "\tGroup had one item; preserving" << std::endl;
						std::deque<UndoStep*>::reverse_iterator it=steps.rbegin()+1;
						delete *it;
						*(steps.rbegin()+1)=steps.back();
					}
					else
						delete steps.back();
					steps.pop_back();
					nestLevel--;
					levelLengths.pop_back();
				}
				else{
					//std::cout << "Ended undo group" << std::endl;
					steps.push_back(step);
					nestLevel--;
					levelLengths.pop_back();
				}
				break;
			case UndoStep::SAVE_DELIMITER:
				//std::cout << "Making note of save" << std::endl;
				//TODO: alternative might be to close any open groups
				if(nestLevel>0)
					throw std::logic_error("Undo history corruption: open undo group at point of save");
				steps.push_back(step);
				levelLengths.back()++;
				break;
			case UndoStep::DESCRIPTION_ONLY:
				if(nestLevel<1)
					throw std::logic_error("Undo system misuse: trying to add a description-only step when not inside an und group");
				//std::cout << "Added description-only step (" << step->description() << ')' << std::endl;
				steps.push_back(step);
				levelLengths.back()++;
		}
	}
	
	bool UndoStack::applyLast(UndoStack& counterpart, bool transfer){
		if(steps.empty()){
			//std::cout << "Nothing to Un/Redo" << std::endl;
			return(false);
		}
		//std::cout << "This stack stack now has " << steps.size() << (steps.size()==1?" item":" items") << std::endl;
		//TODO: alternative might be to close any open groups
		if(nestLevel>0)
			throw std::logic_error("Undo history corruption: open undo group at point of application");
		do{
			if(steps.empty())
				throw std::logic_error("Undo history corruption: too many group ending markers");
			UndoStep* step=steps.back();
			steps.pop_back();
			if(nestLevel==0)
				levelLengths.back()--;
			//std::cout << "Undoing: ";
			switch(step->getGroupType()){
				case UndoStep::NORMAL:
					//std::cout << step->description() << std::endl;
					step->apply();
					break;
				case UndoStep::BEGIN_GROUP:
					//std::cout << "Group beginning" << std::endl;
					if(nestLevel==0)
						throw std::logic_error("Undo history corruption: too many group beginning markers");
					nestLevel--;
					//std::cout << "\tLevel is now " << nestLevel << std::endl;
					break;
				case UndoStep::END_GROUP:
					//std::cout << "Group ending" << std::endl;
					nestLevel++;
					//std::cout << "\tLevel is now " << nestLevel << std::endl;
					break;
				case UndoStep::SAVE_DELIMITER:
					//std::cout << "Save delimiter" << std::endl;
					//TODO: Display a dialog here to ask whether the user wants to undo past the last save point
					//???: What should be done about older save markers, farther down the stack?
					break;
			}
			if(transfer){
				step->invert();
				counterpart.appendChange(step);
			}
			else
				delete step;
		}while(nestLevel);
		//std::cout << "This stack stack now has " << steps.size() << (steps.size()==1?" item":" items") << std::endl;
		//std::cout << "Counterpart stack now has " << counterpart.steps.size() << (counterpart.steps.size()==1?" item":" items") << std::endl;
		return(true);
	}
	
	const std::string& UndoStack::currentDescription() const{
		for(std::deque<UndoStep*>::const_reverse_iterator it=steps.rbegin(), end=steps.rend(); it!=end; ++it){
			if((**it).description()!=emptyDescription)
				return((**it).description());
		}
		return(emptyDescription);
	}
	
	const std::string UndoStack::emptyDescription("");
	
} //namespace Undo
