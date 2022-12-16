
/* bug #1936 - Compiler produces broken Assembly (129 operand for bne) */

#include <stdint.h>

static uint8_t item_counter;


static uint8_t freeze;
static uint8_t powerUp;

static uint8_t wall_appeared;
static uint8_t freeze_locked;
static uint8_t zombie_locked;


struct  ItemStruct
{
    uint8_t _active;
    void(*_effect)(void);
} ;
typedef struct ItemStruct Item;

static Item freezeItem;
static Item powerUpItem;
static Item wallItem;
static Item zombieItem;


static Item extraPointsItem[1];


uint8_t find_inactive(Item* itemArray)
{
}


void drop_item(register Item *item, uint8_t max_counter)
{
}


void handle_item_drop(void)
{
    {
        if(item_counter==1)
        {
            if(!powerUpItem._active)
            {
                drop_item(&powerUpItem,35);
            }
        }
        else if((!freeze_locked)&&(!freeze))
        {
            if(!freezeItem._active)
            {
                drop_item(&freezeItem,45);
            }
        }
        else if(!wall_appeared&&(powerUp>=9))
        {
            if(!wallItem._active)
            {
                drop_item(&wallItem,35);
            }
        }
        else if(!zombie_locked && !zombieItem._active)
            {
                drop_item(&zombieItem,50);
            }
        else
        {
            uint8_t index;

            index = find_inactive(extraPointsItem);
            if(index!=1) // REMARK: compilation does not fail with 0
            {
                drop_item(&extraPointsItem[index],90);
            }
        }
    }
}

int main(void)
{
    return 0;
}

