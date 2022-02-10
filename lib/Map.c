//
// Created by emad on 10/02/2022.
//

#include "Functions.h"
#include "Perimeters.h"

#include "Map.h"

// checking if islands intersect
int ISLAND_COLLIDE(int x1, int y1, int x2,int y2){
    int dis=(ISLAND_SIZE+LOGO_SIZE)/2;
    if( (x1<x2+dis && x1>x2-dis) && (y1<y2+dis && y1>y2-dis) ) return 1;
    return 0;
}

// generating a random map based
struct Map MAP_GENERATOR(int islandCnt, int playerCnt){
    struct Map res;

    //// NOTE:     PLAYER IDs are 1-BASED
    res.playerCnt=playerCnt;
    for(int i=1;i<=playerCnt;i++){
        res.playerList[i].islandCnt=0;
        res.playerList[i].potion=0;
        res.playerList[i].troopCnt=DEFAULT_TROOPS;
        // other things?
    }

    res.islandCnt=islandCnt;
    for(int i=0;i<islandCnt;i++){

        // randomizing coords
        int x,y;
        int isok=0;
        while(!isok){
            isok=1;
            x=RAND(0,GAME_WIDTH-ISLAND_SIZE);
            y=RAND(0,GAME_HEIGHT-ISLAND_SIZE);
            for(int j=0;j<i;j++) {
                if (ISLAND_COLLIDE(x, y, res.islandList[j].x, res.islandList[j].y)) {
                    isok = 0;
                    break;
                }
            }
        }

        res.islandList[i].shape= RAND(0,MAX_SHAPE);
        res.islandList[i].x=x;
        res.islandList[i].y=y;
        res.islandList[i].isSelected=0;
        res.islandList[i].owner=0;
        res.islandList[i].capacity=RAND(MIN_CAPACITY, MAX_CAPACITY);
        res.islandList[i].troopsCount=DEFAULT_TROOPS;

        if(playerCnt>=1 && res.playerList[1].islandCnt==0){
            if(x<GAME_WIDTH/2 && y<GAME_HEIGHT/2){
                res.playerList[1].islandCnt=1;
                res.islandList[i].owner=1;
            }
        }
        if(playerCnt>=2 && res.playerList[2].islandCnt==0){
            if(x>=GAME_WIDTH/2 && y>=GAME_HEIGHT/2){
                res.playerList[2].islandCnt=1;
                res.islandList[i].owner=2;
            }
        }
        if(playerCnt>=3 && res.playerList[3].islandCnt==0){
            if(x>=GAME_WIDTH/2 && y<GAME_HEIGHT/2){
                res.playerList[3].islandCnt=1;
                res.islandList[i].owner=3;
            }
        }
        if(playerCnt>=4 && res.playerList[4].islandCnt==0){
            if(x<GAME_WIDTH/2 && y>=GAME_HEIGHT/2){
                res.playerList[4].islandCnt=1;
                res.islandList[i].owner=4;
            }
        }
    }
    res.troopCnt=0;
    res.campaignCnt=0;
    res.potionCnt=0;
    res.selectedIsland=-1;
    res.frameNo=0;
    return res;
}

// checking if a map is ok
int MAP_CHECK(struct Map map){
    for(int i=1;i<=map.playerCnt;i++){
        int res=0;
        for(int j=0;j<map.islandCnt;j++){
            if(map.islandList[j].owner==i) res=1;
        }
        if(!res) return 0;
    }
    return 1;
}

// A few map based functions

void ADD_TROOP(struct Troop t, struct Map *map){
    map->troopList[map->troopCnt]=t;
    map->troopCnt++;
}

void DESTROY_TROOP(int ind, struct Map *map){
    map->playerList[map->troopList[ind].owner].troopCnt--;
    map->troopList[ind]=map->troopList[map->troopCnt-1];
    map->troopCnt--;
}

void ADD_CAMPAIGN(struct Campaign c, struct Map *map){
    map->campaignList[map->campaignCnt]=c;
    map->campaignCnt++;
}

void DESTROY_CAMPAIGN(int ind, struct Map *map){
    map->campaignList[ind]=map->campaignList[map->campaignCnt-1];
    map->campaignCnt--;
}

void ADD_POTION(struct Potion p, struct Map *map){
    map->potionList[map->potionCnt]=p;
    map->potionCnt++;
}

