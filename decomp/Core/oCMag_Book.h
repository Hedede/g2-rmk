//--------------------------------------
// Magie buch
//--------------------------------------

/* Beschreibungen ergänzt von Mud-freak. Danke dafür! */

/* Genutzt um den Kreis über dem Spieler bei der Zauberauswahl anzuzeigen 
 * außerdem für zuordnungen von Zaubern <-> Items <-> Tasten. */

class oCMag_Book {
    zCArray    <oCSpell*>   spells;
    zCArray    <oCItem*>    spellitems;
    
    zCWorld*     wld;
    zCVob*       owner;
    zCModel*  model;           //model of the owner (seems to be kind of redundant)
    int spellnr;            //selected spell --> n+4 = Slot/Taste
    zREAL MAG_HEIGHT;         //some offset to shift the spell above the head of the owner (for spell choosing)
    zBOOL active;             //unused??
    zBOOL remove_all;         //some internal stuff? --> Beim Schließen des MagBooks, wenn ein vorher gezogene Zauber weggesteckt wird auf 1, sonst auf 0 (ALLE (vorher gezogener einbegriffen) Spells wieder zurück "in die Hüfte")
   zBOOL  in_movement;        //currently rotating the spells above the player head? --> und öffnen oder schließen
    zBOOL    show_handsymbol;    //? --> PFX bzw. Spell in der Hand (vergleichbar mit gezogen oder nicht)
  zREAL    step;               //if n spells are in the mag book this is 360/n 
  int      action;             //internal --> 0 = keine Aktion, 1 = drehen, 2 = öffnen, 3 = schließen
  int      UNUSED;             //
  zBOOL    open;               //currently showing mag book (cirlce above player head)?
  zREAL    open_delay_timer;   //used for delaying the time until the rune turns into a pfx --> 2000 msec
  zBOOL    show_particles;     //currently rendering the spell above the player head as a pfx?
  zREAL    targetdir;          //used for turning the spellbook over time when player pressed "left" or "right" --> um wieviel Grad drehen; Ist nach rechts wie die Eigenschaft step (also 360/n), aber nach links ((360/n)-1)*-1 (negativ und ein Grad weniger)
 zREAL      t1;                 // - " -  --> "Keyframes": FLOATNULL = Die Eigenschaft action startet, FLOATEINS = Die Eigenschaft action endet
  zVEC3    targetpos[3];       //used for popping out the magbook (from the hips) and closing it again. --> von Position
  zVEC3     startpos[3];        // - " -  --> nach Position (auch beim Schließen, d.h. startpos und targetpos werden ausgetauscht)

 int    nextRegister;              //not sure. Something with key assignment? --> Dieser Wert scheint sich nie zu ändern
  int   keys;                   //bitfield. If key n \in {0, 1, ..., 9} is used, keys & (1 << n) is true.  --> zeigt, ob ein Zauber im Slot (Taste) n+4 angelegt ist, hört also eigentlich bei n = 6 (Anzahl der Tasten für die Zauber) auf (nicht bei 9)
};
