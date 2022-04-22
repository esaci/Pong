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
        std::string next_action;
        std::string base_action;
        entity(std::vector<int> &tab, int xi, int yi): spelled(false), on_cible(0){
            int i = 0;id = tab[i++];type = tab[i++];x = tab[i++];y = tab[i++];shield_life = tab[i++];is_controlled = tab[i++];health = tab[i++];vx = tab[i++];vy = tab[i++];near_base = tab[i++];threat_for = tab[i++];
        }
        private:
            entity();
};

class base{
    public:
        int base_x; // The corner of the map representing your base
        int base_y;
        int max_x;
        int max_y;
        int health;
        int mana;
        std::map<int, std::vector<entity*> > order_defense;
        std::map<int, std::vector<entity*> >  order_heros;
        std::map<int, int> map_flag;
    public:
        base(int x, int y): base_x(x), base_y(y), mana(0){
            max_x = 3500;
            if (x != 0)
                max_x = x - 3500;
            max_y = 3500;
            if (y != 0)
                max_y = y - 3500;
        }
        void sethm(int h, int m){health = h; mana += m;}
        
        int to_a_b_distance(entity *arg){
			int rayonLarge = turn > 50 ? 5000 : 10000;
            int res = 0, x = arg->x, y = arg->y;
            for (; x > 0 && y > 0 && x < 17631 && y < 9001;res++, x += (arg->vx / 10), y += (arg->vy / 10))
            {
				if (pow(x - base_x, 2) + pow(y - base_y, 2) < pow(rayonLarge, 2))
                    return (res);
            }
            return(-1);
        }

        int to_b_distance(entity *arg){
            int res = 0, x = arg->x, y = arg->y;
			int rayonSerre = turn > 50 ? 300 : 1000;
            for (; x > 0 && y > 0 && x < 17631 && y < 9001;res++, x += (arg->vx / 10), y += (arg->vy / 10))
            {
				if (pow(x - base_x, 2) + pow(y - base_y, 2) < pow(rayonSerre, 2))
                    return (res);
            }
            return(-1);
        }
        void add_to_def(entity *arg){
            int res = to_b_distance(arg);
            int	add_res = turn > 50 ? 190 : 400;
            if (res != -1)
            {
                order_defense[sqrt(pow(arg->x - base_x,2) + pow(arg->y - base_y,2))].push_back(arg);
                return ;
            }
            res = to_a_b_distance(arg);
            if (res == -1)
            {
                delete arg;
                return;
            }
            order_defense[sqrt(pow(arg->x - base_x,2) + pow(arg->y - base_y,2))].push_back(arg);
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
            for(std::map<int, std::vector <entity *> >::iterator ih = order_heros.begin(); ih != order_heros.end(); ih++)
            {
                for(std::vector<entity *>::iterator ih3 = ih->second.begin(); ih3 != ih->second.end(); ih3++, i++)
                {
                    if (ih3 == ih2)
                        flag = i;
                }
            }
            if (map_flag.find((*ih2)->id) != map_flag.end())
                flag = (map_flag[(*ih2)->id] + flag) == 1 ? map_flag[(*ih2)->id] : flag; 
            int xi = max_x, yi = max_y; 
            if (turn < 50)
                xi = xi == 3500 ? 5000 : 17631 - 5000, yi = yi == 3500 ? 5000 : 9000 - 5000;
            if (flag == 2 /* && turn < 50 */)
                res = "MOVE " + std::to_string(xi/2) + " " + std::to_string(yi/2);
            // else if (flag == 2)
                // res = "MOVE " + std::to_string(xi != 3500 ? 3500 : 17631 - 3500) + " " + std::to_string(yi != 3500 ? 3500 : 9000 - 3500);
            else if (flag == 1)
                res = "MOVE " + std::to_string(xi/8) + " " + std::to_string(yi/2);
            else
                res = "MOVE " + std::to_string(xi/2) + " " + std::to_string(yi/8);
            map_flag[(*ih2)->id] = flag;
            return (res);
        }