void DESTROY_POTION(int ind, struct Map *map){
    map->potionList[ind]=map->potionList[map->potionCnt-1];
    map->potionCnt--;
}

// adding a new campaign to map
void NEW_CAMPAIGN(int st,int fin, struct Map *map){
    struct Campaign c;
    c.owner=map->islandList[st].owner;
    c.xStart=map->islandList[st].x+ISLAND_SIZE/2;
    c.yStart=map->islandList[st].y+ISLAND_SIZE/2;
    c.xEnd=map->islandList[fin].x+ISLAND_SIZE/2;
    c.yEnd=map->islandList[fin].y+ISLAND_SIZE/2;
    c.count=map->islandList[st].troopsCount;
    c.dest=fin;
    c.frame=(map->frameNo%FRAME_PER_OUT);
    map->islandList[st].troopsCount=0;
    ADD_CAMPAIGN(c, map);
}

////////////////////////////////// Game based functions

// handling clicking islands during the game
void CLICKED(int x,int y, struct Map *map){
    if(map->selectedIsland==-1){
        for(int i=0;i<map->islandCnt;i++){
            if(COLLIDE(x,y,0,0,map->islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2,map->islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2,LOGO_SIZE,LOGO_SIZE)){
                if(map->islandList[i].owner==USERID){
                    map->islandList[i].isSelected=1;
                    map->selectedIsland=i;
                }
                break;
            }
        }
    }
    else{
        for(int i=0;i<map->islandCnt;i++){
            if(COLLIDE(x,y,0,0,map->islandList[i].x+(ISLAND_SIZE-LOGO_SIZE)/2,map->islandList[i].y+(ISLAND_SIZE-LOGO_SIZE)/2,LOGO_SIZE,LOGO_SIZE)){
                if(map->selectedIsland!=i){
                    NEW_CAMPAIGN(map->selectedIsland, i, map);
                }
                map->islandList[map->selectedIsland].isSelected=0;
                map->selectedIsland=-1;
                break;
            }
        }
    }
}

///////// Sub-functions of the "MAP_UPDATE()"
void MOVE_TROOPS(int frozen, struct Map *map){
    for(int i=0;i<map->troopCnt;i++){
        if(frozen && map->playerList[map->troopList[i].owner].potion!=FREEZE_ID) continue ;
        map->troopList[i].x+=map->troopList[i].xSpeed;
        map->troopList[i].y+=map->troopList[i].ySpeed;
        if(map->playerList[map->troopList[i].owner].potion==HASTE_ID){
            map->troopList[i].x+=map->troopList[i].xSpeed;
            map->troopList[i].y+=map->troopList[i].ySpeed;
        }
    }
}

struct Campaign HANDLE_CAMPAIGN(struct Campaign c, struct Map *map){
    if(map->frameNo%FRAME_PER_OUT==c.frame || (map->playerList[c.owner].potion==HASTE_ID && map->frameNo%(FRAME_PER_OUT/2)==c.frame%(FRAME_PER_OUT/2))){
        if(c.count>1){
            struct Troop t;
            t.owner=c.owner;
            float dx=c.xEnd-c.xStart,dy=c.yEnd-c.yStart;
            float k=sqrt((dx*dx+dy*dy)/(1.0*TROOP_SPEED*TROOP_SPEED));
            t.xSpeed=dx/k;
            t.ySpeed=dy/k;
            t.dest=c.dest;
            t.x=c.xStart-t.ySpeed*(1.0*TROOP_DIS/TROOP_SPEED)-TROOP_SIZE/2;
            t.y=c.yStart+t.xSpeed*(1.0*TROOP_DIS/TROOP_SPEED)-TROOP_SIZE/2;
            ADD_TROOP(t, map);
            c.count--;
            t.x=c.xStart+t.ySpeed*(1.0*TROOP_DIS/TROOP_SPEED)-TROOP_SIZE/2;
            t.y=c.yStart-t.xSpeed*(1.0*TROOP_DIS/TROOP_SPEED)-TROOP_SIZE/2;
            ADD_TROOP(t, map);
            c.count--;
        }
        else if(c.count==1){
            struct Troop t;
            t.owner=c.owner;
            t.x=c.xStart-TROOP_SIZE/2;
            t.y=c.yStart-TROOP_SIZE/2;
            float dx=c.xEnd-c.xStart,dy=c.yEnd-c.yStart;
            float k=sqrt((dx*dx+dy*dy)/(1.0*TROOP_SPEED*TROOP_SPEED));
            t.xSpeed=dx/k;
            t.ySpeed=dy/k;
            t.dest=c.dest;
            ADD_TROOP(t, map);
            c.count--;
        }
    }
    return c;
}

