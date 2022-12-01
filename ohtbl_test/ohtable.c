#include "ohtable.h"

static STUDENT vacated_mem = {1};

__uint64 _hash1(__uint64 key)
{
	key += (key<<12);
	key ^= (key>>22);
	key += (key<<4);
	key ^= (key>>9);
	key += (key<<10);
	key ^= (key>>2);
	key += (key<<7);
	key ^= (key>>12);

	return key;
}


__uint64 _hash2(__uint64 key)
{
	key = ~key + (key << 21);	// key = (key << 21) - key - 1
	key ^= (key >> 24);
	key += (key << 3) + (key << 8);	// key = key * 265
	key ^= (key >> 14);
	key += (key << 2) + (key << 4);	// key = key * 21
	key ^= (key >> 28);
	key += (key << 31);

	return key;
}


static inline __uint64 _match(__uint64 val1, __uint64 val2)
{
    return val1 - val2;
}


__uint64 _ohtbl_resizing_insert(OHTBL* htbl, STUDENT* data)
{
    __uint64 position;

    for(__uint64 i=0; i<htbl->positions; i++)
    {
        position = ( _hash1(data->id) + (i * _hash2(data->id)) ) % htbl->positions;

        if(htbl->table[position].id == 0 || htbl->table[position].id == vacated_mem.id)
        {
            htbl->table[position] = *data;
            htbl->size++;
            return position;    //넣은 위치
        }
    }
    return ERROR;
}


int _ohtbl_resizing(OHTBL* htbl)
{
    __uint64 pos=0;
    __uint64 old_positions = htbl->positions;

    htbl->size=0;
    htbl->positions = (htbl->positions) * 4;
    
    if( (realloc(htbl->table, (htbl->positions) * sizeof(STUDENT))) == NULL)
        return ERROR;
    
    // initialize new slots
    for(__uint64 i=old_positions; i<htbl->positions; i++)
        htbl->table[i].id = 0;

    // rehashing
    for(__uint64 i=0; i<old_positions; i++)
    {
        if(htbl->table[i].id != 0 && htbl->table[i].id != vacated_mem.id)
        {    
            pos = _ohtbl_resizing_insert(htbl,&htbl->table[i]);
            if( pos == ERROR )
            {    
                printf("ERROR");
                return ERROR;
            }
            else if(pos != i)   // Set the original id of slot to vacated state when index change
                htbl->table[i].id = vacated_mem.id;
        }
    }
    return NO_ERROR;
}


void print_table(OHTBL* htbl)
{

    printf("table size = %lld\n",htbl->positions);
    printf("number of data = %lld\n",htbl->size);
    for(__uint64 i=0; i<(htbl->positions); i++)
    {
        if(htbl->table[i].id != 0 && htbl->table[i].id != vacated_mem.id)
        {
            printf("slot[%lld] = %lld\t", i,htbl->table[i].id);
            printf("%-30s\t\n",htbl->table[i].name);
        }
        else
            printf("slot[%lld] = vacated slot\n", i);
    }
}

int ohtbl_init(OHTBL* htbl, __uint64 positions)
{
    htbl->threshold = 0.7;
    htbl->positions = positions*4;
    
    if( (htbl->table = (STUDENT*)calloc(htbl->positions,sizeof(STUDENT)) ) == NULL)
        return ERROR;
    return NO_ERROR;
}

__uint64 ohtbl_insert(OHTBL* htbl, STUDENT* data)
{
    __uint64 position;
    __uint64 err;

    if( ohtbl_load_factor(htbl) >= htbl->threshold )
        _ohtbl_resizing(htbl);

    err = ohtbl_lookup(htbl,data->id);
    if(err != NO_DATA && err != NOT_FOUND)
    {
        return FOUND;
    }

    for(__uint64 i=0; i<htbl->positions; i++)
    {
        position = ( _hash1(data->id) + (i * _hash2(data->id)) ) % htbl->positions;

        if(htbl->table[position].id == 0 || htbl->table[position].id == vacated_mem.id)
        {
            htbl->table[position] = *data;
            htbl->size++;
            return position;    //넣은 위치
        }
    }
    return ERROR;
}

__uint64 obtbl_remove(OHTBL* htbl, __uint64 remove_id)
{
    __uint64 position;

    for(__uint64 i=0; i<htbl->positions; i++)
    {
        position = ( _hash1(remove_id) + (i * _hash2(remove_id)) ) % htbl->positions;

        if(htbl->table[position].id == 0)
        {
            return NO_DATA;
        }
        else if(htbl->table[position].id == vacated_mem.id)
        {
            continue;
        }
        else if(!_match(htbl->table[position].id, remove_id))
        {
            htbl->table[position].id = vacated_mem.id;
            htbl->size--;
            return position;
        }
    }
    return ERROR;
}

int ohtbl_lookup(OHTBL* htbl, __uint64 search_id)
{
    __uint64 position;

    for(__uint64 i=0; i<htbl->positions; i++)
    {
        position = ( _hash1(search_id) + (i * _hash2(search_id)) ) % htbl->positions;

        if(htbl->table[position].id == 0)
        {
            return NO_DATA;
        }
        else if(!_match(htbl->table[position].id, search_id))
        {
            return FOUND;
        }
    }
    return NOT_FOUND;
}
