#ifndef _MENUTASK_h
#define _MENUTASK_h
#include <Arduino.h>
#include <Qlist.h>

#define ITEMS4DISLAY 4

enum itemtype_t : uint8_t
{
    ITEM_EMPTY,
    ITEM_COMMAND,
    ITEM_SUBMENU,
    ITEM_BOOL,
    ITEM_FLOAT,
    ITEM_INT
};

class Scene{
    public:
    Scene();
    void setActiveMenu(Menu * m){active_menu=m;}
    virtual void command(uint8_t);
    virtual void setNewValue(uint8_t id,bool value);
    virtual void setNewValue(uint8_t id,float value);
    virtual void setNewValue(uint8_t id,int value);
    protected:
    Menu * active_menu;
};

class Menu{
public:
    Menu(Scene *s){
        scene = s;
        //parent=par;
        active_index=0;
        active_item=NULL;
    }
//bool isSubMenu(){return parent!=NULL;}
void setActive(bool en){active=en;}
String getMenu4draw(){
    String s="";
    if (items.length()<1 || active_item==NULL) return s;
    for (uint8_t i=active_index/ITEMS4DISLAY*ITEMS4DISLAY;i<ITEMS4DISLAY;i++){
        if (i>=items.length()) return s;
        if (s.length()>0) s+=";";
            if (i==active_index) {
            if (active_item->getKind()>=ITEM_BOOL && ((MenuItemParametr *)active_item)->isEditing())
            s+="*"; else s+=">";
            }
            else s+=" ";
            s+=active_item->getStrValue();
    }
    return s;
}
void addItem(MenuItem * mi){mi->setMenu(this);items.push_back(mi);if (items.length()==1) active_item=mi;}
void onUp(){if (items.length()<1) return;if (active_index>0) active_index--; active_item=items.get(active_index);}
void onDown(){if (items.length()<1) return;if (active_index<items.length()-1) active_index++; active_item=items.get(active_index);}
void onSelect(){
if (!active_item || !active_item->isEnabled()) return;   
switch (active_item->getKind()){
    case ITEM_COMMAND:
     scene->command(active_item->getID());
    break;
    case ITEM_SUBMENU:
    scene->setActiveMenu(((MenuItemSubmenu *)active_item)->getSubMenu());
    break;
    case ITEM_BOOL:
    ((MenuItemParametr *)active_item)->select();
    if (((MenuItemParametr *)active_item)->isChanged()) scene->setNewValue(active_item->getID(),((MenuItemParametrB *)active_item)->getValue());
    break;
    case ITEM_INT:
    ((MenuItemParametr *)active_item)->select();
    if (((MenuItemParametr *)active_item)->isChanged()) scene->setNewValue(active_item->getID(),((MenuItemParametrI *)active_item)->getValue());
    break;
    case ITEM_FLOAT:
    ((MenuItemParametr *)active_item)->select();
    if (((MenuItemParametr *)active_item)->isChanged()) scene->setNewValue(active_item->getID(),((MenuItemParametrF *)active_item)->getValue());
    
    break;
}
}
protected:
Scene *scene;
bool active;
//Menu * parent;
MenuItem * active_item;
uint8_t active_index;
QList<MenuItem *> items;

};

class MenuItem{
    public:
    MenuItem(String n,uint8_t d){name=n;id=d;enabled=true;}
    void setEnabled(bool en){enabled=en;}
    bool isEnabled(){return enabled;}
    uint8_t getID(){return id;}
    void setMenu(Menu * m){menu=m;}
    itemtype_t getKind(){return kind;};
    virtual String getStrValue();
    protected:
    Menu * menu;
    bool enabled;
    
    uint8_t id;
    String name;
    itemtype_t kind;
};

class MenuItemCommand : public MenuItem{
    public:
     MenuItemCommand(String n,uint8_t d):MenuItem(n,d){kind=ITEM_COMMAND;}
     String getStrValue(){return name;};
    protected:
    
};

class MenuItemSubmenu : public MenuItem{
    public:
    MenuItemSubmenu(String n,uint8_t d,Menu * m):MenuItem(n,d){kind=ITEM_SUBMENU;submenu=m;}
    Menu * getSubMenu(){return submenu;}
    String getStrValue(){return name+"...";};
    protected:
    Menu * submenu;  
};

class MenuItemParametr : public MenuItem{
    public:
    MenuItemParametr(String n,uint8_t d):MenuItem(n,d){kind=ITEM_BOOL;edit=false;}
    void select(){edit=!edit;}
    bool isEditing(){return edit;}
    virtual void up();
    virtual void down();
    virtual bool isChanged()=0;
    virtual void cancel()=0;
    protected:
    bool edit;
    
};

class MenuItemParametrB : public MenuItemParametr{
    public:
    MenuItemParametrB(String n,uint8_t d,bool value):MenuItemParametr(n,d){parametr=value;new_parametr=value;}
    void up(){new_parametr=!new_parametr;}
    void down(){new_parametr=!new_parametr;}
    bool isChanged(){return !edit && parametr!=new_parametr;};
    bool getValue(){parametr=new_parametr; return parametr;}
    String getStrValue(){return name+new_parametr?" YES":" NO";}
    void cancel(){new_parametr=parametr;}
    protected:
    bool parametr,new_parametr;
};

class MenuItemParametrI : public MenuItemParametr{
    public:
    MenuItemParametrI(String n,uint8_t d,int value,int st, int mi, int ma):MenuItemParametr(n,d)
    {parametr=value;parametr=value;new_parametr=value=st;min=mi;max=ma;kind=ITEM_INT;}
    void up(){new_parametr=(new_parametr+step>max)?max:new_parametr+step;}
    void down(){new_parametr=(new_parametr-step<min)?min:new_parametr-step;}
    bool isChanged(){return !edit && parametr!=new_parametr;};
    int getValue(){parametr=new_parametr; return parametr;}
    String getStrValue(){return name+" "+String(new_parametr);}
    void cancel(){new_parametr=parametr;}
    protected:
    int parametr,new_parametr;
    int step,min,max;
};

class MenuItemParametrF : public MenuItemParametr{
    public:
    MenuItemParametrF(String n,uint8_t d,float value,float st, float mi, float ma):MenuItemParametr(n,d)
    {parametr=value;parametr=value;new_parametr=value;step=st;min=mi;max=ma;kind=ITEM_FLOAT;}
    void up(){new_parametr=(new_parametr+step>max)?max:new_parametr+step;}
    void down(){new_parametr=(new_parametr-step<min)?min:new_parametr-step;}
    bool isChanged(){return !edit && parametr!=new_parametr;};
    int getValue(){parametr=new_parametr; return parametr;}
    String getStrValue(){return name+" "+String(new_parametr);}
    void cancel(){new_parametr=parametr;}
    protected:
    float parametr,new_parametr;
    float step,min,max;
};
#endif