void PRODUCE_TROOPS(struct Map *map){
    for(int i=0;i<map->islandCnt;i++){
        if(map->islandList[i].owner==0) continue ;
        if(map->frameNo%FRAME_PER_PROD==0 || (map->playerList[map->islandList[i].owner].potion==WARCRY_ID && map->frameNo%(FRAME_PER_PROD/2)==0)){
            if(map->islandList[i].troopsCount<map->islandList[i].capacity){
                map->islandList[i].troopsCount++;
                map->playerList[map->islandList[i].owner].troopCnt++;
            }
        }
    }
}

void GENERATE_POTION(struct Map *map){
    if(map->potionCnt<MAX_POTION && RAND(0,POTION_CHANCE)==0){
        int aInd=RAND(0, map->islandCnt), bInd=RAND(0, map->islandCnt);
        if(aInd==bInd){
            bInd=(aInd+1)%map->islandCnt;
        }
        struct Potion p;
        p.x=(map->islandList[aInd].x+map->islandList[bInd].x+ISLAND_SIZE-POTION_WIDTH)/2;
        p.y=(map->islandList[aInd].y+map->islandList[bInd].y+ISLAND_SIZE-POTION_HEIGHT)/2;
        p.type=RAND(1, 5);
        ADD_POTION(p, map);
    }
}

void COLLISION_CHECK(struct Map *map){
    // collision check (island to troop)
    for(int i=0;i<map->troopCnt;i++){
        int dest=map->troopList[i].dest;
        if(COLLIDE(map->islandList[dest].x+(ISLAND_SIZE-LOGO_SIZE)/2, map->islandList[dest].y+(ISLAND_SIZE-LOGO_SIZE)/2, LOGO_SIZE, LOGO_SIZE, map->troopList[i].x, map->troopList[i].y, TROOP_SIZE, TROOP_SIZE)){
            if(map->islandList[dest].owner==map->troopList[i].owner || map->playerList[map->islandList[dest].owner].potion==POACH_ID){
                map->islandList[dest].troopsCount++;
                map->playerList[map->islandList[dest].owner].troopCnt++;
            }
            else{
                map->islandList[dest].troopsCount--;
                map->playerList[map->islandList[dest].owner].troopCnt--;
                if(map->islandList[dest].troopsCount<0){
                    map->playerList[map->islandList[dest].owner].islandCnt--;
                    map->playerList[map->troopList[i].owner].islandCnt++;
                    map->islandList[dest].owner=map->troopList[i].owner;
                    map->islandList[dest].troopsCount=1;
                    map->playerList[map->troopList[i].owner].troopCnt++;
                }
            }
            DESTROY_TROOP(i, map);
        }
    }
    // collision check (potion to troop)
    for(int i=0;i<map->potionCnt;i++){
        for(int j=0;j<map->troopCnt;j++){
            if(COLLIDE(map->potionList[i].x, map->potionList[i].y, POTION_WIDTH, POTION_HEIGHT, map->troopList[j].x, map->troopList[j].y, TROOP_SIZE, TROOP_SIZE)){
                if(map->playerList[map->troopList[j].owner].potion==0){
                    map->playerList[map->troopList[j].owner].potion=map->potionList[i].type;
                    map->playerList[map->troopList[j].owner].potionLeft=POTION_LEN[map->potionList[i].type];
                    DESTROY_POTION(i, map);
                }
            }
        }
    }
    // collide check (troop to troop)
    for(int i=0;i<map->troopCnt;i++){
        for(int j=0;j<map->troopCnt;j++){
            if(i!=j && map->troopList[i].owner!=map->troopList[j].owner && COLLIDE(map->troopList[i].x, map->troopList[i].y, TROOP_SIZE, TROOP_SIZE, map->troopList[j].x, map->troopList[j].y, TROOP_SIZE, TROOP_SIZE)){
                DESTROY_TROOP(i, map);
                if(j==map->troopCnt) DESTROY_TROOP(i, map);
                else DESTROY_TROOP(j, map);
            }
        }
    }
}

