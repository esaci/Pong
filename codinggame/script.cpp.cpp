#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

int turn = 0;
class base;
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
        bool spelled, winded, agressif;
        int abase_x, abase_y;
        int ebase_x, ebase_y;
        int guard_x, guard_y;
        base &base_ennemy;
        std::string next_action;
        std::string base_action;
        entity(std::vector<int> &tab, base &be): spelled(false), winded(false), agressif(true), base_ennemy(be){
            int i = 0;id = tab[i++];type = tab[i++];x = tab[i++];y = tab[i++];shield_life = tab[i++];is_controlled = tab[i++];health = tab[i++];vx = tab[i++];vy = tab[i++];near_base = tab[i++];threat_for = tab[i++];ebase_x = tab[i++];ebase_y = tab[i++];abase_x = tab[i++];abase_y = tab[i++];
        }
        int     dist(entity *arg){
            if (!arg)
                return (10000000);
            return (sqrt(pow(arg->x - x,2) + pow(arg->y - y,2)));
        }
        int     dist_base(int base_x, int base_y, entity *arg){
            return (sqrt(pow(arg->x - base_x, 2) + pow(arg->y - base_y, 2)));
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
        void    fill_tab_cibles(std::vector<entity *> &tab_cibles, std::map<int, std::map<int, std::vector<entity *>> > &map_map_cibles){
            tab_cibles.clear();
            std::cerr << "-------------------\nFill mon tab_cibles:\n";
            for(std::map<int, std::map<int, std::vector<entity *> > >::iterator it = map_map_cibles.begin();it != map_map_cibles.end();it++){
                for(std::map<int, std::vector<entity *> >::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {
                    for(std::vector<entity *>::iterator it3 = it2->second.begin(); it3 != it2->second.end();it3++)
                    {
                        std::cerr << (*it3)->id << " Entity:\nDistance :" << it->first << std::endl;
                        tab_cibles.push_back(*it3);
                    }
                }
            }
        }
        bool is_threat(base *arg, entity *heros, int rayon);
        void    spellTo(entity *cible, std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, int &mana){
                int dist_he = -1, dist_heb, dist_e = -1, dist_eb, dist_ha = -1,dist_hab ,buff = 0, ennemy_zone = 3000;
                bool jj;
                entity *he, *ha, *e;

                if (turn > 30)
                    ennemy_zone = 5000;
                int small_dist = -1, small_b_dist = -1, buff_dist, buff_b_dist;
                for (std::map<int, std::vector<entity *> >::iterator it = tab_e.begin(); it != tab_e.end(); it++)
                {
                    for (std::vector<entity *>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                    {
                        // (*it2)->x += (*it2)->vx, (*it2)->y += (*it2)->vy;
                        buff_dist = dist(*it2), buff_b_dist = dist_base(base_x, base_y, *it2);
                        (*it2)->x -= (*it2)->vx, (*it2)->y -= (*it2)->vy;
                        if ((*it2)->type == 2 && dist_he == -1 && !(*it2)->shield_life)
                            dist_he = buff_dist,dist_heb = buff_b_dist, he = *it2;
                        else if ((*it2)->type == 2 && ((buff_dist + buff_b_dist) < (dist_he + dist_heb)) && buff_dist < 2200)
                            dist_he = buff_dist,dist_heb = buff_b_dist, he = *it2;
                        if ((*it2)->type == 1 && *it2 != this && dist_ha == -1 && !(*it2)->shield_life && !(*it2)->spelled)
                            dist_ha = buff_dist,dist_hab = buff_b_dist, ha = *it2;
                        else if ((*it2)->type == 1 && *it2 != this && !(*it2)->shield_life && !(*it2)->spelled && ((buff_dist + buff_b_dist) < (dist_ha + dist_hab)) && buff_dist < 2200)
                            dist_ha = buff_dist,dist_hab = buff_b_dist, ha = *it2;
                        if (!(*it2)->type && dist_e != -1 && !(*it2)->shield_life && !(*it2)->winded && *it2 != cible)
                            dist_e = dist(*it2), e = *it2, dist_eb = buff_b_dist;
                        else if (!(*it2)->type && !(*it2)->shield_life && !(*it2)->winded && ((buff_dist + buff_b_dist) < (dist_e + dist_eb)) && *it2 != cible)
                            dist_e = dist(*it2), e = *it2, dist_eb = buff_b_dist;
                    }
                }
                dist_he = dist_he == -1 ? 100000 : dist_he, dist_ha = (dist_ha == -1 ? 100000 : dist_ha), dist_e = dist_e == -1 ? 100000 : dist_e;
                if ((dist_ha > 2200 && dist_e > 2200 && dist_he > ennemy_zone))
                    return (moveTo(cible, tab_e, base_x, base_y, buff));
                jj = ebase_x == base_x ? true : false;
                base_x = ebase_x == base_x ? 17630 - base_x : base_x;
                base_y = ebase_y == base_y ? 9000 - base_y : base_y;
                if (dist_he < 2200 && he->spelled == false && !(he)->shield_life && turn > 180)
                {
                    next_action.assign("SPELL CONTROL " + std::to_string(he->id) + " " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                    he->spelled = true;
                }
                else if (dist_ha < 2000 && dist_he < 10000 && he->dist(ha) < 2200 && ha->spelled == false)
                {
                    next_action.assign("SPELL SHIELD " + std::to_string(ha->id));
                    ha->spelled = true;
                }
                else if (dist_he < 2200 && he->spelled == false && !(he)->shield_life && turn > 180)
                {
                    next_action.assign("SPELL CONTROL " + std::to_string(he->id) + " " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                    he->spelled = true;
                }
                else if (dist_e < 2200 && e->spelled == false && e != cible && e->is_threat(&base_ennemy, this, 300) == true && e->threat_for != 2)
                {
                    next_action.assign("SPELL CONTROL " + std::to_string(e->id) + " " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                    e->spelled = true;
                }
                else if (!shield_life && dist_he < ennemy_zone && spelled == false && jj == false)
                {
                    next_action.assign("SPELL SHIELD " + std::to_string(id));
                    spelled = true;
                }
                else if (sqrt(pow(x - ebase_x, 2) + pow(y - ebase_y, 2)) < 400)
                    return ;
                else
                    return (moveTo(cible, tab_e, base_x, base_y, buff));
                mana -= 10;
        }
        void    moveTo(entity *cible, std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, int &mana){
            // if (mana >= 10 && (dist(cible) < 800 || (cible->is_not_threat(&base_ennemy, 2600)) == false && dist(cible) < 2000 && mana >= 30))
            if (mana >= 10 && (/* dist(cible) < 800  *//* ||  */sqrt(pow(cible->x - ebase_x,2) + pow(cible->y - ebase_y,2)) < 800))
                return (spellTo(cible, tab_e, base_x, base_y, mana));
            // if (!cible->is_not_threat(&base_ennemy, turn > 100 ? ((turn - 100) * 10 + 100) : 100))
                // return (spellTo(cible, tab_e, ebase_x, ebase_y, mana));
            std::map<int, std::map<int, std::vector<entity *> > > map_map_cibles;
            int close_x = !base_x ? -200 : 200;
            int close_y = !base_y ? -100 : 100;
            // map_map_cibles[0][dist_base(basea_x, basea_y, cible)].push_back(cible);
            for (std::map<int, std::vector<entity *> >::iterator it = tab_e.begin(); it != tab_e.end(); it++)
            {
                for (std::vector<entity *>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {
                    if (cible->dist(*it2) > 1600 || (*it2)->type)
                        continue;
                    map_map_cibles[(cible->dist(*it2)/100)][cible->dist(*it2) + dist_base(base_x, base_y, *it2)].push_back(*it2);
                }
            }
            bool baricentre = false;
            std::vector<entity *> tab_cibles;
            int xi, yi, varx, vary, v_x, v_y;
            for (fill_tab_cibles(tab_cibles, map_map_cibles); baricentre == false && tab_cibles.size(); fill_tab_cibles(tab_cibles, map_map_cibles)){
                    xi = 0, yi = 0, v_x = 0, v_y = 0;
                    for (std::vector<entity *>::iterator it3 = tab_cibles.begin(); it3 < tab_cibles.end(); it3++)
                        xi += (*it3)->x, yi += (*it3)->y, v_x += (*it3)->vx, v_y += (*it3)->vy;
                    if (tab_cibles.size())
                        xi /= (int)tab_cibles.size(), yi /= (int)tab_cibles.size(), v_x /= (int)tab_cibles.size(), v_y /= (int)tab_cibles.size();
                    varx = xi, vary = yi;
                    if (sqrt(pow(x - varx, 2) + pow(y - vary, 2)) < 800 || tab_cibles.size() == 1)
                        baricentre = true;
                    else
                    {
                        (map_map_cibles.rbegin()->second).rbegin()->second.erase(--(map_map_cibles.rbegin()->second).rbegin()->second.end());
                        if (!(map_map_cibles.rbegin()->second).rbegin()->second.size())
                            (map_map_cibles.rbegin()->second).erase(--((map_map_cibles.rbegin()->second).end()));
                        if (!(map_map_cibles.rbegin()->second.size()))
                            (map_map_cibles.erase(--(map_map_cibles.end())));
                    }
            }
            varx = std::max(1, varx), varx = std::min(17629, varx), vary = std::max(1, vary), vary = std::min(8999, vary);
            if (mana >= 10 && sqrt(pow(varx - x,2) + pow(vary - y, 2)) < 400)
                return (spellTo(cible, tab_e, base_x, base_y, mana));
            std::cerr << "Pour lui " << cible->id << " il ya " << tab_cibles.size() << " cibles !" << std::endl;
            if (sqrt(pow(varx - x,2) + pow(vary - y, 2)) > 800 || tab_cibles.size() <= 1)
            {
                varx += (v_x + close_x), vary += (v_y + close_y); 
            }
            varx = std::max(1, varx), varx = std::min(17629, varx), vary = std::max(1, vary), vary = std::min(8999, vary);
            // if (v_x > 17300 ||  v_y > 9000)
                // std::cerr << "COMMENT CA2 " << v_x<< ", " << v_y << std::endl;
            // if (agressif == false)
                // varx += ebase_x != 0 ? 400 : -400, vary += ebase_y != 0 ? 400 : -400;
            // if (varx > 17300 ||  vary > 9000)
                // std::cerr << "COMMENT CA3 " << varx<< ", " << vary << std::endl;
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
        bool close_to_guard(entity *arg){
            bool res = sqrt(pow(arg->x - guard_x, 2) + pow(arg->y - guard_y, 2)) < 6000;
            return (res);
        }
        void    erase_around(std::map<int, std::vector<entity *> > &map_entity, int rayon){
            for (std::map<int, std::vector<entity *> >::iterator it = map_entity.begin(); it != map_entity.end();){
                for (std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();)
                {
                    if (!(*it2)->type && !(*it2)->shield_life && dist(*it2) < rayon)
                    {
                        (it->second).erase(it2);
                        it2 = (it->second).begin();
                    }
                    else
                        it2++;
                }
                if (!it->second.size())
                {
                    map_entity.erase(it);
                    it = map_entity.begin();
                }
                else
                    it++;
            }
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
        int max_x, max_y;
        int flag0_x, flag0_y, flag0_xn, flag0_yn;
        int flag1_x, flag1_y, flag1_xn, flag1_yn;
        int flag2_x, flag2_y, flag2_xn, flag2_yn;
        int flag3_x, flag3_y, flag3_xn, flag3_yn;
        int health;
        int mana;
        int uusafe_dist_per_damage;
        int usafe_dist_per_damage;
        int safe_dist_per_damage;
        int max_def;
        std::map<int, std::vector<entity*> > order_defense;
        std::map<int, std::vector<entity*> > full_map;
        std::map<int, std::vector<entity*> >  order_heros;
        std::map<int, int> map_flag;
        std::vector<std::string > name;
    public:
        base(int x, int y): base_x(x), base_y(y), mana(0), uusafe_dist_per_damage(100), usafe_dist_per_damage(300), safe_dist_per_damage(200), max_def(12000){
            max_x = !x ? 6000: x - 6000;
            max_y = !y ? 2000 : y - 2000;
            flag0_x = !x ? 2000 : x - 2000;
            flag0_y = !y ? 6000 : y - 6000;
            flag0_xn = flag0_x;
            flag0_yn = flag0_y;
            flag1_x = max_x;
            flag1_y = max_y;
            flag1_xn = flag1_x;
            flag1_yn = flag1_y;
            flag2_x = !x ? 17630 - 8000 : 8000;
            flag2_y = !y ? 9000 - 2000 : 2000;
            flag2_xn = flag2_x;
            flag2_yn = flag2_y;
            flag3_x = !x ? 17630 - 4000 : 4000;
            flag3_y = !y ? 9000 - 2000 : 2000 ;
            flag3_xn = flag3_x;
            flag3_yn = flag3_y;
            name.push_back("Mortimer");
            name.push_back("Randolph");
            name.push_back("J_J");
            ebase_x = 17630 - base_x,ebase_y = 9000 - base_y;
        }
        void sethm(int h, int m){health = h; mana += m;}
        
        int     dist(entity *arg){
            if (!arg)
                return (10000000);
            return (sqrt(pow(arg->x - base_x,2) + pow(arg->y - base_y,2)));
        }

        void add_to_def(entity *arg){
            if (!arg->type && dist(arg) < max_def)
                order_defense[dist(arg)].push_back(arg);
            else if (arg->type == 1)
                order_heros[sqrt((pow((arg->x - base_x), 2))+ (pow((arg->y - base_y), 2)))- 300].push_back(arg);
            if (arg->id == 71)
                std::cerr << "Entity 71 dist_b: " << dist(arg) << "\nThreat ? " << arg->is_threat(this, *(order_heros.begin()->second.begin()), safe_dist_per_damage) << std::endl;
            full_map[dist(arg)].push_back(arg);
        }
        bool toward_b(entity *arg){
            return (sqrt(pow((arg->x + arg->vx) - base_x, 2) + pow((arg->y + arg->vy) - base_y, 2)) < dist(arg)); 
        }
        void    clear_o_d(){
            // std::cerr << "Il y a " << taille() << " cibles !" << std::endl;
            safe_dist_per_damage = (turn < 100 ? 200 : 200 + ((turn - 100) * 5));
            usafe_dist_per_damage = 100;

            int oldtaille = taille(), ntaille;
            if (!oldtaille)
                return(full_comportement());
            entity *first_heros = *(order_heros.begin()->second.begin());
            entity *second_heros = (++(order_heros.begin()->second.begin()) != (order_heros.begin()->second.end()) ? *(++(order_heros.begin()->second.begin())) : *((++order_heros.begin())->second.begin()));
            entity *third_heros = (((--order_heros.end())->second.back()));
            std::map<int, std::vector<entity *> > temp = order_defense;
            for(std::map<int, std::vector<entity*> >::iterator it = order_defense.begin(); it != order_defense.end();)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();){
                    if (dist(*it2) > 12000)
                        std::cerr << (*it2)->id << " Entity est trop loin !!" << std::endl;
                    if (((*it2)->is_threat(this, first_heros, safe_dist_per_damage) == false && (*it2)->health> 2))
                    {
                        it->second.erase(it2);
                        it2 = it->second.begin();
                    }
                    else
                        it2++;
                }
                if (!it->second.size())
                {
                    order_defense.erase(it);
                    it = order_defense.begin();
                }
                else
                    it++;
            }
            order_defense = !taille() ? temp : order_defense;
            ntaille = taille();
            entity *first_elem = *(order_defense.begin()->second.begin());
            if ((first_elem)->is_threat(this, first_heros, usafe_dist_per_damage) == true)
                return(full_comportement());
            // if (!(first_elem)->is_not_threat(this, (turn > 100 ? ((turn - 100) * 5 + 50) : 50)))
                // return;
            int small_dist = -1, small_b_dist = -1, buff_dist, buff_b_dist;
            if (taille() == 1)
            {
                order_heros.clear();
                if (first_heros->dist(first_elem) < second_heros->dist(first_elem))
                    order_heros[0].push_back(first_heros), order_heros[1].push_back(second_heros), order_heros[2].push_back(third_heros);
                else
                    order_heros[0].push_back(second_heros), order_heros[1].push_back(first_heros), order_heros[2].push_back(third_heros);
                if (oldtaille != ntaille){
                    entity *first = *(temp.begin()->second.begin());
                    entity *first_heros = *(order_heros[1].begin());
                    for(std::map<int, std::vector<entity *> >::iterator it = temp.begin(); it != temp.end();it++)
                    {
                        for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++){
                            buff_dist = first_heros->dist(*it2), buff_b_dist = dist(*it2);
                            if (small_dist == -1 && *it2 != first_elem)
                                small_dist = buff_dist, small_b_dist = buff_b_dist, first = *it2;
                            else if ((buff_dist + buff_b_dist) < (small_dist + small_b_dist))
                                small_dist = buff_dist, small_b_dist = buff_b_dist, first = *it2;
                        }
                    }
                    order_defense.begin()->second.push_back(first);
                }
                return(full_comportement());
            }
            full_comportement();
            entity *first = *(temp.begin()->second.begin());
            entity *second = (++(temp.begin()->second.begin()) != (temp.begin()->second.end()) ? *(++(temp.begin()->second.begin())) : *((++temp.begin())->second.begin()));
            order_defense.clear();
            small_dist = -1, small_b_dist = -1;
            int buff_dist2, buff_b_dist2;
            int small_dist2 = -1, small_b_dist2 = -1;
            std::map<int, entity *> map_heros1;
            std::map<int, entity *> map_heros2;
            for (std::map<int, std::vector<entity *> >::iterator itt = temp.begin(); itt != temp.end();itt++)
            {
                for (std::vector<entity *>::iterator it2 = itt->second.begin(); it2 < itt->second.end();it2++)
                {
                    buff_dist = first_heros->dist(*it2), buff_b_dist = dist(*it2);
                    buff_dist2 = second_heros->dist(*it2), buff_b_dist2 = dist(*it2);
                    if (first_heros->close_to_guard(*it2) == true){
                    if (map_heros1.find(buff_dist) != map_heros1.end())
                        map_heros1[buff_dist] = dist(map_heros1[buff_dist]) > buff_b_dist ? (*it2) : map_heros1[buff_dist]; 
                    else
                        map_heros1[buff_dist] = *it2;
                    }
                    if (second_heros->close_to_guard(*it2) == true){
                        if (map_heros2.find(buff_dist2) != map_heros2.end())
                            map_heros2[buff_dist2] = dist(map_heros2[buff_dist2]) > buff_b_dist2 ? (*it2) : map_heros2[buff_dist2]; 
                        else
                            map_heros2[buff_dist2] = *it2;
                    }
                }
            }
            first = (map_heros1.begin()->second != map_heros2.begin()->second ? map_heros1.begin()->second : NULL); 
            if (!first)
            {
                if (map_heros1.size() != 1 && first_heros->dist(map_heros1.begin()->second) > second_heros->dist(map_heros1.begin()->second))
                    first = (++map_heros1.begin())->second;
                else if (first_heros->dist(map_heros1.begin()->second) <= second_heros->dist(map_heros1.begin()->second))
                {
                    first = map_heros1.begin()->second;
                    map_heros2.erase(map_heros2.begin());
                }
            }
            if (first)
                order_defense[10].push_back(first);
            if (map_heros2.begin() != map_heros2.end())
                order_defense[11].push_back(map_heros2.begin()->second);
        }
        int    tailleh( void ){
            return (2);
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

            for(std::map<int, std::vector <entity *> >::iterator ih = order_heros.begin(); ih != order_heros.end(); ih++)
            {
                for(std::vector<entity *>::iterator ih3 = ih->second.begin(); ih3 != ih->second.end(); ih3++, i++)
                {
                    if (*ih3 == *ih2)
                        flag = i;
                }
            }
            if (map_flag.find((*ih2)->id) != map_flag.end() && map_flag[(*ih2)->id] + flag > 1  && map_flag[(*ih2)->id] + flag < 4)
                map_flag.clear();
            if (map_flag.find((*ih2)->id) != map_flag.end())
                flag = (map_flag[(*ih2)->id] + flag) == 1 ? map_flag[(*ih2)->id] : flag;
            map_flag[(*ih2)->id] = flag;
            if (flag == 2 && turn >= 100 && sqrt(pow(flag3_xn - (*ih2)->x, 2) + pow(flag3_yn - (*ih2)->y, 2)) < 1600)
                flag3_xn += max_x >= base_x ? -800 : 800, flag3_yn += max_y >= base_y ? 0 : 0;
            if (flag == 2 && turn < 100)
                res = "MOVE " + std::to_string(flag2_xn) + " " + std::to_string(flag2_yn), (*ih2)->guard_x = flag2_xn, (*ih2)->guard_y = flag2_yn;
            else if (flag == 2)
                res = "MOVE " + std::to_string(flag3_xn) + " " + std::to_string(flag3_yn), (*ih2)->guard_x = flag3_xn, (*ih2)->guard_y = flag3_yn;
            else if (flag == 1)
                res = "MOVE " + std::to_string(flag1_xn) + " " + std::to_string(flag1_yn), (*ih2)->guard_x = flag1_xn, (*ih2)->guard_y = flag1_yn;
            else
                res = "MOVE " + std::to_string(flag0_xn) + " " + std::to_string(flag0_yn), (*ih2)->guard_x = flag0_xn, (*ih2)->guard_y = flag0_yn;
            return (res);
        }

        void    full_comportement( void ){
            int limit_ronde = 2000;

            // if (turn < 100)
                // limit_ronde = 5000;
            for(std::map<int, std::vector<entity *> >::iterator ih = order_heros.begin(); ih != order_heros.end();ih++)
            {
            for(std::vector<entity *>::iterator ih2 = ih->second.begin(); ih2 !=  ih->second.end();ih2++)
                (*ih2)->next_action.assign(comportement(ih2));
            if (flag0_xn - flag0_x > limit_ronde || flag0_x - flag0_xn > limit_ronde)
                flag0_xn = flag0_x, flag0_yn = flag0_y;
            if (flag1_xn - flag1_x > limit_ronde || flag1_x - flag1_xn > limit_ronde)
                flag1_xn = flag1_x, flag1_yn = flag1_y;
            if (flag2_xn - flag2_x > limit_ronde || flag2_x - flag2_xn > limit_ronde)
                flag2_xn = flag2_x, flag2_yn = flag2_y;
            if (flag3_xn - flag3_x > limit_ronde || flag3_x - flag3_xn > limit_ronde)
                flag3_xn = flag3_x, flag3_yn = flag3_y;

        }
        }
        void    go_farm(entity *arg){
            // name[2] += " 1";
            entity *first = *(full_map.begin()->second.begin());
            int     group = first->grouped(full_map, 1600), dist_e = sqrt(pow(first->x - ebase_x, 2) + pow(first->y - ebase_y, 2)), group2, dist2, max_dist = 12000;
            for(std::map<int, std::vector<entity*> >::iterator it = full_map.begin(); it != full_map.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                {
                    group2 = (*it2)->grouped(full_map, 1600);
                    dist2 = sqrt(pow((*it2)->x - ebase_x, 2) + pow((*it2)->y - ebase_y, 2));
                    if (!(*it2)->type && dist2 < max_dist && (group2 > group || (dist_e > dist2 && group == group2) && ((*it2)->threat_for != 2 || turn < 100)))
                    {
                        first = *it2;
                        group = group2;
                        dist_e = dist2;
                    }
                }
            }
            if (!first->type && (first->threat_for != 2 || turn < 100) && dist_e < max_dist)
                return (arg->moveTo(first, full_map, ebase_x, ebase_y, mana));
            // name[2] += " 0";
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
                    if ((*it2)->type != 1 && !(*it2)->shield_life && arg->dist(*it2) < 3000)
                        map_toj[(*it2)->base_ennemy.dist(*it2)].push_back(*it2);
                }
            }
            if ((mana < 30 && turn < 200) || turn < 100 || !map_toj.size())
                return (go_farm(arg));
            if ((*(map_toj.begin()->second.begin()))->is_threat(&arg->base_ennemy, arg, 1000) == false)
                return;
            temp = map_toj;
            for(;map_toj.size() && mana >= 10;){
                entity *elem = (*(map_toj.begin()->second.begin()));

                if (elem->type || elem->shield_life || elem->health < 10 || arg->dist(elem) > 2200)
                {
                    map_toj.begin()->second.erase(map_toj.begin()->second.begin());
                    if (!map_toj.begin()->second.size())
                        map_toj.erase(map_toj.begin());
                    continue;
                }
                // else if (arg->dist(elem) < 1200 && ((mana + elem->health * 10)) >= 400 && elem->winded == false)
                // {
                    // std::cerr << "VER 1 WIND" << std::endl;
                    // arg->next_action.assign("SPELL WIND " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0)), elem->spelled = true, elem->winded = true;
                // }
                else if (arg->dist(elem) < 1200  && elem->is_threat(&arg->base_ennemy, arg, 600) == true && elem->winded == false)
                {
                    std::cerr << "VER 2 WIND" << std::endl;
                    arg->next_action.assign("SPELL WIND " + std::to_string(base_x == 0 ? 17600 : 100) + " " + std::to_string(base_y == 0 ? 8900 : 100)), elem->spelled = true, elem->winded = true;
                }
                // else if (elem->threat_for == 2  && ((mana + elem->health * 10)) >= 400 && !arg->shield_life && arg->spelled == false)
                // {
                    // std::cerr << "VER 1 SHIELD" << std::endl;
                    // arg->next_action.assign("SPELL SHIELD " + std::to_string(elem->id)), elem->spelled = true;
                // }
                else if (elem->threat_for == 2  && elem->is_threat(&arg->base_ennemy, arg, 300) == true && !arg->shield_life && arg->spelled == false)
                {
                    std::cerr << "VER 2 SHIELD" << std::endl;
                    arg->next_action.assign("SPELL SHIELD " + std::to_string(elem->id)), elem->spelled = true;
                }
                else if (arg->dist(elem) < 1900 && elem->type == 2 && elem->grouped(full_map, 2000) && arg->spelled == false && turn > 180)
                    arg->next_action.assign("SPELL CONTROL " + std::to_string(elem->id) + " " + std::to_string(base_x != 0 ? 17580 : 50) + " " + std::to_string(base_y != 0 ? 8950 : 50)), elem->spelled = true;
                else
                {
                    map_toj.begin()->second.erase(map_toj.begin()->second.begin());
                    if (!map_toj.begin()->second.size())
                        map_toj.erase(map_toj.begin());
                    continue;
                }
                mana -= 10;
                return;
            }
            map_toj = temp;
            for(;map_toj.size() && mana >= 10;)
            {
                entity *elem = (*(map_toj.begin()->second.begin()));
                if (arg->dist(elem) < 1900 && elem->type == 2 && elem->check_pres(map_toj, 1400) && map_toj.begin()->first > 3000 && arg->spelled == false)
                    arg->next_action.assign("SPELL CONTROL " + std::to_string(elem->id) + " " + std::to_string(base_x != 0 ? 17630 : 0) + " " + std::to_string(base_y != 0 ? 9000 : 0)), arg->spelled = true;
                else
                {
                    map_toj.begin()->second.erase(map_toj.begin()->second.begin());
                    if (!map_toj.begin()->second.size())
                        map_toj.erase(map_toj.begin());
                    continue;
                }
                mana -= 10;
                return;
            }
            for(std::map<int, std::vector<entity*> >::iterator it = temp.begin(); it != temp.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                {
                    if (!(*it2)->type && !(*it2)->shield_life && (((mana + (*it2)->health * 10)) >= 300) || !(*it2)->is_threat(&(*it2)->base_ennemy, arg, 600))
                    {
                        return (arg->moveTo((*it2), temp, ebase_x, ebase_y, mana));
                    }
                }
            }
            return (go_farm(arg));
        }
        void    define_next_actions( void ){
            std::map<int, std::vector<entity*> >::iterator ih = order_heros.begin();
            std::map<int, std::vector<entity*> >::iterator ie;
            entity *elem;
    
            for(int i = 0, varx, vary; ih != order_heros.end(); ih++)
            {
                for(std::vector<entity *>::iterator ih2 = ih->second.begin(); ih2 != ih->second.end(); ih2++, i++)
                {
                    if (map_flag.find((*ih2)->id) != map_flag.end() && map_flag[(*ih2)->id] == 2)
                    {
                        j_j(*ih2);
                        continue;
                    }
                    if(!taille())
                        continue;
                    ie = order_defense.begin();
                    elem = (*(ie->second.begin()));
                    int dist_e = (*ih2)->dist(elem);
                    int dist_b = dist(elem);

                    if (elem->is_threat(this, (*ih2), safe_dist_per_damage) == false || elem->shield_life || mana < 10)
                        (*ih2)->moveTo(elem, full_map, base_x, base_y, mana);
                    else if (dist_e < 1200)
                    {
                        (*ih2)->next_action.assign("SPELL WIND " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0)), elem->spelled = true, elem->winded = true;
                        mana -= 10;
                    }
                    else if (dist_e < 2200 && (dist_b < dist(*ih2)) && elem->spelled == false && toward_b((*ih2)) && dist(*ih2) < 800)
                    {
                        std::cerr << "CONTROL DEFINE " << (*ih2)->id << std::endl;
                        (*ih2)->next_action.assign("SPELL CONTROL " + std::to_string(elem->id) + " " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0)), elem->spelled = true;
                        mana -= 10;
                    }
                    else
                        (*ih2)->moveTo(elem, full_map, base_x, base_y, mana);
                    if (elem->winded == true){
                        (*ih2)->erase_around(order_defense, 2200);
                    }
                    else if (elem->is_threat(this, (*ih2), usafe_dist_per_damage) == false || elem->health < 3)
                    {
                        ie->second.erase(ie->second.begin());
                        if (!ie->second.size())
                            order_defense.erase(order_defense.begin());
                    }
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

bool    entity::is_threat(base *arg, entity *heros,int case_degat){
    int dist_total = arg->dist(this) - ((arg->dist(heros)/2));
    bool temp = (health >= (dist_total / case_degat));
    // std::cerr << id << " entity rayon: " << case_degat << "\n Base :(" << arg->base_x << ", " << arg->base_y << ") \nDistance/case_degat :" << arg->dist(this) / case_degat << "\n hp: " << health << "\nRenvoie : " << temp << "\n ------------------------" << std::endl;
   return (temp);
}

int main()
{
    int x, y;
    int heroes_per_player; // Always 3
    int entity_count; // Amount of heros and monsters you can see
    entity *tmp;
    std::vector<int> tab;

    tab.reserve(30);
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
            tab[x++] = base_ally.ebase_x, tab[x++] = base_ally.ebase_y;tab[x++] = base_ally.base_x, tab[x++] = base_ally.base_y;
            tmp = new entity(tab, base_enemy);
            base_ally.add_to_def(tmp);
        }
        base_ally.clear_o_d();
        base_ally.define_next_actions();
        for (int i = 0, a = 1; i < heroes_per_player; i++) {
            base_ally.do_next_action();
        }
        base_ally.full_clear();
        turn++;
    }
}