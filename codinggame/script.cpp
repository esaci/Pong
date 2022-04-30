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
        bool spelled, winded, agressif, danger, ignore_it, someone_in_range;
        int abase_x, abase_y;
        int ebase_x, ebase_y;
        int guard_x, guard_y;
        int varx, vary;
        int pot_baricentre;
        base &base_ennemy;
        std::string next_action;
        std::string base_action;
        entity(std::vector<int> &tab, base &be): spelled(false), winded(false), agressif(true), danger(false), ignore_it(false), someone_in_range(false), base_ennemy(be), varx(0), vary(0), pot_baricentre(0){
            int i = 0;id = tab[i++];type = tab[i++];x = tab[i++];y = tab[i++];shield_life = tab[i++];is_controlled = tab[i++];health = tab[i++];vx = tab[i++];vy = tab[i++];near_base = tab[i++];threat_for = tab[i++];ebase_x = tab[i++];ebase_y = tab[i++];abase_x = tab[i++];abase_y = tab[i++];
        }
        entity(entity const &arg): base_ennemy(arg.base_ennemy){
            id = arg.id;
            type = arg.type;
            x = arg.x;
            y = arg.y;
            shield_life = arg.shield_life;
            is_controlled = arg.is_controlled;
            health = arg.health;
            vx = arg.vx;
            vy = arg.vy;
            v_x = arg.v_x, v_y = arg.v_y;
            near_base = arg.near_base;
            threat_for = arg.threat_for;
            spelled = arg.spelled, winded = arg.winded, agressif = arg.agressif, danger = arg.danger, ignore_it = arg.ignore_it, someone_in_range = arg.someone_in_range;
            abase_x = arg.abase_x, abase_y = arg.abase_y;
            ebase_x = arg.ebase_x, ebase_y = arg.ebase_y;
            guard_x = arg.guard_x, guard_y = arg.guard_y;
            varx = arg.varx, vary = arg.vary;
            pot_baricentre = arg.pot_baricentre;
            next_action = arg.next_action;
            base_action = arg.base_action;
        }
        int     dist(entity *arg){
            if (!arg)
            {
                std::cerr << "ERROR DISTANCE AVEC ENTITY NULL " << id << std::endl;
                return (10000000);
            }
            double value = sqrt(pow((double)arg->x - (double)x,(double)2) + pow((double)arg->y - (double)y,(double)2));
            if (value > 214748364)
                value = 21474836;
            return (value);
        }
        int     dist_v(entity *arg){
            if (!arg)
            {
                std::cerr << "ERROR DISTANCEV AVEC ENTITY NULL " << id << std::endl;
                return (10000000);
            }

            double value = sqrt(pow((double)arg->x + (double)arg->vx - (double)x,2) + pow((double)arg->y + (double)arg->vy - (double)y ,2)); 
            if (value > 214748364)
                value = 21474836;

            return (value);
        }
        int     dist_base(int obase_x, int obase_y, entity *arg){
            if (!arg)
            {
                std::cerr << "ERROR DISTANCEB AVEC ENTITY NULL " << id << std::endl;
                return (10000000);
            }
            double value = sqrt(pow((double)arg->x - (double)obase_x, 2) + pow((double)arg->y - (double)obase_y, 2));
            if (value > 214748364)
                value = 21474836;
            return (value);
        }
        bool    is_in(std::vector<entity *> &tab_cibles){
            bool res = false;
            for(std::vector<entity *>::iterator it = tab_cibles.begin(); it != tab_cibles.end() && res == false; it++)
                res = *it == this ? true : res;
            return (res);
        }
        int grouped_def(std::map<int, std::vector<entity *> > &full_map, int rayon){
            int res = 0;
            for (std::map<int, std::vector<entity* > >::iterator it = full_map.begin(); it != full_map.end();it++){
                for (std::vector<entity *>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {
                    if (!(*it2)->type && dist(*it2) < rayon && this != *it2 &&  (*it2)->threat_for == 1 && !(*it2)->shield_life && (*it2)->health > 6)
                        res++; 
                }
            }
            return (res);
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
            for(std::map<int, std::map<int, std::vector<entity *> > >::iterator it = map_map_cibles.begin();it != map_map_cibles.end();it++){
                for(std::map<int, std::vector<entity *> >::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {
                    for(std::vector<entity *>::iterator it3 = it2->second.begin(); it3 != it2->second.end();it3++)
                        tab_cibles.push_back(*it3);
                }
            }
        }
        bool is_not_jj(base *arg, entity *heros);
        void my_baricentre(std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, base *arg, entity *heros);
        bool is_threat(base *arg, entity *heros, int rayon);
        void    spellTo(entity *cible, std::map<int, std::vector<entity *> > &tab_e, int vbase_x, int vbase_y, int &mana, base *arg){
                int dist_he = -1, dist_heb, dist_e = -1, dist_eb, dist_ha = -1,dist_hab ,buff = 0;
                bool jj;
                entity *he = NULL, *ha = NULL, *e = NULL;

                int small_dist = -1, small_b_dist = -1, buff_dist, buff_b_dist;
                for (std::map<int, std::vector<entity *> >::iterator it = tab_e.begin(); it != tab_e.end(); it++)
                {
                    for (std::vector<entity *>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                    {
                        buff_dist = dist(*it2), buff_b_dist = dist_base(vbase_x, vbase_y, *it2);
                        if ((*it2)->type == 2 && dist_he == -1)
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
                // vbase_x = ebase_x == vbase_x ? 17630 - vbase_x : vbase_x;
                // vbase_y = ebase_y == vbase_y ? 9000 - vbase_y : vbase_y;
                if (e && !jj && dist_e < 2200 && e->threat_for == 2  && e->is_threat(&base_ennemy, he == NULL ? this : he, 150) == true && !e->shield_life && e->dist_base(ebase_x, ebase_y, e) < 4800){
                    std::cerr << "SHIELD DE SPELLTOC" << std::endl;
                    next_action.assign("SPELL SHIELD " + std::to_string(e->id)), e->spelled = true;
                }
                else if (!shield_life && !spelled && turn > 50 && he && ha && dist_ha < 2200 && dist_base(vbase_x, vbase_y,he) < 10000 && ha->spelled == false && is_not_jj(arg, ha) == true){
                    std::cerr << "SHIELD DE SPELLTO " << std::endl;
                    next_action.assign("SPELL SHIELD " + std::to_string(ha->id));
                    ha->spelled = true;
                }
                else if (turn > 200 && he && dist_he < 2200 && he->spelled == false && !(he)->shield_life && !jj && he->grouped_def(tab_e, 2200) > 2){
                    std::cerr << "CONTROL DE SPELLTO " << std::endl;
                    next_action.assign("SPELL CONTROL " + std::to_string(he->id) + " " + std::to_string(!vbase_x ? 17630 : 0) + " " + std::to_string(vbase_y == 0 ? 9000 : 0));
                    he->spelled = true;
                }
                else if (turn > 50 && e && !jj && dist_e < 2200 && e->spelled == false && e != cible && dist_e > 800 && e->threat_for != 2 && e->someone_in_range == false && dist_base(vbase_x, vbase_y, cible) < 800 && dist_base(vbase_x, vbase_y, cible) > 400){
                    std::cerr << "CONTROL2 DE SPELLTO " <<  dist(e) << std::endl;
                    next_action.assign("SPELL CONTROL " + std::to_string(e->id) + " " + std::to_string(!vbase_x ? 17630 : 0) + " " + std::to_string(vbase_y == 0 ? 9000 : 0));
                    e->spelled = true;
                }
                // else if (turn > 50 && he && !shield_life && dist_base(vbase_x, vbase_y,he) < 10000 && spelled == false && jj == false){
                    // std::cerr << "SHIELD2 DE SPELLTO " << std::endl;
                    // next_action.assign("SPELL SHIELD " + std::to_string(id));
                    // spelled = true;
                // }
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
            bool res = (sqrt(pow(arg->x - guard_x, 2) + pow(arg->y - guard_y, 2)) + dist_base(abase_x, abase_y, arg)) <= 8800 || dist_base(abase_x, abase_y, arg) <= 7000;
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
                    if (!(*it2)->type && !(*it2)->shield_life && dist(*it2) < rayon)
                    {
                        (*it2)->ignore_it = true;
                        (*it2)->winded = winded, (*it2)->spelled = spelled;
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
        int flag4_x, flag4_y;
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
        std::vector<entity *> tab_h;
    public:
        base(int x, int y): base_x(x), base_y(y), mana(0), uusafe_dist_per_damage(100), usafe_dist_per_damage(300), safe_dist_per_damage(200), max_def(10000){
            max_x = !x ? 5000: x - 5000;
            max_y = !y ? 2000 : y - 2000;
            flag0_x = !x ? 2000 : x - 2000;
            flag0_y = !y ? 5000 : y - 5000;
            flag0_xn = flag0_x;
            flag0_yn = flag0_y;
            flag1_x = max_x;
            flag1_y = max_y;
            flag1_xn = flag1_x;
            flag1_yn = flag1_y;
            flag2_x = !x ? 17630 - 7500 : 7500;
            flag2_y = !y ? 9000 - 1000 : 1000;
            flag2_xn = flag2_x;
            flag2_yn = flag2_y;
            flag3_x = !x ? 17630 - 6000 : 6000;
            flag4_x = !x ? 17630 - 1000 : 1000;
            flag3_y = !y ? 9000 - 1000 : 1000 ;
            flag4_y = !y ? 9000 - 6000 : 6000 ;
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
            {
                std::cerr << "ERROR B_DISTANCE AVEC ENTITY NULL " << std::endl;
                return (10000000);
            }
            double value = sqrt(pow(arg->x - base_x,2) + pow(arg->y - base_y,2));
            if (value > 214748364)
                value = 21474836;
            return (value);
        }

        void add_to_def(entity *arg){
            if (!arg->type && arg->dist_base(base_x, base_y, arg) < 15000)
                order_defense[dist(arg)].push_back(arg);
            else if (arg->type == 1)
                order_heros[dist(arg)].push_back(arg), tab_h.push_back(arg);
            full_map[dist(arg)].push_back(arg);
            // if (arg->id == 67)
                // std::cerr << "Entite 67 posx, posy : " << arg->x << ", " << arg->y << " et je serais " << arg->vx + arg->x << ", " << arg->y + arg->vy << std::endl; 
        }
        bool toward_b(entity *arg){
            if (!arg)
            {
                std::cerr << "ERROR toward_b AVEC ENTITY NULL " << std::endl;
                return (false);
            }
            double value = sqrt(pow((arg->x + arg->vx), 2) + pow((arg->y + arg->vy), 2));
            if (value > 214748364)
                value = 21474836;
            return (value <= dist(arg)); 
        }
        void    clear_o_d(){
            safe_dist_per_damage = 300;
            usafe_dist_per_damage = 400;
            uusafe_dist_per_damage = 500;
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
                    if (dist(*it2) < 4000)
                        it2++;
                    else if ((*it2)->is_threat(this, first_heros, safe_dist_per_damage) == false)
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
            // if ((first_elem)->is_threat(this, first_heros, 150) == true && (first_elem)->is_threat(this, second_heros, 150) == true)
                // return(full_comportement());
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
                full_comportement();
                if (first_heros->close_to_guard(order_defense.begin()->second.front()) == false && second_heros->close_to_guard(order_defense.begin()->second.front()) == false)
                {
                    order_defense.clear();
                    return ;
                }
                if (oldtaille != ntaille){
                    entity *first = NULL;
                    for(std::map<int, std::vector<entity *> >::iterator it = temp.begin(); it != temp.end();it++)
                    {
                        int summ, sum;
                        for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++){
                            if (second_heros->close_to_guard(*it2) == false)
                                continue;
                            buff_dist = second_heros->dist(*it2), buff_b_dist = dist(*it2) * 2,  sum = buff_dist + buff_b_dist;
                            if (small_dist == -1 && *it2 != first_elem)
                                small_dist = buff_dist, small_b_dist = buff_b_dist, first = *it2, summ = buff_dist + buff_b_dist;
                            else if (sum < summ && *it2 != first_elem)
                                small_dist = buff_dist, small_b_dist = buff_b_dist, first = *it2, summ = buff_dist + buff_b_dist;
                        }
                    }
                    if (first)
                        order_defense.begin()->second.push_back(first);
                }
                return ;
            }
            first_heros = *(order_heros.begin()->second.begin());
            second_heros = (++(order_heros.begin()->second.begin()) != (order_heros.begin()->second.end()) ? *(++(order_heros.begin()->second.begin())) : *((++order_heros.begin())->second.begin()));
            third_heros = (((--order_heros.end())->second.back()));
            entity *first = NULL;
            entity *second = NULL;
            order_defense.clear();
            small_dist = -1, small_b_dist = -1;
            int buff_dist2, buff_b_dist2;
            int small_dist2 = -1, small_b_dist2 = -1;
            std::map<int, entity *> map_heros1;
            std::map<int, entity *> map_heros2;
            full_comportement();
            for (std::map<int, std::vector<entity *> >::iterator itt = temp.begin(); itt != temp.end();itt++)
            {
                for (std::vector<entity *>::iterator it2 = itt->second.begin(); it2 < itt->second.end();it2++)
                {
                    buff_dist = first_heros->dist(*it2), buff_b_dist = dist(*it2) * 2 + first_heros->dist_to_guard(*it2);
                    buff_dist2 = second_heros->dist(*it2), buff_b_dist2 = dist(*it2) * 2 + second_heros->dist_to_guard(*it2);
                    if (first_heros->close_to_guard(*it2) == true){
                        if (map_heros1.find(buff_b_dist) != map_heros1.end())
                            map_heros1[buff_b_dist] = ((first_heros->dist(map_heros1[buff_b_dist])) > buff_dist ? (*it2) : map_heros1[buff_b_dist]); 
                        else
                            map_heros1[buff_b_dist] = *it2;
                    }
                    if (second_heros->close_to_guard(*it2) == true){
                        if (map_heros2.find(buff_b_dist2) != map_heros2.end())
                            map_heros2[buff_b_dist2] = second_heros->dist(map_heros2[buff_b_dist2]) > buff_dist2 ? (*it2) : map_heros2[buff_b_dist2]; 
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
            if (flag == 2  && (*ih2)->x == flag3_xn)
                flag3_xn = flag3_xn == flag3_x ? flag4_x : flag3_x, flag3_yn = flag3_yn == flag3_y ? flag4_y : flag3_y;
            if (flag == 2 && turn < 30)
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
            int limit_ronde = 200000;

            // if (turn < 100)
                // limit_ronde = 5000;
            for(std::map<int, std::vector<entity *> >::iterator ih = order_heros.begin(); ih != order_heros.end();ih++){
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
            name[2] += " FARM";
            entity *first = NULL;
            entity *second = NULL;
            std::vector<entity*>::iterator ith;
            bool    someone_have_it;
            int     max_pot_bari = 0; 
            int     group = 0, group2, dist2, sum_dist, max_dist = 8000, dist_e = max_dist - 1, dist_pb = max_dist - 1;
            for(std::map<int, std::vector<entity*> >::iterator it = full_map.begin(); it != full_map.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                {
                    dist2 = arg->dist_base(ebase_x, ebase_y, *it2);
                    if (((*it2)->threat_for == 2 && turn > 50) || (*it2)->type || dist2 > max_dist || (*it2)->ignore_it == true)
                        continue;
                    sum_dist = dist2 + arg->dist(*it2) - ((*it2)->health * 100);
                    group2 = (*it2)->size_baricentre(full_map, ebase_x, ebase_y, this, arg);
                    for(someone_have_it = false, ith = tab_h.begin(); ith != tab_h.end() && someone_have_it == false;)
                        someone_have_it = sqrt(pow((*it2)->varx - (*ith)->x, 2) + pow((*it2)->vary - (*ith)->y, 2)) < 1600 && *ith != arg ? true : false, ith += someone_have_it ? 0 : 1;
                    if (someone_have_it == true)
                        continue; 
                    // for(someone_have_it = false, ith = tab_h.begin(); ith != tab_h.end() && someone_have_it == false && (turn > 40) ;)
                        // someone_have_it = (*ith)->threat_for == 2, ith += someone_have_it ? 0 : 1;
                    // if (someone_have_it == true)
                        // continue; 
                    if (!first && ((*it2)->threat_for != 2 || turn < 80))
                        first = *it2, group = group2, dist_e = sum_dist;
                    else if ((group2 > group || ((dist_e > sum_dist && (*it2)->threat_for > first->threat_for) && group == group2) && ((*it2)->threat_for != 2 || turn < 80)))
                        first = *it2, group = group2, dist_e = sum_dist;
                    if (group > 1)
                        max_pot_bari = 0;
                    else if (max_pot_bari < (*it2)->pot_baricentre || (dist_pb > sum_dist && max_pot_bari == (*it2)->pot_baricentre) && ((*it2)->threat_for != 2 || turn < 80))
                    {
                        max_pot_bari = (*it2)->pot_baricentre;
                        second = (*it2);
                        dist_pb = sum_dist;
                    }

                }
            }
            if (first && !max_pot_bari)
                std::cerr << "1 - Le mob a farm est " << first->id << " !" << std::endl;
            if (second)
                std::cerr << "2 - Le mob a farm est " << second->id << " !" << max_pot_bari << std::endl;
            if (max_pot_bari > 2 && second && !second->type && (second->threat_for != 2))
                return (arg->moveTo(second, full_map, ebase_x, ebase_y, mana, this));
            else if (first && !first->type)
                return (arg->moveTo(first, full_map, ebase_x, ebase_y, mana, this));
        }
        void    j_j(entity *arg){
            std::map<int, std::vector<entity *> > map_toj, temp;
            std::vector<entity *> close_heros;
            entity *pot_heros;
            static bool launcher_ready = false, launching = false;
            name[2] = "J_J";
            for(std::map<int, std::vector<entity*> >::iterator it = full_map.begin(); it != full_map.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                {
                    if ((*it2) != arg && !(*it2)->shield_life && arg->dist_base(ebase_x, ebase_y, *it2) < 12000)
                        map_toj[(*it2)->base_ennemy.dist(*it2)].push_back(*it2);
                    if ((*it2)->type == 2)
                        close_heros.push_back(*it2);
                }
            }
            if (!map_toj.size() || (mana < 20 && launching == false && launcher_ready == false))
                return (go_farm(arg, arg));
            temp = map_toj;
            entity *elem;
            int elem_d_base, heros_d_base;
            int tempx, tempy;
            bool around = false;
            for(;map_toj.size();){
                elem = (*(map_toj.begin()->second.begin()));

                pot_heros = arg;
                if (close_heros.size() > 1)
                {
                    int sum_dist, min_sum_dist = -1;
                    around = false;
                    for(std::vector<entity*>::iterator it = close_heros.begin(); it != close_heros.end();it++)
                    {
                        sum_dist = (*it)->dist(elem) + arg->dist_base(ebase_x, ebase_y, *it);
                        pot_heros = pot_heros == arg ? *it : pot_heros, min_sum_dist = min_sum_dist == -1 ? sum_dist : min_sum_dist;
                        if (sum_dist < min_sum_dist)
                            pot_heros = *it;
                        if ((*it)->dist(elem) <= 1280)
                            around = true;
                    }
                }
                elem_d_base = arg->dist_base(ebase_x, ebase_y, elem);
                heros_d_base = arg->dist_base(ebase_x, ebase_y, arg);
                double dist_eb = arg->dist_base(ebase_x, ebase_y, elem);
                double xxi = ebase_x - (((dist_eb + 1100) * ( ebase_x - (elem->x + elem->vx))) / dist_eb);
                double xi = ebase_x - (((dist_eb - 1100) * ( ebase_x - (elem->x + elem->vx))) / dist_eb);
                double yyi = ebase_y - (((dist_eb + 1100) * ( ebase_y - (elem->y + elem->vy))) / dist_eb);
                double yi = ebase_y - (((dist_eb - 1100) * ( ebase_y - (elem->y + elem->vy))) / dist_eb);
                double axi = xi, ayi = yi;
                entity temp(*elem);
                if (sqrt(pow(xi - arg->x, 2) + pow(yi - arg->y, 2)) > 800)
                    temp.x += temp.vx, temp.y += temp.vy, dist_eb = arg->dist_base(ebase_x, ebase_y, &temp), xi = ebase_x - (((dist_eb - 1100) * ( ebase_x - (temp.x + temp.vx))) / dist_eb), yi = ebase_y - (((dist_eb - 1100) * ( ebase_y - (temp.y + temp.vy))) / dist_eb);
                int wind_dist = sqrt(pow(xxi - arg->x, 2) + pow(yyi - arg->y, 2));
                int launch_dist = sqrt(pow(xi - arg->x, 2) + pow(yi - arg->y, 2));
                int elem_d_base_c = sqrt(pow(ebase_x - temp.x - temp.vx, 2) + pow(ebase_y - temp.y - temp.vy, 2));
                std::cerr << "ID : DIST : FUTURDIST: " << elem->id << " : " <<dist_eb <<" : " << elem_d_base_c <<  std::endl;
                if (elem->shield_life || arg->dist(elem) > 2200)
                {
                    map_toj.begin()->second.erase(map_toj.begin()->second.begin());
                    if (!map_toj.begin()->second.size())
                        map_toj.erase(map_toj.begin());
                    continue;
                }
                else if (!elem->type  && elem->health > 8 && launcher_ready == false  && (launching == true ||  (elem_d_base_c < 7000 && elem_d_base > heros_d_base)) && launch_dist < 1600){
                    if (sqrt(pow(axi - arg->x, 2) + pow(ayi - arg->y, 2)) > 800)
                        launching = true;
                    else
                        launching = false;
                    if (launching == false)
                        name[2] += " CHED " + std::to_string(elem->id);
                    else
                        name[2] += " CHING " + std::to_string(elem->id);
                    tempx = std::max(1, static_cast<int>(xi)), tempx = std::min(17629, static_cast<int>(xi)), tempy = std::max(1, static_cast<int>(yi)), tempy = std::min(8999, static_cast<int>(yi));
                    launcher_ready = launching != true;
                    std::cerr << launcher_ready << " :DIST: " << arg->dist(elem) << " proc tour: " << sqrt(pow(xi - elem->x - elem->vx, 2) + pow(yi - elem->y- elem->vy, 2)) << std::endl;
                    arg->next_action.assign("MOVE " + std::to_string(static_cast<int>(xi)) + " " + std::to_string(static_cast<int>(yi)));
                    return;
                }
                else if (launching == false && arg->dist(elem) >= 1280 && wind_dist <= 800  && !elem->type  && elem->health > 8 && elem_d_base <= heros_d_base  && (elem_d_base_c < 7000 || around)){
                    name[2] += " JJs " + std::to_string(elem->id);
                    launching = false, launcher_ready = true;
                    arg->next_action.assign("MOVE " + std::to_string(static_cast<int>(xxi)) + " " + std::to_string(static_cast<int>(yyi)));
                    return;
                }
                else if (mana > 9 && launching == false && !elem->type && (elem->health > 8 || launcher_ready) && arg->dist(elem) <= 1280 && (launcher_ready || elem_d_base < 5000 || around)){
                    if (launcher_ready)
                        name[2] += " BOOOOM " + std::to_string(elem->id);
                    launching = false, launcher_ready = false;
                    std::cerr << "VER 2 WIND sur " << elem->id << " a une distance " << arg->dist(elem) << std::endl;
                    arg->next_action.assign("SPELL WIND " + std::to_string(arg->x - elem->x + ebase_x) + " " + std::to_string(arg->y - elem->y + ebase_y)), elem->spelled = true, elem->winded = true;
                }
                else if (mana > 9 && elem->type == 1 && arg->dist(elem) < 2200)
                    arg->next_action.assign("SPELL SHIELD " + std::to_string(elem->id)), launching = false, launcher_ready = false;
                else if (mana > 9 && !elem->type && elem->threat_for == 2  && elem->is_threat(&arg->base_ennemy, arg, 50) == true && !arg->shield_life && arg->health > 80)
                {
                    std::cerr << "VER 2 SHIELD" << std::endl;
                    launching = false, launcher_ready = false;
                    arg->next_action.assign("SPELL SHIELD " + std::to_string(elem->id)), elem->spelled = true;
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
            launching = false, launcher_ready = false;
            return (go_farm(arg, arg));
        }

        bool    check_map(entity *elem){
            for(std::map<int, std::vector<entity*> >::iterator it = full_map.begin(); it != full_map.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++){
                    if ((*it2)->type == 2 && elem->dist(*it2) <= 1280)
                        return (true);
                }
            }
            return (false);
        }

        void    define_next_actions( void ){
            std::map<int, std::vector<entity*> >::iterator ih = order_heros.begin();
            std::map<int, std::vector<entity*> >::iterator ie;
            entity *elem;
    
            for(int i = 0, varx, vary; ih != order_heros.end(); ih++)
            {
                for(std::vector<entity *>::iterator ih2 = ih->second.begin(); ih2 != ih->second.end(); ih2++, i++)
                {
                    if (map_flag.find((*ih2)->id) != map_flag.end() && (map_flag[(*ih2)->id] == 2))
                    {
                        j_j(*ih2);
                        continue;
                    }
                    if (i  > 0)
                    {
                        for(;taille();){
                            ie = order_defense.begin();
                            elem = (*(ie->second.begin()));
                            if ((*ih2)->close_to_guard(elem))
                                break;
                            ie->second.erase(ie->second.begin());
                            if (!ie->second.size())
                                order_defense.erase(order_defense.begin());
                        }
                    }
                    if(!taille())
                        continue;
                    
                    ie = order_defense.begin();
                    elem = (*(ie->second.begin()));
                    int dist_e = (*ih2)->dist(elem);
                    int dist_b = dist(elem);
                    std::cerr << "La cible de " << (*ih2)->id << " est " << elem->id << std::endl;
                    elem->someone_in_range = check_map(elem);
                    if (elem->shield_life || mana < 10 || (elem->health < 5 && dist_b > 1000 && elem->someone_in_range != true))
                        (*ih2)->moveTo(elem, full_map, base_x, base_y, mana, this);
                    else if (dist_e < 1280 && ((elem->is_threat(this, (*ih2), safe_dist_per_damage) == true && dist(elem) < 1000) || elem->someone_in_range == true)){
                        std::cerr << "WIND DEFINE SUR " << elem->id << std::endl; 
                        (*ih2)->next_action.assign("SPELL WIND " + std::to_string((*ih2)->x + elem->x - base_x) + " " + std::to_string((*ih2)->y + elem->y - base_y)), elem->spelled = true, elem->winded = true;
                        mana -= 10;
                    }
                    else if (dist_e < 2200 && dist_e > 1600 && elem->health > 15 && elem->is_threat(this, (*ih2), uusafe_dist_per_damage) == false && elem->spelled == false && elem->threat_for != 2 && !(*ih2)->grouped_def(full_map, 800)){
                        std::cerr << "NOUV SPELL CONTROL DEFINE" << std::endl;
                        (*ih2)->next_action.assign("SPELL CONTROL " + std::to_string(elem->id) + " " + std::to_string(!base_x ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0)), elem->spelled = true;
                        mana -= 10;
                    }
                    else if (dist_e < 2200 && (dist_b < dist(*ih2)) && elem->spelled == false && toward_b((*ih2)) && dist_b > 400 && dist_b < 900){
                        (*ih2)->next_action.assign("SPELL CONTROL " + std::to_string(elem->id) + " " + std::to_string(!base_x ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0)), elem->spelled = true;
                        mana -= 10;
                    }
                    else
                        (*ih2)->moveTo(elem, full_map, base_x, base_y, mana, this);
                    if (elem->winded == true && elem->is_threat(this, (*ih2), 110) == false){
                        (*ih2)->erase_around(this, order_defense, 1280);
                    }
                    else if (elem->winded == false && elem->is_threat(this, (*ih2), 200) == false){
                        (*ih2)->erase_around(this, order_defense, 800);
                    }
                    else /* if (elem->is_threat(this, (*ih2), 150) == false || elem->health < 3) */
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
            tab_h.clear();
            full_map.clear();
        }
        ~base(){full_clear();}
};

bool entity::is_not_jj(base *arg, entity *heros){
    return (arg->map_flag.find(heros->id) != arg->map_flag.end() && arg->map_flag[heros->id] != 2);
}


bool    entity::is_threat(base *arg, entity *heros,int case_degat){
    int dist_total = arg->dist(this) - (dist(heros)/2), c_health = health > 2 ? health : 2;
    dist_total = (dist_total / case_degat);
    if (arg->base_x != heros->base_ennemy.base_x)
    {
        dist_total = !dist_total ? 2 : dist_total;
        c_health = c_health % 2 ? c_health + 1 : c_health;
        dist_total = dist_total % 2 ? dist_total + 1 : dist_total;
    }
    bool temp = (c_health >= dist_total);
   return (temp);
}

void    entity::moveTo(entity *cible, std::map<int, std::vector<entity *> > &tab_e, int base_x, int base_y, int &mana, base *arg){
    std::vector<entity *> tab_bar, temp_bar;    
    entity *new_cible = cible;
    int bar_size = cible->size_baricentre(tab_e, base_x, base_y, arg, this), buff_bar_s;
    bool    can_spell = true;

    tab_bar = arg->t_c, temp_bar = tab_bar;
    for(std::vector<entity *>::iterator it = tab_bar.begin(); it != tab_bar.end() && tab_bar.size() > 1; it++)
    {
        buff_bar_s = (*it)->size_baricentre(tab_e, base_x, base_y, arg, this);
        if (buff_bar_s > bar_size && cible->is_in(arg->t_c) == true)
            new_cible = *it, bar_size = buff_bar_s, temp_bar = arg->t_c;
    }
    cible = new_cible;
    cible->varx = std::max(1, cible->varx), cible->varx = std::min(17629, cible->varx), cible->vary = std::max(1, cible->vary), cible->vary = std::min(8999, cible->vary);
    for(std::vector<entity *>::iterator it = temp_bar.begin(); it != temp_bar.end() && temp_bar.size() > 1 && can_spell == true; it++)
    {
        if (dist(*it) > 800)
            can_spell = false;
    }
    if (mana >= 10 && temp_bar.size() && can_spell == true)
        return (spellTo(cible, tab_e, base_x, base_y, mana, arg));
    if (mana >= 10 && bar_size < 2 && (sqrt(pow(cible->varx - x,2) + pow(cible->vary - y,2)) < 800))
        return (spellTo(cible, tab_e, base_x, base_y, mana, arg));
    if (bar_size < 2 && sqrt(pow(cible->varx - x,2) + pow(cible->vary - y, 2)) > 800)
        cible->varx += (v_x), cible->vary += (v_y);
    if (bar_size < 2 && sqrt(pow(cible->varx - x,2) + pow(cible->vary - y, 2)) < 800)
    {
        double diff = 800 - sqrt(pow(cible->varx - x,2) + pow(cible->vary - y, 2));
        double elem_d_base = dist_base(base_x, base_y, cible);
        double heros_d_base = dist_base(base_x, base_y, this);
        double dist_eb = dist_base(base_x, base_y, cible);
        cible->varx = base_x - (((dist_eb - diff) * ( base_x - (cible->x + cible->vx))) / dist_eb);
        cible->vary = base_y - (((dist_eb - diff) * ( base_y - (cible->y + cible->vy))) / dist_eb);
        // (*ih2)->x + elem->x - base_x)std::to_string((*ih2)->y + elem->y - base_y)), elem->spelled = true, elem->winded = true;
    }
    // if (bar_size > 1)
        // mana += 2 * bar_size;
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

    tab.reserve(100);
    std::cin >> x >> y; std::cin.ignore();
    base base_ally(x, y), base_enemy(17630 - x, 9000 - y);
    std::cin >> heroes_per_player; std::cin.ignore();
    while (1) {
        tab.clear();
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