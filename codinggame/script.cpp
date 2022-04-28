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
        int v_x, v_y;
        int near_base; // 0=monster with no target yet, 1=monster targeting a base
        int threat_for; // Given this monster's trajectory, is it a threat to 1=your base, 2=your opponent's base, 0=neither
        bool spelled, winded, agressif, danger;
        int abase_x, abase_y;
        int ebase_x, ebase_y;
        int guard_x, guard_y;
        int varx, vary;
        int pot_baricentre;
        base &base_ennemy;
        std::string next_action;
        std::string base_action;
        entity(std::vector<int> &tab, base &be): spelled(false), winded(false), agressif(true), danger(false), base_ennemy(be), varx(0), vary(0), pot_baricentre(0){
            int i = 0;id = tab[i++];type = tab[i++];x = tab[i++];y = tab[i++];shield_life = tab[i++];is_controlled = tab[i++];health = tab[i++];vx = tab[i++];vy = tab[i++];near_base = tab[i++];threat_for = tab[i++];ebase_x = tab[i++];ebase_y = tab[i++];abase_x = tab[i++];abase_y = tab[i++];
        }
        int     dist(entity *arg){
            if (!arg)
                return (10000000);
            return (sqrt(pow(arg->x - x,2) + pow(arg->y - y,2)));
        }
        int     dist_v(entity *arg){
            if (!arg)
                return (10000000);
            return (sqrt(pow(arg->x + arg->vx - x,2) + pow(arg->y + arg->vy - y ,2)));
        }
        int     dist_base(int base_x, int base_y, entity *arg){
            return (sqrt(pow(arg->x - base_x, 2) + pow(arg->y - base_y, 2)));
        }
        int grouped(std::map<int, std::vector<entity *> > &full_map, int rayon){
            int res = 0;
            for (std::map<int, std::vector<entity* > >::iterator it = full_map.begin(); it != full_map.end();it++){
                for (std::vector<entity *>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {
                    if (!(*it2)->type && dist(*it2) < rayon && this != *it2 &&  (*it2)->threat_for == 2 && !(*it2)->shield_life)
                        res++; 
                }
            }
            return (res);
        }
        void    fill_tab_cibles(std::vector<entity *> &tab_cibles, std::map<int, std::map<int, std::vector<entity *>> > &map_map_cibles){
            tab_cibles.clear();
            // std::cerr << "-------------------\nFill mon tab_cibles:\n";
            for(std::map<int, std::map<int, std::vector<entity *> > >::iterator it = map_map_cibles.begin();it != map_map_cibles.end();it++){
                for(std::map<int, std::vector<entity *> >::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {
                    for(std::vector<entity *>::iterator it3 = it2->second.begin(); it3 != it2->second.end();it3++)
                    {
                        // std::cerr << (*it3)->id << " Entity:\nDistance :" << it->first << std::endl;
                        tab_cibles.push_back(*it3);
                    }
                }
            }
        }
        void my_baricentre(std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, base *arg, entity *heros);
        bool is_threat(base *arg, entity *heros, int rayon);
        void    spellTo(entity *cible, std::map<int, std::vector<entity *> > &tab_e, int vbase_x, int vbase_y, int &mana, base *arg){
                int dist_he = -1, dist_heb, dist_e = -1, dist_eb, dist_ha = -1,dist_hab ,buff = 0;
                bool jj;
                entity *he, *ha, *e;

                int small_dist = -1, small_b_dist = -1, buff_dist, buff_b_dist;
                for (std::map<int, std::vector<entity *> >::iterator it = tab_e.begin(); it != tab_e.end(); it++)
                {
                    for (std::vector<entity *>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                    {
                        // (*it2)->x += (*it2)->vx, (*it2)->y += (*it2)->vy;
                        buff_dist = dist(*it2), buff_b_dist = dist_base(vbase_x, vbase_y, *it2);
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
                jj = ebase_x == vbase_x ? true : false;
                vbase_x = ebase_x == vbase_x ? 17630 - vbase_x : vbase_x;
                vbase_y = ebase_y == vbase_y ? 9000 - vbase_y : vbase_y;
                if (dist_ha < 2000 && dist_base(vbase_x, vbase_y,he) < 10000 && ha->spelled == false)
                {
                    std::cerr << "SHIELD DE SPELLTO " << std::endl;
                    next_action.assign("SPELL SHIELD " + std::to_string(ha->id));
                    ha->spelled = true;
                }
                else if (dist_he < 2200 && he->spelled == false && !(he)->shield_life && !jj)
                {
                    std::cerr << "CONTROL DE SPELLTO " << std::endl;
                    next_action.assign("SPELL CONTROL " + std::to_string(he->id) + " " + std::to_string(!vbase_x ? 17630 : 0) + " " + std::to_string(vbase_y == 0 ? 9000 : 0));
                    he->spelled = true;
                }
                else if (dist_e < 2200 && e->spelled == false && e != cible && cible->dist(e) > 800 && e->threat_for != 2)
                {
                    std::cerr << "CONTROL2 DE SPELLTO " << std::endl;
                    next_action.assign("SPELL CONTROL " + std::to_string(e->id) + " " + std::to_string(!vbase_x ? 17630 : 0) + " " + std::to_string(vbase_y == 0 ? 9000 : 0));
                    e->spelled = true;
                }
                else if (!shield_life && dist_base(vbase_x, vbase_y,he) < 10000 && spelled == false && jj == false)
                {
                    std::cerr << "SHIELD2 DE SPELLTO " << std::endl;
                    next_action.assign("SPELL SHIELD " + std::to_string(id));
                    spelled = true;
                }
                else
                    return (moveTo(cible, tab_e, vbase_x, vbase_y, buff, arg));
                mana -= 10;
        }
        void    moveTo(entity *cible, std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, int &mana, base *arg);
        int     size_baricentre(std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, base *arg, entity *heros);

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
            bool res = (sqrt(pow(arg->x - guard_x, 2) + pow(arg->y - guard_y, 2)) < 1200 || dist_base(abase_x, abase_y, arg) <= 8000);
            // if (res == true)
                // std::cerr << arg->id << "est dans le rayon d'action de " << id << "\nIl est a " << dist(arg) << "de la base" << std::endl;
            return (res);
        }
        int dist_to_guard(entity *arg){
            int res = sqrt(pow(arg->x - guard_x, 2) + pow(arg->y - guard_y, 2));
            // std::cerr << "IDC:ID:GX:GY:res "<< arg->id << " : " << id << " : " << guard_x << " : " << guard_y << " : " << res << std::endl;
            return (res);
        }

        void    erase_around(base *arg, std::map<int, std::vector<entity *> > &map_entity, int rayon){
            // std::cerr << this->id << " Entity declenche erase_around" << std::endl;
            for (std::map<int, std::vector<entity *> >::iterator it = map_entity.begin(); it != map_entity.end();){
                for (std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();)
                {
                    // if (id == 0 && !(*it2)->type)
                        // std::cerr << (*it2)->id << " Monstre est dans la boucle a erase !\nDist: " << dist(*it2) << std::endl;
                    if (!(*it2)->type && !(*it2)->shield_life && dist(*it2) < rayon && (*it2)->is_threat(arg, this, 100) == false)
                    {
                        (it->second).erase(it2);
                        it2 = (it->second).begin();
                    }
                    else if (!(*it2)->type && dist(*it2) < rayon && (*it2)->is_threat(arg, this, 100) == true)
                            (*it2)->winded = true, (*it2)->spelled = true, it2++;
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
        std::map<int, std::map<int, std::vector<entity *> > > m_m_c;
        std::vector<entity *> t_c;
    public:
        base(int x, int y): base_x(x), base_y(y), mana(0), uusafe_dist_per_damage(100), usafe_dist_per_damage(300), safe_dist_per_damage(200), max_def(10000){
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
            if (!arg->type && arg->dist_base(base_x, base_y, arg) < 15000)
                order_defense[dist(arg)].push_back(arg);
            else if (arg->type == 1)
                order_heros[sqrt((pow((arg->x - base_x), 2))+ (pow((arg->y - base_y), 2)))- 300].push_back(arg);
            full_map[dist(arg)].push_back(arg);
        }
        bool toward_b(entity *arg){
            return (sqrt(pow((arg->x + arg->vx) - base_x, 2) + pow((arg->y + arg->vy) - base_y, 2)) <= dist(arg)); 
        }
        void    clear_o_d(){
            safe_dist_per_damage = 100 + turn;
            usafe_dist_per_damage = 150;

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
                    if ((*it2)->is_threat(this, first_heros, safe_dist_per_damage) == false)
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
            int small_dist = -1, small_b_dist = -1, buff_dist, buff_b_dist;
            if (taille() == 1)
            {
                order_heros.clear();
                if (first_heros->dist(first_elem) < second_heros->dist(first_elem))
                    order_heros[0].push_back(first_heros), order_heros[1].push_back(second_heros), order_heros[2].push_back(third_heros);
                else
                    order_heros[0].push_back(second_heros), order_heros[1].push_back(first_heros), order_heros[2].push_back(third_heros);
                first_heros = *(order_heros.begin()->second.begin());
                second_heros = (++(order_heros.begin()->second.begin()) != (order_heros.begin()->second.end()) ? *(++(order_heros.begin()->second.begin())) : *((++order_heros.begin())->second.begin()));
                third_heros = (((--order_heros.end())->second.back()));
                if (oldtaille != ntaille){
                    entity *first = NULL;
                    for(std::map<int, std::vector<entity *> >::iterator it = temp.begin(); it != temp.end();it++)
                    {
                        int summ, sum;
                        for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++){
                            buff_dist = second_heros->dist(*it2), buff_b_dist = dist(*it2),  sum = buff_dist + buff_b_dist;
                            if (small_dist == -1 && *it2 != first_elem)
                                small_dist = buff_dist, small_b_dist = buff_b_dist, first = *it2, summ = buff_dist + buff_b_dist;
                            else if (sum < summ && *it2 != first_elem)
                                small_dist = buff_dist, small_b_dist = buff_b_dist, first = *it2, summ = buff_dist + buff_b_dist;
                        }
                    }
                    if (first)
                        order_defense.begin()->second.push_back(first);
                }
                return(full_comportement());
            }
            first_heros = *(order_heros.begin()->second.begin());
            second_heros = (++(order_heros.begin()->second.begin()) != (order_heros.begin()->second.end()) ? *(++(order_heros.begin()->second.begin())) : *((++order_heros.begin())->second.begin()));
            third_heros = (((--order_heros.end())->second.back()));
            entity *first /* = *(temp.begin()->second.begin()) */;
            entity *second /* = (++(temp.begin()->second.begin()) != (temp.begin()->second.end()) ? *(++(temp.begin()->second.begin())) : *((++temp.begin())->second.begin())) */;
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
                    buff_dist = first_heros->dist(*it2), buff_b_dist = dist(*it2) * 2 + first_heros->dist_to_guard(*it2);
                    buff_dist2 = second_heros->dist(*it2), buff_b_dist2 = dist(*it2) * 2 + second_heros->dist_to_guard(*it2);
                    if (first_heros->close_to_guard(*it2) == true){
                        if (map_heros1.find(buff_b_dist) != map_heros1.end())
                            map_heros1[buff_b_dist] = dist(map_heros1[buff_b_dist]) > buff_dist ? (*it2) : map_heros1[buff_b_dist]; 
                        else
                            map_heros1[buff_b_dist] = *it2;
                    }
                    if (second_heros->close_to_guard(*it2) == true){
                        if (map_heros2.find(buff_b_dist2) != map_heros2.end())
                            map_heros2[buff_b_dist2] = dist(map_heros2[buff_b_dist2]) > buff_dist2 ? (*it2) : map_heros2[buff_b_dist2]; 
                        else
                            map_heros2[buff_b_dist2] = *it2;
                    }
                }
            }
            first = NULL;
            if (map_heros1.size()){
                first = (!map_heros2.size() || map_heros1.begin()->second != map_heros2.begin()->second ? map_heros1.begin()->second : NULL); 
                if (!first){
                    if (first_heros->dist(map_heros1.begin()->second) <= second_heros->dist(map_heros1.begin()->second)){
                        first = map_heros1.begin()->second;
                        map_heros1.erase(map_heros1.begin());
                        map_heros2.erase(map_heros2.begin());
                    }
                    else{
                        map_heros1.erase(map_heros1.begin());
                        first = map_heros1.begin() != map_heros1.end() ? map_heros1.begin()->second : NULL;
                    }
                }
                if (first)
                    order_defense[10].push_back(first);
            }
            if (map_heros2.begin() != map_heros2.end())
                order_defense[11].push_back(map_heros2.begin()->second);
            if (!first && map_heros2.size()){
                order_heros.clear();
                order_heros[0].push_back(second_heros), order_heros[1].push_back(first_heros), order_heros[2].push_back(third_heros);
            }
            full_comportement();
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
            if (flag == 2 && (turn < 100 || mana < 100))
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
        void    go_farm(entity *arg, entity *pot_heros){
            // name[2] += " 1";
            entity *first = NULL;
            entity *second = NULL;
            int     max_pot_bari = 0; 
            int     group = 0, group2, dist2, sum_dist, max_dist = 10000, dist_e = max_dist - 1, dist_pb = max_dist - 1;
            for(std::map<int, std::vector<entity*> >::iterator it = full_map.begin(); it != full_map.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                {
                    dist2 = arg->dist_base(ebase_x, ebase_y, *it2);
                    if (((*it2)->shield_life && (*it2)->threat_for == 2) || (*it2)->type || (*it2)->threat_for == 2 || dist2 >= max_dist)
                        continue;
                    sum_dist = dist2 + arg->dist(*it2) - ((*it2)->health * 100);
                    group2 = (*it2)->size_baricentre(full_map, ebase_x, ebase_y, this, arg);
                    if ((group2 > group || (dist_e > sum_dist && group == group2) && ((*it2)->threat_for != 2 || turn < 100)))
                    {
                        first = *it2;
                        group = group2;
                        dist_e = sum_dist;
                    }
                    if (group > 1)
                        max_pot_bari = 0;
                    else if (max_pot_bari < (*it2)->pot_baricentre || (dist_pb > sum_dist && max_pot_bari == (*it2)->pot_baricentre) && ((*it2)->threat_for != 2 || turn < 100))
                    {
                        max_pot_bari = (*it2)->pot_baricentre;
                        second = (*it2);
                        dist_pb = sum_dist;
                    }

                }
            }
            if (first)
                std::cerr << "1 - Le mob a farm est " << first->id << " !" << std::endl;
            if (second)
                std::cerr << "2 - Le mob a farm est " << second->id << " !" << max_pot_bari << std::endl;
            if (max_pot_bari && second && !second->type && (second->threat_for != 2 || turn < 100))
                return (arg->moveTo(second, full_map, ebase_x, ebase_y, mana, this));
            else if (first && !first->type)
                return (arg->moveTo(first, full_map, ebase_x, ebase_y, mana, this));
        }
        void    j_j(entity *arg){
            std::map<int, std::vector<entity *> > map_toj, temp;
            std::vector<entity *> close_heros;
            entity *pot_heros;
            name[2] = "J_J";
            for(std::map<int, std::vector<entity*> >::iterator it = full_map.begin(); it != full_map.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                {
                    if ((*it2) != arg && !(*it2)->shield_life && arg->dist_base(ebase_x, ebase_y, (*it2)) < 12000)
                        map_toj[(*it2)->base_ennemy.dist(*it2)].push_back(*it2);
                    if ((*it2)->type == 2)
                        close_heros.push_back(*it2);
                }
            }
            if ((mana < 30 && turn < 200) || !map_toj.size())
                return (go_farm(arg, arg));
            temp = map_toj;
            for(;map_toj.size() && mana >= 10;){
                entity *elem = (*(map_toj.begin()->second.begin()));

                pot_heros = arg;
                if (close_heros.size() > 1)
                {
                    int sum_dist, min_sum_dist = -1;
                    for(std::vector<entity*>::iterator it = close_heros.begin(); it != close_heros.end();it++)
                    {
                        sum_dist = (*it)->dist(elem) + arg->dist_base(ebase_x, ebase_y, *it);
                        pot_heros = pot_heros == arg ? *it : pot_heros, min_sum_dist = min_sum_dist == -1 ? sum_dist : min_sum_dist;
                        if (sum_dist < min_sum_dist)
                            pot_heros = *it;
                    }
                }
                if (elem->shield_life || arg->dist(elem) > 2200)
                {
                    map_toj.begin()->second.erase(map_toj.begin()->second.begin());
                    if (!map_toj.begin()->second.size())
                        map_toj.erase(map_toj.begin());
                    continue;
                }
                else if (!elem->type && elem->threat_for == 2  && elem->is_threat(&arg->base_ennemy, pot_heros, 110) == true && !arg->shield_life && arg->dist_base(ebase_x, ebase_y, elem) < 4800)
                {
                    std::cerr << "VER 2 SHIELD" << std::endl;
                    arg->next_action.assign("SPELL SHIELD " + std::to_string(elem->id)), elem->spelled = true;
                }
                else if (!elem->type && arg->dist(elem) < 1200  &&  arg->dist_base(ebase_x, ebase_y, elem) < 7000 && elem->is_threat(&arg->base_ennemy, pot_heros, 400) == true)
                {
                    std::cerr << "VER 2 WIND sur " << elem->id << " a une distance " << arg->dist(elem) << std::endl;
                    arg->next_action.assign("SPELL WIND " + std::to_string(!base_x ? 17600 : 100) + " " + std::to_string(base_y == 0 ? 8900 : 100)), elem->spelled = true, elem->winded = true;
                }
                if (elem->type == 1 && arg->dist(elem) < 1400)
                    arg->next_action.assign("SPELL SHIELD " + std::to_string(elem->id));
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
            // if (turn < 80)
                // return (go_farm(arg, arg));
            map_toj = temp;
            for(;map_toj.size() && mana >= 10;)
            {
                entity *elem = (*(map_toj.begin()->second.begin()));
                if (!elem->type && arg->dist(elem) < 2200  && (elem->is_threat(&arg->base_ennemy, pot_heros, 200) == true || elem->is_threat(this, order_heros.begin()->second.back(), 400) == true) && elem->threat_for != 2)
                {
                    std::cerr << "VER 1 CONTROL" << std::endl;
                    arg->next_action.assign("SPELL CONTROL " + std::to_string(elem->id) + " " + std::to_string(!base_x ? 17600 : 100) + " " + std::to_string(base_y == 0 ? 8900 : 100)) + " SPELL 1 ", elem->spelled = true;
                }
                else if (elem->type == 2 && arg->dist(elem) < 2200 && elem->grouped(full_map, 1600) > ((220 - turn)/50) && arg->spelled == false)
                {
                    std::cerr << "VER 2 CONTROL" << std::endl;
                    arg->next_action.assign("SPELL CONTROL " + std::to_string(elem->id) + " " + std::to_string(!base_x ? 0 : 17600) + " " + std::to_string(!base_y ? 0 : 9000))+ " SPELL 2 ", elem->spelled = true;
                }
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
                    if (!(*it2)->type && !(*it2)->is_threat(&(*it2)->base_ennemy, arg, 200) && (*it2)->dist_base(ebase_x, ebase_y, *it2) < 12000)
                    {
                        return (arg->moveTo((*it2), temp, ebase_x, ebase_y, mana, this));
                    }
                }
            }
            return (go_farm(arg, arg));
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
                    std::cerr << "La cible de " << (*ih2)->id << " est " << elem->id << std::endl;
                    if ((dist_b > 3000 && elem->is_threat(this, (*ih2), 250) == false) || elem->shield_life || mana < 10)
                        (*ih2)->moveTo(elem, full_map, base_x, base_y, mana, this);
                    else if (dist_e < 1280 && elem->is_threat(this, (*ih2), 110) == true || dist_b <= 3000)
                    {
                        (*ih2)->next_action.assign("SPELL WIND " + std::to_string(!base_x ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0)), elem->spelled = true, elem->winded = true;
                        mana -= 10;
                    }
                    else if (dist_e < 2200 && (dist_b < dist(*ih2)) && elem->spelled == false && toward_b((*ih2)) && dist_b < 400){
                        (*ih2)->next_action.assign("SPELL CONTROL " + std::to_string(elem->id) + " " + std::to_string(!base_x ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0)), elem->spelled = true;
                        mana -= 10;
                    }
                    else
                        (*ih2)->moveTo(elem, full_map, base_x, base_y, mana, this);
                    // if (elem->winded == true && elem->is_threat(this, (*ih2), 110) == false){
                        // (*ih2)->erase_around(this, order_defense, 2000);
                    // }
                    if (elem->is_threat(this, (*ih2), 250) == false || elem->health < 3)
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
    int dist_total = arg->dist(this) - ((arg->dist(heros)/2)), c_health = health > 2 ? health : 2;
    if (shield_life)
        c_health /= 2;
    c_health = c_health % 2 ? c_health + 1 : c_health;
    dist_total = (dist_total / case_degat);
    dist_total = !dist_total ? 2 : dist_total;
    dist_total = dist_total % 2 ? dist_total + 1 : dist_total;
    bool temp = (c_health >= dist_total);
    // std::cerr << id << " entity rayon: " << case_degat << "\n Base :(" << arg->base_x << ", " << arg->base_y << ") \nDistance/case_degat :" << arg->dist(this) / case_degat << "\n hp: " << health << "\nRenvoie : " << temp << "\n ------------------------" << std::endl;
   return (temp);
}

void    entity::moveTo(entity *cible, std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, int &mana, base *arg){
    int close_x = !base_x  ? -200 : 200;
    int close_y = !base_y  ? -100 : 100;
    close_y = cible->threat_for != 1 ? 0 : close_y, close_x = cible->threat_for != 2 ? 0 : close_x;
    if (base_x == ebase_x)
        close_x = (!base_x ? -200 : 200), close_x = cible->threat_for != 2 ? 0 : close_x;
    if (base_y == ebase_y)
        close_y = (!base_y ? -100 : 100), close_y = cible->threat_for != 2 ? 0 : close_y;
    
    int bar_size = cible->size_baricentre(tab_e, base_x, base_y, arg, this);
    cible->varx = std::max(1, cible->varx), cible->varx = std::min(17629, cible->varx), cible->vary = std::max(1, cible->vary), cible->vary = std::min(8999, cible->vary);
    if (mana >= 10 && (sqrt(pow(cible->varx - x,2) + pow(cible->vary - y,2)) < 200))
        return (spellTo(cible, tab_e, base_x, base_y, mana, arg));
    if (mana >= 10 && bar_size < 2 && (sqrt(pow(cible->varx - x,2) + pow(cible->vary - y,2)) < 800))
        return (spellTo(cible, tab_e, base_x, base_y, mana, arg));
    if (sqrt(pow(cible->varx - x,2) + pow(cible->vary - y, 2)) > 1600 || arg->t_c.size() <= 1)
        cible->varx += (v_x), cible->vary += (v_y);
    cible->varx = std::max(1, cible->varx), cible->varx = std::min(17629, cible->varx), cible->vary = std::max(1, cible->vary), cible->vary = std::min(8999, cible->vary);
    next_action.assign("MOVE " + std::to_string(cible->varx) + " " + std::to_string(cible->vary));
}

int entity::size_baricentre(std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, base *arg, entity *heros){
    my_baricentre(tab_e, base_x, base_y, arg, heros);
    if (sqrt(pow(varx - heros->x, 2) + pow(vary - heros->y, 2)) > 800)
        return (0);
    return (arg->t_c.size());
}

void entity::my_baricentre(std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, base *arg, entity *heros){
    std::map<int, std::map<int, std::vector<entity *> > > &map_map_cibles = arg->m_m_c;
    std::vector<entity *> &tab_cibles = arg->t_c;
    int xi, yi;

    // std::cerr << 
    v_x = 0, v_y = 0, varx = 0, vary = 0;
    tab_cibles.clear(), map_map_cibles.clear();
    for (std::map<int, std::vector<entity *> >::iterator it = tab_e.begin(); it != tab_e.end(); it++)
    {
        for (std::vector<entity *>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            if (this->dist(*it2) > 1600 || (*it2)->type)
                continue;
            map_map_cibles[(this->dist(*it2)/100)][this->dist(*it2) + dist_base(base_x, base_y, *it2)].push_back(*it2);
        }
    }
    fill_tab_cibles(tab_cibles, map_map_cibles);
    pot_baricentre = tab_cibles.size();
    for (;tab_cibles.size(); fill_tab_cibles(tab_cibles, map_map_cibles)){
            xi = 0, yi = 0, v_x = 0, v_y = 0;
            for (std::vector<entity *>::iterator it3 = tab_cibles.begin(); it3 < tab_cibles.end(); it3++)
                xi += (*it3)->x, yi += (*it3)->y, v_x += (*it3)->vx, v_y += (*it3)->vy;
            if (tab_cibles.size())
                xi /= (int)tab_cibles.size(), yi /= (int)tab_cibles.size(), v_x /= (int)tab_cibles.size(), v_y /= (int)tab_cibles.size();
            varx = xi, vary = yi;
            if (sqrt(pow(heros->x - varx, 2) + pow(heros->y - vary, 2)) < 800 || tab_cibles.size() == 1)
                break;
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