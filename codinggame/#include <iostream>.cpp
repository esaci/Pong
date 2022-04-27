#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

int turn = 0;

class entity{
    public:
        int id; // Unique identifier
        int type; // 0=monster, 1=your hero, 2=opponent hero
        int x; // Position of this entity
        int y;
        int shield_life; // Ignore for this league; Count down until shield spell fades
        int is_controlled; // Ignore for this league; Equals 1 when this entity is under a control spell
        int health; // Remaining health of this monster
        int vx; // Trajectory of this monster
        int vy;
        int near_base; // 0=monster with no target yet, 1=monster targeting a base
        int threat_for; // Given this monster's trajectory, is it a threat to 1=your base, 2=your opponent's base, 0=neither
        bool spelled;
        int on_cible;
        int v_xy;
        int ebase_x;
        int ebase_y;
        std::string next_action;
        std::string base_action;
        entity(std::vector<int> &tab): spelled(false), on_cible(0), v_xy(200){
            int i = 0;id = tab[i++];type = tab[i++];x = tab[i++];y = tab[i++];shield_life = tab[i++];is_controlled = tab[i++];health = tab[i++];vx = tab[i++];vy = tab[i++];near_base = tab[i++];threat_for = tab[i++];ebase_x = tab[i++];ebase_y = tab[i++];
        }
        int     dist(entity *arg){
            if (!arg)
                return (10000000);
            return (sqrt(pow(arg->x - x,2) + pow(arg->y - y,2)));
        }
        int grouped(std::map<int, std::vector<entity *> > &full_map, int rayon){
            int res = 0;
            for (std::map<int, std::vector<entity* > >::iterator it = full_map.begin(); it != full_map.end();it++){
                for (std::vector<entity *>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {
                    if (dist(*it2) < rayon && this != *it2 && !(*it2)->type && (*it2)->threat_for != 2)
                        res++; 
                }
            }
            return (res);
        }
        void    spellTo(entity *cible, std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, int &mana){
                int dist_he = -1, dist_e = -1, dist_ha = -1, buff = 0, ennemy_zone = 3000;
                entity *he, *ha, *e;

                if (turn > 30)
                    ennemy_zone = 5000;
                for (std::map<int, std::vector<entity *> >::iterator it = tab_e.begin(); it != tab_e.end(); it++)
                {
                    if (dist_he != -1 && dist_e != -1 && dist_ha != -1)
                        break;
                    for (std::vector<entity *>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                    {
                        if (dist_he == -1 && (*it2)->type == 2)
                            dist_he = dist(*it2), he = *it2;
                        else if (*it2 != this && dist_ha == -1 && (*it2)->type == 1 && !(*it2)->shield_life)
                            dist_ha = dist(*it2), ha = *it2;
                        else if (dist_e != -1 && !(*it2)->type && dist(*it2) > 400 && !(*it2)->shield_life)
                            dist_e = dist(*it2), e = *it2;
                    }
                }
                dist_he = dist_he == -1 ? 100000 : dist_he, dist_ha = (dist_ha == -1 ? 100000 : dist_ha), dist_e = dist_e == -1 ? 100000 : dist_e;
                if (dist_ha > 2200 && dist_e > 2200 && dist_he > ennemy_zone)
                    return (moveTo(cible, tab_e, base_x, base_y, buff));
                else if (cible == this && dist_he < 2200 && !he->spelled && !(he)->shield_life)
                {
                    next_action.assign("SPELL CONTROL " + std::to_string(he->id) + " " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                    he->spelled = true;
                }
                else if (dist_ha < 2200 && dist_he < 10000 && he->dist(ha) < 2200 && !ha->spelled)
                {
                    next_action.assign("SPELL SHIELD " + std::to_string(ha->id));
                    ha->spelled = true;
                }
                else if (dist_he < 2200 && !he->spelled && !(he)->shield_life)
                {
                    next_action.assign("SPELL CONTROL " + std::to_string(he->id) + " " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                    he->spelled = true;
                }
                else if (dist_e < 2200 && !e->spelled)
                {
                    next_action.assign("SPELL CONTROL " + std::to_string(e->id) + " " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                    e->spelled = true;
                }
                else if (!shield_life && dist_he < ennemy_zone && !spelled)
                {
                    next_action.assign("SPELL SHIELD " + std::to_string(id));
                    spelled = true;
                }
                else if (sqrt(pow(x - ebase_x, 2) + pow(y - ebase_y, 2)) < 400)
                    return ;
                else
                    return (moveTo(cible, tab_e, base_x, base_y, buff));
                mana =-10;
        }
        void    moveTo(entity *cible, std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, int &mana){
            if (dist(cible) < 300 && mana > 10 || sqrt(pow(cible->x - ebase_x,2) + pow(cible->y - ebase_y,2)) < 400)
                return (spellTo(cible, tab_e, base_x, base_y, mana));
            std::vector<entity *> tab_cibles;
            tab_cibles.reserve(4);
            tab_cibles.push_back(cible);
            for (std::map<int, std::vector<entity *> >::iterator it = tab_e.begin(); it != tab_e.end(); it++)
            {
                for (std::vector<entity *>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {
                    if (*it2 == cible)
                        continue;
                    tab_cibles.push_back(*it2);
                    for (std::vector<entity *>::iterator it3 = tab_cibles.begin(); it3 < tab_cibles.end(); it3++)
                    {
                        if ((*it3)->dist(*it2) > 600){
                            tab_cibles.pop_back();
                            break;
                        }
                    }
                }
            }
            int xi = 0, yi = 0;
            if (cible->spelled)
                v_xy *= (-1);
            for (std::vector<entity *>::iterator it3 = tab_cibles.begin(); it3 < tab_cibles.end(); it3++)
                xi += (*it3)->x, yi += (*it3)->y; 
            xi /= tab_cibles.size(), yi /= tab_cibles.size();
            int varx = (base_x > xi ? v_xy : -v_xy ) + xi, vary = (base_y > yi ? v_xy: -v_xy ) + yi;
            varx = std::max(1, varx), varx = std::min(17629, varx), vary = std::max(1, vary), vary = std::min(8999, vary);
            next_action.assign("MOVE " + std::to_string(varx) + " " + std::to_string(vary));
        }

        bool    check_pres(std::map<int, std::vector<entity *> > &map_entity, int rayon){
            for (std::map<int, std::vector<entity *> >::iterator it = map_entity.begin(); it != map_entity.end(); it++){
                for (std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {
                    if (!(*it2)->type && (*it2) != this && (*it2)->threat_for == 2 && !(*it2)->shield_life)
                    {
                        if (this->dist(*it2) < rayon)
                            return (true);
                    }
                }
            }
            return (false);
        }

        private:
            entity();
};

class base{
    public:
        int base_x; // The corner of the map representing your base
        int base_y;
        int ebase_x;
        int ebase_y;
        int max_x;
        int max_x2;
        int max_jx;
        int max_y;
        int max_y2;
        int max_jy;
        int max_xn;
        int max_x2n;
        int max_jxn;
        int max_yn;
        int max_y2n;
        int max_jyn;
        int health;
        int mana;
        int limit;
        std::map<int, std::vector<entity*> > order_defense;
        std::map<int, std::vector<entity*> > full_map;
        std::map<int, std::vector<entity*> >  order_heros;
        std::map<int, int> map_flag;
        std::vector<std::string > name;
    public:
        base(int x, int y): base_x(x), base_y(y), mana(0), limit(8010){
            max_x = 3000;
            max_x2 = 5000;
            if (x != 0)
            {
                max_x = x - 3000;
                max_x2 = x - 5000;
            }
            max_y = 1500;
            max_y2 = 4500;
            if (y != 0)
            {
                max_y = y - 1500;
                max_y2 = y - 4500;
            }
            max_jx = max_x <= base_x ? 3500 : 17631 - 3500;
            max_jy = max_y <= base_y ? 3500 : 9000 - 3500;
            name.push_back("Mortimer");
            name.push_back("Randolph");
            name.push_back("J_J");
            ebase_x = 17630 - base_x,ebase_y = 9000 - base_y;
            max_xn = max_x;
            max_x2n = max_x2;
            max_jxn = max_jx;
            max_yn = max_y;
            max_y2n = max_y2;
            max_jyn = max_jy;
        }
        void sethm(int h, int m){health = h; mana += m;}
        
        int     dist(entity *arg){
            if (!arg)
                return (10000000);
            return (sqrt(pow(arg->x - base_x,2) + pow(arg->y - base_y,2)));
        }

        int to_a_b_distance(entity *arg){
			int rayonLarge = turn > 100 ? 15000 : 10000;
            int res = 0;
            if (pow(arg->vx + arg->x - base_x, 2) + pow(arg->vy + arg->y - base_y, 2) < pow(rayonLarge, 2))
                return (res);
            return(-1);
        }

        int to_b_distance(entity *arg){
            int res = 0;
			int rayonSerre = turn > 100 ? 1000 : 5000;
            if (pow(arg->vx + arg->x - base_x, 2) + pow(arg->vy + arg->y - base_y, 2) < pow(rayonSerre, 2))
                return (res);
            return(-1);
        }
        void add_to_def(entity *arg){
            int res = to_b_distance(arg);
            if (res != -1)
            {
                order_defense[dist(arg)].push_back(arg);
                return ;
            }
            res = to_a_b_distance(arg);
            if (res == -1)
                return;
            order_defense[dist(arg)].push_back(arg);
        }
        void    clear_o_d(){
            if (turn > 100)
                limit = 4500;
            if (order_defense.size() < 2)
                return ;
            for(std::map<int, std::vector<entity*> >::iterator it = order_defense.begin(); it != order_defense.end();it++)
            {
                if (it->first < limit)
                {
                    map_flag.clear();
                    return;
                }
            }
            std::map<int, std::vector<entity *> > temp = order_defense;
            entity *first = *(temp.begin()->second.begin()), *first_heros = *(order_heros.begin()->second.begin());

            order_defense.clear();
            for (std::map<int, std::vector<entity *> >::iterator itt = temp.begin(); itt != temp.end();itt++)
            {
                for (std::vector<entity *>::iterator itt2 = itt->second.begin(); itt2 < itt->second.end();itt2++)
                {
                    if (first_heros->dist(*itt2) < first_heros->dist(first))
                        first = *itt2; 
                }
            }
            order_defense[limit + 10].push_back(first);
            entity *second = (++(temp.begin()->second.begin()) != (temp.begin()->second.end()) ? *(++(temp.begin()->second.begin())) : *((++temp.begin())->second.begin()));
            entity *second_heros = (++(order_heros.begin()->second.begin()) != (order_heros.begin()->second.end()) ? *(++(order_heros.begin()->second.begin())) : *((++order_heros.begin())->second.begin()));
            for (std::map<int, std::vector<entity *> >::iterator itt = temp.begin(); itt != temp.end();itt++)
            {
                for (std::vector<entity *>::iterator itt2 = itt->second.begin(); itt2 < itt->second.end();itt2++)
                {
                    if (second_heros->dist(*itt2) < second_heros->dist(second) && *itt2 != first && (*itt2 != second || temp.size() < 3))
                        second = *itt2;
                }
            }
            order_defense[limit + 11].push_back(second);
            entity *third = *((--temp.end()->second.begin()));
            entity *third_heros = *((--order_heros.end()->second.begin()));
            for (std::map<int, std::vector<entity *> >::iterator itt = temp.begin(); itt != temp.end();itt++)
            {
                for (std::vector<entity *>::iterator itt2 = itt->second.begin(); itt2 < itt->second.end();itt2++)
                {
                    if (third_heros->dist(*itt2) < third_heros->dist(third) && *itt2 != first)
                        third = *itt2;
                }
            }
            order_defense[limit + 12].push_back(third);
        }
        int    tailleh( void ){
            int res = 0;

            for(std::map<int, std::vector<entity*> >::iterator it = order_heros.begin(); it != order_heros.end();it++)
                res += it->second.size();
            return (res);
        }

        int    taille( void ){
            int res = 0;

            for(std::map<int, std::vector<entity*> >::iterator it = order_defense.begin(); it != order_defense.end();it++)
                res += it->second.size();
            return (res);
        }

        std::string    comportement(std::vector<entity *>::iterator ih2){
            std::string res;
            int flag = 0, i = 0;
            int tab_pos[5][2] = {
            {max_x2n, max_yn}
            ,{max_x, max_y2n}
            ,{max_x2n, max_y2n}
            ,{max_jxn, max_jyn}};
            for(std::map<int, std::vector <entity *> >::iterator ih = order_heros.begin(); ih != order_heros.end(); ih++)
            {
                for(std::vector<entity *>::iterator ih3 = ih->second.begin(); ih3 != ih->second.end(); ih3++, i++)
                {
                    if (*ih3 == *ih2)
                        flag = i;
                }
            }
            // if (map_flag.find((*ih2)->id) != map_flag.end() && turn < 30)
                // flag = map_flag[(*ih2)->id];
            if (map_flag.find((*ih2)->id) != map_flag.end() && map_flag[(*ih2)->id] + flag > 1  && map_flag[(*ih2)->id] + flag < 4)
                map_flag.clear();
            if (map_flag.find((*ih2)->id) != map_flag.end())
                flag = (map_flag[(*ih2)->id] + flag) == 1 ? map_flag[(*ih2)->id] : flag;
            map_flag[(*ih2)->id] = flag;
            (*ih2)->v_xy = 200;
            if (sqrt(pow(tab_pos[flag][0] - (*ih2)->x, 2) + pow(tab_pos[flag][1] - (*ih2)->y, 2)) < 600)
                max_x2n += max_x >= base_x ? 500 : -500, max_y2n += max_y >= base_y ? 300 : -300;
            if (flag == 2 && turn < 100)
                res = "MOVE " + std::to_string(max_x2n) + " " + std::to_string(max_y2n);
            else if (flag == 2)
                res = "MOVE " + std::to_string(max_jxn) + " " + std::to_string(max_jyn);
            else if (flag == 1)
                res = "MOVE " + std::to_string(max_x) + " " + std::to_string(max_y2n);
            else
                res = "MOVE " + std::to_string(max_x2n) + " " + std::to_string(max_y);
            return (res);
        }

        void    full_comportement( void ){
            int limit_ronde = 2000;

            if (turn < 100)
                limit_ronde = 5000;
            for(std::map<int, std::vector<entity *> >::iterator ih = order_heros.begin(); ih != order_heros.end();ih++)
            {
            for(std::vector<entity *>::iterator ih2 = ih->second.begin(); ih2 !=  ih->second.end();ih2++)
            {
                (*ih2)->next_action.assign(comportement(ih2));
            }
            if (max_x2n - max_x2 > limit_ronde || max_x2 - max_x2n > limit_ronde)
                max_x2n = max_x2, max_y2n = max_y2;
        }
        }
        void    go_farm(entity *arg){
            name[2] += " FARM";
            entity *first = *(full_map.begin()->second.begin());
            int     group = first->grouped(full_map, 600), dist_e = sqrt(pow(first->x - ebase_x, 2) + pow(first->y - ebase_y, 2)), group2, dist2, max_dist = 8000;
            if (turn < 100 || mana < 100)
                max_dist = 10000;
            for(std::map<int, std::vector<entity*> >::iterator it = full_map.begin(); it != full_map.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                {
                    group2 = (*it2)->grouped(full_map, 400);
                    dist2 = sqrt(pow((*it2)->x - ebase_x, 2) + pow((*it2)->y - ebase_y, 2));
                    if (!(*it2)->type && dist2 < max_dist && (group2 > group || (dist_e > dist2 && group == group2) && (*it2)->threat_for != 2))
                    {
                        first = *it2;
                        group = group2;
                        dist_e = dist2;
                    }
                }
            }
            arg->v_xy = 100;
            if (!first->type && first->threat_for != 2 && dist_e < max_dist)
                return (arg->moveTo(first, full_map, ebase_x, ebase_y, mana));
            name[2] += " PAS";
            // int liberte = 1000;
            // if (turn > 100)
                // liberte = 800;
            // if (sqrt(pow(arg->x - max_jxn,2) + pow(arg->y - max_jyn,2)) < liberte)
                // arg->next_action.assign("MOVE " + std::to_string(9000) + " " + std::to_string(4500));
        }
        void    j_j(entity *arg){
            std::map<int, std::vector<entity *> > map_toj, temp;
            name[2] = "J_J";
            for(std::map<int, std::vector<entity*> >::iterator it = full_map.begin(); it != full_map.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                {
                    if ((*it2)->type != 1 && !(*it2)->shield_life && arg->dist(*it2) < 2200)
                        map_toj[sqrt(pow((*it2)->x - ebase_x, 2) + pow((*it2)->y - ebase_y, 2))].push_back(*it2);
                }
            }
            if ((mana < 30 && turn < 200) || turn < 100 || !map_toj.size())
                return (go_farm(arg));
            if (map_toj.begin()->first > 10000)
                return;
            temp = map_toj;
            for(;map_toj.size();){
                entity *elem = (*(map_toj.begin()->second.begin()));
                if (elem->type || elem->shield_life || elem->health < 10 || arg->dist(elem) > 2000 )
                {
                    map_toj.begin()->second.erase(map_toj.begin()->second.begin());
                    if (!map_toj.begin()->second.size())
                        map_toj.erase(map_toj.begin());
                    continue;
                }
                else if (elem->threat_for == 2 && map_toj.begin()->first < 5000 && ((mana + elem->health * 5)) >= 300)
                    arg->next_action.assign("SPELL SHIELD " + std::to_string(elem->id));
                else if (arg->dist(elem) < 1200  && elem->threat_for == 2 && ((mana + elem->health * 10)) >= 300)
                    arg->next_action.assign("SPELL WIND " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                else if (arg->dist(elem) < 1200  && map_toj.begin()->first < 5000)
                    arg->next_action.assign("SPELL WIND " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                else
                {
                    map_toj.begin()->second.erase(map_toj.begin()->second.begin());
                    if (!map_toj.begin()->second.size())
                        map_toj.erase(map_toj.begin());
                    continue;
                }
                mana =-10;
                return;
            }
            map_toj = temp;
            for(;map_toj.size();)
            {
                entity *elem = (*(map_toj.begin()->second.begin()));
                if (arg->dist(elem) < 1900 && elem->type == 2 && elem->check_pres(map_toj, 1400) && map_toj.begin()->first > 3000 && turn > 180)
                    arg->next_action.assign("SPELL CONTROL " + std::to_string(elem->id) + " " + std::to_string(base_x != 0 ? 17630 : 0) + " " + std::to_string(base_y != 0 ? 9000 : 0));
                // else if (arg->dist(elem) < 1900 && !elem->type && !elem->shield_life && elem->threat_for == 2 && map_toj.begin()->first > 800 && elem->health > (map_toj.begin()->first / 100))
                    // arg->next_action.assign("SPELL SHIELD " + std::to_string(elem->id));
                else
                {
                    map_toj.begin()->second.erase(map_toj.begin()->second.begin());
                    if (!map_toj.begin()->second.size())
                        map_toj.erase(map_toj.begin());
                    continue;
                }
                mana =-10;
                return;
            }
            for(std::map<int, std::vector<entity*> >::iterator it = temp.begin(); it != temp.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                {
                    arg->v_xy = 800;
                    if (!(*it2)->type && !(*it2)->shield_life && (*it2)->threat_for == 2)
                        return (arg->moveTo((*it2), temp, ebase_x, ebase_y, mana));
                }
            }
            return (go_farm(arg));
        }
        void    define_next_actions( void ){
            std::map<int, std::vector<entity*> >::iterator ih = order_heros.begin();
            std::map<int, std::vector<entity*> >::iterator ie;
            int v_xy;

            // if (!order_defense.size())
            for(int i = 0, varx, vary; ih != order_heros.end(); ih++)
            {
                for(std::vector<entity *>::iterator ih2 = ih->second.begin(); ih2 != ih->second.end(); ih2++, i++, v_xy = 400)
                {
                    if (turn > 0 && map_flag[(*ih2)->id] == 2)
                    {
                        j_j(*ih2);
                        continue;
                    }
                    ie = order_defense.begin();
                    if(!taille())
                        continue;
                    int dist_e = (*ih2)->dist((*(ie->second.begin())));
                    int dist_b = dist((*(ie->second.begin())));
                    // std::cerr << "Mana : " << mana << "\nDistance entre Hero " << (*ih2)->id << " et entity" << (*(ie->second.begin()))->id <<": " << dist_e << "\nEntity et Base: " << dist_b << std::endl;
                    if (dist_b > 3000)
                        (*ih2)->moveTo((*(ie->second.begin())), full_map, base_x, base_y, mana);
                    else if (dist_e < 1280 && mana > 10 && !(*(ie->second.begin()))->shield_life)
                    {
                        (*ih2)->next_action.assign("SPELL WIND " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                        mana -= 10;
                        (*(ie->second.begin()))->spelled = true;
                    }
                    else if (dist_e < 2200 && mana > 10 && !(*(ie->second.begin()))->shield_life && (*(ie->second.begin()))->health > (dist_b / 200)  && dist_b < dist(*ih2))
                    {
                        (*ih2)->next_action.assign("SPELL CONTROL " + std::to_string((*(ie->second.begin()))->id) + " " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                        mana -= 10;
                        (*(ie->second.begin()))->spelled = true;
                    }
                    // else if (dist_b >= dist(*ih2))
                        // (*ih2)->moveTo(*ih2, full_map, base_x, base_y, mana);
                    else
                        (*ih2)->moveTo((*(ie->second.begin())), full_map, base_x, base_y, mana);
                    if ((taille() + i >= tailleh() && (ie->first > limit)) || (*(ie->second.begin()))->on_cible || ((*(ie->second.begin()))->health < (ie->first / 300)))
                    {
                        (*(ie->second.begin()))->on_cible = 0;
                        ie->second.erase(ie->second.begin());
                        if (!ie->second.size())
                            order_defense.erase(order_defense.begin());
                    }
                    else
                        (*(ie->second.begin()))->on_cible++;
                }
            }
        }
        void    do_next_action( void ){
            int smaller_id = order_heros.begin()->second[0]->id; 
            std::vector<entity*>::iterator it3 = order_heros.begin()->second.begin();
            std::map<int, std::vector<entity*> >::iterator it4 = order_heros.begin();

            for(std::map<int, std::vector<entity*> >::iterator it = order_heros.begin(); it != order_heros.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                {
                    smaller_id = smaller_id > (*it2)->id ? (*it2)->id : smaller_id;
                    if (smaller_id == (*it2)->id)
                    {
                        it3 = it2;
                        it4 = it;
                    }
                }
            }
            std::cout << (*it3)->next_action << " " << name[map_flag[(*it3)->id]] << std::endl;
            it4->second.erase((it3));
            if (!it4->second.size())
                order_heros.erase(it4);
            
        }
        void    full_clear( void ){
            for(std::map<int, std::vector<entity*> >::iterator it = full_map.begin(); it != full_map.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                    delete *it2;
            }
            order_heros.clear();
            order_defense.clear();
            full_map.clear();
        }
        ~base(){full_clear();}
};

int main()
{
    int x, y;
    int heroes_per_player; // Always 3
    int entity_count; // Amount of heros and monsters you can see
    entity *tmp;
    std::map<int, std::vector<entity *> > threat_base;
    std::vector<int> tab;
    std::map<int, std::vector<entity *> >::iterator it;

    tab.reserve(20);
    std::cin >> x >> y; std::cin.ignore();
    base base_ally(x, y), base_enemy(17630 - x, 9000 - y);
    std::cin >> heroes_per_player; std::cin.ignore();
    while (1) {
        std::cin >> base_ally.health >> base_ally.mana;std::cin.ignore();
        std::cin >> base_enemy.health >> base_enemy.mana;std::cin.ignore();
        std::cin >> entity_count; std::cin.ignore();
        for (int i = 0; i < entity_count; i++) {
            x = 0;
            std::cin >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++]; std::cin.ignore();
            tab[x++] = base_ally.ebase_x, tab[x++] = base_ally.ebase_y;
            tmp = new entity(tab);

            if (!tmp->type)
                base_ally.add_to_def(tmp);
            else if (tmp->type == 1)
                base_ally.order_heros[sqrt((pow((tmp->x - base_ally.base_x), 2))+ (pow((tmp->y - base_ally.base_y), 2)))- 300].push_back(tmp);
            base_ally.full_map[base_ally.dist(tmp)].push_back(tmp);
        }
        base_ally.clear_o_d();
        base_ally.full_comportement();
        base_ally.define_next_actions();
        for (int i = 0, a = 1; i < heroes_per_player; i++) {
            base_ally.do_next_action();
        }
        base_ally.full_clear();
        turn++;
    }
}