void PLAYER_UPDATE(struct Map *map){
    for(int i=1;i<=map->playerCnt;i++){
        if(map->playerList[i].potion==0){
            continue ;
        }
        map->playerList[i].potionLeft--;
        if(map->playerList[i].potionLeft<=0){
            map->playerList[i].potion=0;
            map->playerList[i].potionLeft=0;
        }
    }
}

// decides to play for the bot <id>
void AI(int id, struct Map *map){
    for(int i=0;i<map->islandCnt;i++){
        if(map->frameNo < 2000) break;
        if(map->islandList[i].owner!=id) continue ;
        if(map->islandList[i].troopsCount>=map->islandList[i].capacity){
            int min=INF, ind=-1;
            for(int j=0;j<map->islandCnt;j++){
                if(map->islandList[j].owner==id) continue ;
                if(map->islandList[j].troopsCount<min){
                    min=map->islandList[j].troopsCount;
                    ind=j;
                }
            }
            NEW_CAMPAIGN(i, ind, map);
            return ;
        }
    }
    for(int i=0;i<map->islandCnt;i++){
        if(map->islandList[i].owner!=id || map->islandList[i].troopsCount<10) continue;
        int min=INF, ind=-1;
        for(int j=0;j<map->islandCnt;j++){
            if(map->islandList[j].owner==id) continue;
            if(DISTANCE(map->islandList[i].x, map->islandList[i].y, map->islandList[j].x, map->islandList[j].y)<min){
                min=DISTANCE(map->islandList[i].x, map->islandList[i].y, map->islandList[j].x, map->islandList[j].y);
                ind=j;
            }
        }
        if(map->islandList[ind].troopsCount < map->islandList[i].troopsCount){
            NEW_CAMPAIGN(i, ind, map);
            return ;
        }
    }
    for(int i=0;i<map->islandCnt;i++){
        if(map->frameNo < 2000) break;
        if(map->islandList[i].owner!=id || map->islandList[i].troopsCount<10) continue;
        int min=INF, ind=-1;
        for(int j=0;j<map->islandCnt;j++){
            if(map->islandList[j].owner==id || map->islandList[j].owner==USERID) continue;
            if(map->islandList[j].troopsCount > map->islandList[i].troopsCount) continue;
            if(DISTANCE(map->islandList[i].x, map->islandList[i].y, map->islandList[j].x, map->islandList[j].y)<min){
                min=DISTANCE(map->islandList[i].x, map->islandList[i].y, map->islandList[j].x, map->islandList[j].y);
                ind=j;
            }
        }
        if(ind!=-1){
            NEW_CAMPAIGN(i, ind, map);
            return ;
        }
    }
}

// updating the necessary parts of the map for each frame (movement, potion generation, troop production, islands state, Collisions, ...)
int MAP_UPDATE(struct Map *map){
    map->frameNo=(map->frameNo+1)%MAX_FRAME;

    if(map->selectedIsland!=-1  && map->islandList[map->selectedIsland].owner!=USERID){
        map->selectedIsland=-1;
    }

    for(int i=2;i<=map->playerCnt;i++){
        if(map->frameNo%((i+map->playerCnt)*FPS)==0) AI(i, map); // the "if" limits the bots intelligence
    }

    // figuring out if the map is frozen
    int frozen=0;
    for(int i=0;i<map->playerCnt;i++){
        if(map->playerList[i].potion==FREEZE_ID) frozen=1;
    }

    GENERATE_POTION(map);
    COLLISION_CHECK(map);
    MOVE_TROOPS(frozen, map);
    // handling campaigns
    for(int i=0;i<map->campaignCnt;i++){
        if(frozen && map->playerList[map->campaignList[i].owner].potion!=FREEZE_ID) continue;
        map->campaignList[i]=HANDLE_CAMPAIGN(map->campaignList[i], map);
    }
    for(int i=0;i<map->campaignCnt;i++){
        while(i<map->campaignCnt && map->campaignList[i].count<=0){
            DESTROY_CAMPAIGN(i, map);
        }
    }
    PRODUCE_TROOPS(map);
    PLAYER_UPDATE(map);
    // Checking if the game is over
    if(map->playerList[USERID].troopCnt<=0 && map->playerList[USERID].islandCnt<=0) return -1; // user has lost
    for(int i=1;i<=map->playerCnt;i++){
        if(i!=USERID && (map->playerList[i].troopCnt>0 || map->playerList[i].islandCnt>0)){
            return 0; // game is not finished
        }
    }
    return 1; // user has won
}