        void    define_next_actions( void ){
            std::map<int, std::vector<entity*> >::iterator ih = order_heros.begin();
            std::map<int, std::vector<entity*> >::iterator ie;
            int v_xy;

            // if (!order_defense.size())
            for(int i = 0, varx, vary; ih != order_heros.end(); ih++)
            {
                v_xy = 400;
                for(std::vector<entity *>::iterator ih2 = ih->second.begin(); ih2 != ih->second.end(); ih2++, i++, v_xy = 400)
                {
                    // std::cerr << "On a " << taille() << " cibles!" << std::endl;
                    ie = order_defense.begin();
                    if(!taille())
                    {
                        (*ih2)->next_action.assign(comportement(ih2));
                        continue;
                    }
                    if((*ih2)->spelled)
                        v_xy *= (-1);
                    varx = (base_x > (*(ie->second.begin()))->x ? v_xy : -v_xy ) + (*(ie->second.begin()))->x, vary = (base_y > (*(ie->second.begin()))->y ? v_xy: -v_xy ) + (*(ie->second.begin()))->y;
                    varx = std::max(1, varx), varx = std::min(17629, varx), vary = std::max(1, vary), vary = std::min(8999, vary);

                    int dist_e = sqrt(pow((*ih2)->x - (*(ie->second.begin()))->x, 2) +pow((*ih2)->y - (*(ie->second.begin()))->y, 2));
                    int dist_b = sqrt(pow(base_x - (*(ie->second.begin()))->x, 2) +pow(base_y - (*(ie->second.begin()))->y, 2));
                    std::cerr << "Mana : " << mana << "\nDistance entre Hero " << (*ih2)->id << " et entity" << (*(ie->second.begin()))->id <<": " << dist_e << "\nEntity et Base: " << dist_b << std::endl;
                    if (dist_b > 1000 || (*ih2)->spelled)
                        (*ih2)->next_action.assign("MOVE " + std::to_string(varx) + " " + std::to_string(vary));
                    else if (dist_e < 1280 && mana > 10)
                    {
                        (*ih2)->next_action.assign("SPELL WIND " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                        mana -= 10;
                        (*ih2)->spelled = true;
                    }
                    else if (dist_e < 2100 && mana > 10)
                    {
                        (*ih2)->next_action.assign("SPELL CONTROL " + std::to_string((*(ie->second.begin()))->id) + " " + std::to_string(base_x == 0 ? 17630 : 0) + " " + std::to_string(base_y == 0 ? 9000 : 0));
                        mana -= 10;
                        (*ih2)->spelled = true;
                    }
                    else
                        (*ih2)->next_action.assign("MOVE " + std::to_string(varx) + " " + std::to_string(vary));
                    if ((taille() + i >= tailleh() && (ie->first > 10)) || (*(ie->second.begin()))->on_cible)
                    {
                        (*(ie->second.begin()))->on_cible = 0;
                        delete *(ie->second.begin());
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
                        it3 = it2, it4 = it;
                }
            }
            std::cout << (*it3)->next_action << std::endl;
            delete *it3;
            it4->second.erase((it3));
            if (!it4->second.size())
                order_heros.erase(it4);
            
        }
        void    full_clear( void ){
            for(std::map<int, std::vector<entity*> >::iterator it = order_defense.begin(); it != order_defense.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                    delete *it2;
            }
            for(std::map<int, std::vector<entity*> >::iterator it = order_heros.begin(); it != order_heros.end();it++)
            {
                for(std::vector<entity*>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
                    delete *it2;
            }
            order_heros.clear();
            order_defense.clear();
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
    base base_ally(x, y), base_ennemy(17630 - x, 9000 - y);
    std::cin >> heroes_per_player; std::cin.ignore();
    while (1) {
        for (int i = 0; i < 2; i++) {
            std::cin >> x >> y; std::cin.ignore();
            base_ally.sethm(x, y);
        }
        std::cin >> entity_count; std::cin.ignore();
        for (int i = 0; i < entity_count; i++) {
            x = 0;
            std::cin >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x++] >> tab[x]; std::cin.ignore();
            tmp = new entity(tab, base_ally.max_x, base_ally.max_y);

            if (!tmp->type)
                base_ally.add_to_def(tmp);
            else if (tmp->type == 1)
                base_ally.order_heros[sqrt((pow((tmp->x - base_ally.base_x), 2))+ (pow((tmp->y - base_ally.base_y), 2)))- 300].push_back(tmp);
            else
                delete tmp;
        }
        base_ally.define_next_actions();
        it = base_ally.order_defense.begin();
        for (int i = 0, a = 1; i < heroes_per_player; i++) {
            base_ally.do_next_action();
        }
        base_ally.full_clear();
        turn++;
    }
}